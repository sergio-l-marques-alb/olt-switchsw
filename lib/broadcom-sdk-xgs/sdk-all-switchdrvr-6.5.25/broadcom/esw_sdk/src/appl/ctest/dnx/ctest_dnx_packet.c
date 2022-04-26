/**
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        ctest_dnx_packet.c
 * Purpose:     Utility to send packet and verify signals as a result
 * Requires:
 */
#include <sal/compiler.h>
#include <sal/types.h>
#include <soc/sand/sand_signals.h>

#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/diag/dnx/diag_dnx_utils.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <appl/reference/dnx/appl_ref_dynamic_port.h>

#include <soc/types.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <include/soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/sand/auto_generated/dbx_pre_compiled_common.h>
#include <soc/sand/auto_generated/dbx_pre_compiled_jr2_devices.h>

#include <include/appl/diag/dnx/diag_dnx_utils.h>
#include <include/appl/diag/dnx/diag_dnx_l2.h>

#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/sat/sat.h>

#include <shared/shrextend/shrextend_error.h>
#include <shared/dbx/dbx_file.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/gport.h>

#include <sal/appl/sal.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

extern sh_pkt_sat_identifier_info_t pkt_sat_identifier_info;

static shr_error_e
ctest_dnx_verify_perform(
    int unit,
    int core,
    xml_node curSet)
{
    xml_node curSignal;
    char name[RHNAME_MAX_SIZE];
    char signal_n[RHNAME_MAX_SIZE], block_n[RHNAME_MAX_SIZE], from_n[RHNAME_MAX_SIZE];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    RHDATA_GET_STR_DEF_NULL(curSet, "name", name);

    PRT_TITLE_SET("Signal for test:%s", name);
    PRT_COLUMN_ADD("Signal");
    PRT_COLUMN_ADD("From");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Expected");
    PRT_COLUMN_ADD("Observed");
    PRT_COLUMN_ADD("Status");
    RHDATA_ITERATOR(curSignal, curSet, "signal")
    {
        char expected_value[DSIG_MAX_SIZE_STR], return_value[DSIG_MAX_SIZE_STR];
        RHDATA_GET_STR_DEF_NULL(curSignal, "name", signal_n);
        RHDATA_GET_STR_DEF_NULL(curSignal, "block", block_n);
        RHDATA_GET_STR_DEF_NULL(curSignal, "from", from_n);
        RHDATA_GET_STR_STOP(curSignal, "value", expected_value);
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", signal_n);
        PRT_CELL_SET("%s", from_n);
        PRT_CELL_SET("%s", block_n);
        PRT_CELL_SET("%s", expected_value);

        SHR_SET_CURRENT_ERR(sand_signal_verify(unit, core, block_n, from_n, NULL, signal_n,
                                               NULL, 0, expected_value, return_value, 0));
        switch (SHR_GET_CURRENT_ERR())
        {
            case _SHR_E_NOT_FOUND:
                PRT_CELL_SKIP(1);
                PRT_CELL_SET("Not Found");
                break;
            case _SHR_E_FAIL:
                PRT_CELL_SET("%s", return_value);
                PRT_CELL_SET("Oops");
                break;
            case _SHR_E_NONE:
                PRT_CELL_SKIP(1);
                PRT_CELL_SET("OK");
                break;
            default:
                PRT_CELL_SKIP(1);
                PRT_CELL_SET("Error");
                break;
        }
    }
    PRT_COMMIT;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief Send the packet and verify the results of run, based on definitions in XML file.
 *    In DNX-Devices.xml should be following section
 *   <case-objects>
 *       <include file="TestCases.xml" device_specific="1" type="tests"/>
 *   </case-objects>
 *   You may add unlimited number of XML files in above format, using type="tests"
 *   If the test file is specific per device it should be placed under $DB/device and marked as device_specific=1
 *   If the test file is applicable for all, it should be under $DB and device_specific=0
 *   Use $SDK/tools/sand/db/jericho2_a0/TestCases.xml as primary file with examples inside,, do not delete default packets
 *   and signal set, thay are used for code verification
 * \param [in] unit - device id
 * \param [in] core - core  id
 * \param [in] set_n - signal set for test verification, signal set with this name should be present in signals section
 *
 * \retval _SHR_E_NONE if test was found, performed and verified
 * \retval _SHR_E_NOT_FOUND if there was no such test
 * \retval _SHR_E_FAIL - if the test was found and performed but failed
 * \remark
 */
static shr_error_e
ctest_dnx_case_verify(
    int unit,
    int core,
    char *signal_set_n)
{
    xml_node curTop = NULL, curSubTop = NULL, curLine;
    char filename[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if ((curTop = dbx_pre_compiled_common_top_get(unit, "DNX-Devices.xml", "top", 0)) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    if ((curSubTop = dbx_xml_child_get_first(curTop, "case-objects")) == NULL)
    {
        /*
         * No parsing objects - just leave
         */
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

    RHDATA_ITERATOR(curLine, curSubTop, "include")
    {
        char type[RHNAME_MAX_SIZE];

        RHDATA_GET_STR_DEF_NULL(curLine, "type", type);
        if (!sal_strncasecmp(type, "tests", sizeof(type)))
        {
            xml_node curFile, curSection, cur;
            int device_specific = 0;

            RHDATA_GET_INT_DEF(curLine, "device_specific", device_specific, 0);
            RHDATA_GET_STR_CONT(curLine, "file", filename);
            if (device_specific == TRUE)
            {
                curFile = dbx_pre_compiled_devices_top_get(unit, filename, "top", CONF_OPEN_PER_DEVICE);
            }
            else
            {
                curFile = dbx_pre_compiled_common_top_get(unit, filename, "top", 0);
            }

            if (curFile == NULL)
                continue;

            /*
             * Go through signals and found the requested one
             */
            if (!ISEMPTY(signal_set_n) && ((curSection = dbx_xml_child_get_first(curFile, "signals")) != NULL))
            {
                RHDATA_ITERATOR(cur, curSection, "set")
                {
                    char name[RHNAME_MAX_SIZE];
                    RHDATA_GET_STR_DEF_NULL(cur, "name", name);
                    if (!sal_strncasecmp(signal_set_n, name, sizeof(name)))
                    {
                        SHR_CLI_EXIT_IF_ERR(ctest_dnx_verify_perform(unit, core, cur), "");
                    }
                }
            }
            dbx_xml_top_close(curFile);
        }
    }

exit:
    dbx_xml_top_close(curTop);
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_packet_test_options[] = {
    {"packet", SAL_FIELD_TYPE_STR, "Packet name from XML", "default"}
    ,
    {"set", SAL_FIELD_TYPE_STR, "Signal set name from XML", "default"}
    ,
    {"show", SAL_FIELD_TYPE_BOOL, "Show packet content", "no"}
    ,
    {"port", SAL_FIELD_TYPE_UINT32, "Use specific source TM_PORT", "0"}
    ,
    {NULL}
};

static sh_sand_man_t dnx_packet_test_man = {
    "Test any case from XML file"
};

static shr_error_e
dnx_packet_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *packet_n, *set_n;
    int show_flag;
    rhhandle_t packet_h;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("packet", packet_n);
    SH_SAND_GET_STR("set", set_n);
    SH_SAND_GET_BOOL("show", show_flag);

    SHR_CLI_EXIT_IF_ERR(diag_sand_packet_send_by_case(unit, packet_n, &packet_h, SAND_PACKET_RESUME), "");
    /*
     * Verify the case, but do not exit even on error before showing the packet if needed
     */
    SHR_SET_CURRENT_ERR(ctest_dnx_case_verify(unit, PACKET_CORE(packet_h), set_n));

    if (show_flag == TRUE)
    {   /* We are not interested in return status */
        diag_sand_packet_last_show(unit, PACKET_CORE(packet_h));
    }

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_enum_t sh_enum_table_rate_types[] = {
    {"kbps", BCM_SAT_GTF_RATE_IN_BYTES, "Stream rate should be set in kbits per second"},
    {"pps", BCM_SAT_GTF_RATE_IN_PACKETS, "Stream rate should be set in packets per second"},
    {NULL}
};

static sh_sand_option_t dnx_stream_test_options[] = {
    {"size", SAL_FIELD_TYPE_UINT32, "Packet Size(bytes)", "128"},
    {"duration", SAL_FIELD_TYPE_UINT32, "Stream Duration(seconds)", "1"},
    {"rate", SAL_FIELD_TYPE_UINT32, "Stream Rate(kbits per second)", "100000"},
    {"burst", SAL_FIELD_TYPE_UINT32, "Stream Rate(Kbits)", "1000"},
    {"granularity", SAL_FIELD_TYPE_INT32, "Clocks per cycle - set globally", "-1"},
    {"delay", SAL_FIELD_TYPE_UINT32, "Time to wait for taking stats after send", "0"},
    {"deviation", SAL_FIELD_TYPE_UINT32, "Error allowed between calculated and counted bytes/packets(%)", "3"},
    {"show", SAL_FIELD_TYPE_BOOL, "Print Result Table", "Yes"},
    {"enq_count", SAL_FIELD_TYPE_BOOL, "Verify ENQ Count", "No"},
    {"in_port", SAL_FIELD_TYPE_UINT32, "SAT Source Logical Port", "218", NULL, "218-219"},
    {"out_port", SAL_FIELD_TYPE_UINT32, "Destination Port were to send flow", "218", NULL},
    {"credit", SAL_FIELD_TYPE_INT32, "BackPressure Credit Count", "5", NULL, "0-127"},
    {"type", SAL_FIELD_TYPE_ENUM, "Rate type - kbits or packets per second", "kbps", sh_enum_table_rate_types},
    {NULL}
};

static sh_sand_man_t dnx_stream_test_man = {
    "Test multiple SAT streams"
};

static sh_sand_invoke_t dnx_stream_tests[] = {
    {"default", "", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_10M", "rate=10000 duration=10", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_1G", "rate=1000000", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_10G", "rate=10000000", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_10G_512B", "rate=10000000  size=512", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_10G_2KB", "rate=10000000  size=2048", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_40G", "rate=40000000", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_40G_512B", "rate=40000000  size=512", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_40G_2KB", "rate=40000000  size=2048", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_100G", "rate=100000000", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_100G_512B", "rate=100000000 size=512", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_100G_1KB", "rate=100000000 size=1024", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_100G_2KB", "rate=100000000 size=2048", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_200G", "rate=200000000 size=128", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_200G_512B", "rate=200000000 size=512", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_200G_2KB", "rate=200000000 size=2048", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_300G_128B", "rate=300000000 size=128", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_300G_512B", "rate=300000000 size=512", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_300G_2KB", "rate=300000000 size=2048", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_384G_384B", "rate=354000000 size=384 out_port=1", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_384G_512B", "rate=354000000 size=512", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_384G_1KB", "rate=384000000 size=1024", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_25M_512b_PPS", "rate=25000000 burst=100000 size=512 type=pps", SH_CMD_SKIP_EXEC * CTEST_ADAPTER_UNSUPPORTED},
    {NULL}
};
static sh_sand_invoke_t q2a_stream_tests[] = {
    {"default", "", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_10M", "rate=10000 duration=10 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_1G", "rate=1000000 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_10G", "rate=10000000 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_10G_512B", "rate=10000000  size=512 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_10G_2KB", "rate=10000000  size=2048 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_40G", "rate=40000000 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_40G_512B", "rate=40000000  size=512 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_40G_2KB", "rate=40000000  size=2048 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_100G", "rate=100000000 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_100G_512B", "rate=100000000 size=512 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_100G_1KB", "rate=100000000 size=1024 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_100G_2KB", "rate=100000000 size=2048 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_200G", "rate=200000000 size=128 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_200G_384B", "rate=200000000 size=384 out_port=1", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_200G_512B", "rate=200000000 size=512 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_200G_2KB", "rate=200000000 size=2048 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_270G_1KB", "rate=270000000 size=1024 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_280G_2KB", "rate=280000000 size=2048 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_300G_128B", "rate=300000000 size=128 ", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED},
    {"Rate_25M_512b_PPS", "rate=25000000 burst=100000 size=512 type=pps out_port=1",
     SH_CMD_SKIP_EXEC * CTEST_ADAPTER_UNSUPPORTED},
    {NULL}
};

static shr_error_e
sh_cmd_is_jr2_device(
    int unit,
    rhlist_t * test_list)
{
    int rv = _SHR_E_NONE;
    rv = sh_cmd_is_device(unit, test_list);
    if (!
        (soc_is(unit, JERICHO2_A0_DEVICE) || soc_is(unit, JERICHO2_B1_ONLY) || soc_is(unit, J2C_DEVICE)
         || soc_is(unit, J2P_DEVICE)))
    {
#if !defined(ADAPTER_SERVER_MODE)
        return _SHR_E_UNIT;
#endif
    }
    return rv;
}

static shr_error_e
sh_cmd_is_q2a_device(
    int unit,
    rhlist_t * test_list)
{
    int rv = _SHR_E_NONE;
    rv = sh_cmd_is_device(unit, test_list);
    if (soc_is(unit, Q2A_DEVICE) == FALSE)
    {
#if !defined(ADAPTER_SERVER_MODE)
        return _SHR_E_UNIT;
#endif
    }
    return rv;
}

static shr_error_e
dnx_stream_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    rhhandle_t packet_h = NULL;
    uint32 packet_size, stream_duration, stream_rate, stream_burst, deviation;
    int stream_granularity;
    int credit;
    int type;
    uint32 delay;
    bcm_core_t core;
    int i_pipe;
    uint32 in_port, out_port, itmh_out_port, parser_context = 1, parser_offset = 0;
    dnx_ingress_congestion_counter_info_t counter_info;

    int show_flag, enq_count;
    COMPILER_UINT64 packet_count, bytes_count;
    COMPILER_UINT64 calc_packet_count, calc_bytes_count;
    COMPILER_UINT64 deviation_packets = COMPILER_64_INIT(0, 0);
    COMPILER_UINT64 deviation_bytes = COMPILER_64_INIT(0, 0);
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;

    uint32 flags;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("size", packet_size);
    SH_SAND_GET_UINT32("duration", stream_duration);
    SH_SAND_GET_UINT32("rate", stream_rate);
    SH_SAND_GET_UINT32("burst", stream_burst);
    SH_SAND_GET_INT32("granularity", stream_granularity);
    SH_SAND_GET_UINT32("delay", delay);
    SH_SAND_GET_UINT32("Deviation", deviation);
    SH_SAND_GET_UINT32("in_port", in_port);
    SH_SAND_GET_UINT32("out_port", out_port);
    SH_SAND_GET_INT32("credit", credit);
    SH_SAND_GET_ENUM("type", type);
    SH_SAND_GET_BOOL("enq_count", enq_count);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, in_port, &core));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, in_port, &flags, &interface_info, &mapping_info));

    for (i_pipe = 0; i_pipe < 4; i_pipe++)
    {
        SHR_IF_ERR_EXIT(dnx_sat_gtf_credit_config(unit, i_pipe, credit));
    }

    /** send packets */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_create(unit, core, packet_h, packet_size, stream_duration,
                                                   type, stream_rate, stream_burst, stream_granularity));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ITMH_Base"));
    itmh_out_port = 0xC000 | out_port;
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ITMH_Base.Dst", &itmh_out_port, 21));
    if (diag_sand_packet_proto_add(unit, packet_h, "PPH_Base") == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PPH_Base.PPH_Parsing_Start_Type",
                                                          &parser_context, 5));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PPH_Base.PPH_Parsing_Start_Offset",
                                                          &parser_offset, 7));
    }
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "UDH"));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, in_port, out_port, 1));

    /** Clean global counters */
    if (enq_count == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_programmable_counter_info_get(unit, core, &counter_info));
    }

    SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, &packet_h, 1));

    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, in_port, out_port, 0));

    if (delay != 0)
    {
        sal_sleep(delay);
    }
    /*
     * Calculate supposed bytes count - rate * duration - count in kbits  * 1000 / 8 = count in bytes
     */
    if (type == BCM_SAT_GTF_RATE_IN_BYTES)
    {
        COMPILER_UINT64 divisor;

        COMPILER_64_SET(divisor, 0, stream_rate);
        COMPILER_64_COPY(calc_bytes_count, divisor);
        COMPILER_64_UMUL_32(calc_bytes_count, 1000);
        COMPILER_64_UMUL_32(calc_bytes_count, stream_duration);
        COMPILER_64_SET(divisor, 0, 8);
        COMPILER_64_UDIV_64(calc_bytes_count, divisor);
        /*
         * Calculate supposed packet count - byte count / packet_size
         */
        COMPILER_64_COPY(calc_packet_count, calc_bytes_count);
        COMPILER_64_UDIV_32(calc_packet_count, packet_size);
    }
    else
    {
        /*
         * For PPS - just take rate and multiply it by time
         */
        COMPILER_64_SET(calc_packet_count, 0, stream_rate);
        COMPILER_64_UMUL_32(calc_packet_count, stream_duration);
        /*
         * Take packet sum
         */
        COMPILER_64_COPY(calc_bytes_count, calc_packet_count);
        COMPILER_64_UMUL_32(calc_bytes_count, packet_size);
    }
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_get_stats(unit, packet_h, &bytes_count, &packet_count));

    if (enq_count == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_programmable_counter_info_get(unit, core, &counter_info));
    }

    SH_SAND_GET_BOOL("show", show_flag);
    if (show_flag == TRUE)
    {
        int bit_rate;
        bit_rate = type == BCM_SAT_GTF_RATE_IN_BYTES ?
            stream_rate / 1000000 : ((stream_rate / 1000) * packet_size * 8) / 1000000;
        PRT_TITLE_SET("SAT Test Results");
        PRT_INFO_ADD("InPort %d OutPort %d", in_port, out_port);
        if (stream_rate >= 1000000)
        {
            PRT_INFO_ADD("Rate        %d(Gbps)", bit_rate);
        }
        else
        {
            PRT_INFO_ADD("Rate        %d(Kbps)", stream_rate);
        }
        PRT_INFO_ADD("Granularity %d(Cycles)", PACKET_GRANULARITY(packet_h));
        if (type == BCM_SAT_GTF_RATE_IN_BYTES)
        {
            COMPILER_UINT64 pps;
            COMPILER_UINT64 divisor;

            COMPILER_64_SET(divisor, 0, stream_rate);
            COMPILER_64_COPY(pps, divisor);
            COMPILER_64_UMUL_32(pps, 1000);
            COMPILER_64_UDIV_32(pps, packet_size);
            COMPILER_64_SET(divisor, 0, 8);
            COMPILER_64_UDIV_64(pps, divisor);
            PRT_INFO_ADD("PPS         %u", COMPILER_64_LO(pps));
        }
        else
        {
            PRT_INFO_ADD("PPS         %u", stream_rate);
        }
        PRT_INFO_ADD("Burst       %d(kb)", PACKET_BURST(packet_h));
        PRT_INFO_ADD("Packet Size %d(B)", packet_size);
        PRT_INFO_ADD("Duration    %d(sec)", stream_duration);
        PRT_COLUMN_ADD("");
        PRT_COLUMN_ADD("Bytes");
        PRT_COLUMN_ALIGN;
        PRT_COLUMN_ADD("Packets");
        PRT_COLUMN_ALIGN;
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Calculated");
        PRT_CELL_SET("0x%x,0x%x", COMPILER_64_HI(calc_bytes_count), COMPILER_64_LO(calc_bytes_count));
        PRT_CELL_SET("0x%x,0x%x", COMPILER_64_HI(calc_packet_count), COMPILER_64_LO(calc_packet_count));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Measured");
        PRT_CELL_SET("0x%x,0x%x", COMPILER_64_HI(bytes_count), COMPILER_64_LO(bytes_count));
        PRT_CELL_SET("0x%x,0x%x", COMPILER_64_HI(packet_count), COMPILER_64_LO(packet_count));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Deviation(%%)");
        DNX_64_ABS_DEVIATION(calc_bytes_count, bytes_count, deviation_bytes);
        DNX_64_ABS_DEVIATION(calc_packet_count, packet_count, deviation_packets);
        PRT_CELL_SET("%d", COMPILER_64_LO(deviation_bytes));
        PRT_CELL_SET("%d", COMPILER_64_LO(deviation_packets));
        if (enq_count == TRUE)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("SRAM_ENQ");
            PRT_CELL_SKIP(1);
            PRT_CELL_SET("0x%x", counter_info.sram_enq_pkt_cnt);
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("SRAM_DEQ_TO_DRAM");
            PRT_CELL_SKIP(1);
            PRT_CELL_SET("0x%x", counter_info.sram_to_dram_pkt_cnt);
        }
        PRT_COMMITX;
    }
    /*
     * Success criteria - packets, byte counter can be overflowed (34b vs packet 32b)
     */
    if (COMPILER_64_LO(deviation_packets) > deviation)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

    if ((enq_count == TRUE) &&
        ((COMPILER_64_LO(packet_count) != counter_info.sram_enq_pkt_cnt) || (counter_info.sram_to_dram_pkt_cnt != 0)))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }
exit:
    diag_sand_packet_free(unit, packet_h);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_collector_test_options[] = {
    {"size", SAL_FIELD_TYPE_UINT32, "Packet Size(bytes)", "100"},
    {"duration", SAL_FIELD_TYPE_UINT32, "Stream Duration(seconds)", "10"},
    {"rate", SAL_FIELD_TYPE_UINT32, "Stream Rate(kbits per second)", "100000"},
    {"burst", SAL_FIELD_TYPE_UINT32, "Stream Rate(Kbits)", "100"},
    {"granularity", SAL_FIELD_TYPE_UINT32, "Clocks per cycle - set globally", "-1"},
    {"delay", SAL_FIELD_TYPE_UINT32, "Time to wait for taking stats after send", "10"},
    {"deviation", SAL_FIELD_TYPE_UINT32, "Error allowed between calculated and counted bytes/packets(%)", "3"},
    {"show", SAL_FIELD_TYPE_BOOL, "Print Result Table", "Yes"},
    {"in_port", SAL_FIELD_TYPE_UINT32, "SAT Source Logical Port", "1", NULL},
    {"out_port", SAL_FIELD_TYPE_UINT32, "Destination Port were to send flow", "218", NULL},
    {"type", SAL_FIELD_TYPE_ENUM, "Rate type - kbits or packets per second", "kbps", sh_enum_table_rate_types},
    {NULL}
};

static sh_sand_man_t dnx_collector_test_man = {
    "Test SAT Collector action on 1 stream"
};

static sh_sand_invoke_t dnx_collector_tests[] = {
    {"default", "", CTEST_POSTCOMMIT},
    {"Rate_10M", "rate=10000 duration=10", CTEST_POSTCOMMIT},
    {"Rate_1G", "rate=1000000", CTEST_POSTCOMMIT},
    {"Rate_10G_67b", "rate=10000000 burst=1250 size=67", CTEST_POSTCOMMIT},
    {"Rate_40G_67b", "rate=40000000 burst=5000 size=67", CTEST_POSTCOMMIT},
    {"Rate_100G_67b", "rate=100000000 burst=12500 size=67", CTEST_POSTCOMMIT},
    {"Rate_200G_67b", "rate=200000000 burst=25000 size=67", SH_CMD_SKIP_EXEC},
    {"Rate_10G_512b", "rate=10000000 burst=1250 size=512", CTEST_POSTCOMMIT},
    {"Rate_40G_512b", "rate=40000000 burst=5000 size=512", CTEST_POSTCOMMIT},
    {"Rate_100G_512b", "rate=100000000 burst=12500 size=512", CTEST_POSTCOMMIT},
    {"Rate_200G_512b", "rate=200000000 burst=25000 size=512", CTEST_POSTCOMMIT},
    {"Rate_10G_2kb", "rate=10000000 burst=1250 size=1518", CTEST_POSTCOMMIT},
    {"Rate_40G_2kb", "rate=40000000 burst=5000 size=1518", CTEST_POSTCOMMIT},
    {"Rate_100G_2kb", "rate=100000000 burst=12500 size=1518", CTEST_POSTCOMMIT},
    {"Rate_200G_2kb", "rate=200000000 burst=25000 size=1518", CTEST_POSTCOMMIT},
    {"Rate_400G_2kb", "rate=400000000 burst=40000 size=1518", CTEST_POSTCOMMIT},
    {"Rate_25M_512b_PPS", "rate=25000000 burst=3125 size=512 type=pps", SH_CMD_SKIP_EXEC},
    {NULL}
};

/* SAT collector test
 * The packets are collected based on the PCP and VID values of the send packets.
 * The test verified that the transmitted packets count is equal to the received packets counter.*/
static shr_error_e
dnx_collector_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    rhhandle_t packet_h = NULL;
    uint32 packet_size, stream_duration, stream_rate, stream_burst, stream_granularity, deviation;
    uint32 delay;
    bcm_core_t core;
    uint32 in_port, out_port;
    int type;

    int show_flag;
    uint64 packet_count, bytes_count;
    uint64 calc_packet_count, calc_bytes_count;
    uint64 deviation_packets = COMPILER_64_INIT(0, 0);
    uint64 deviation_bytes = COMPILER_64_INIT(0, 0);
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    rhhandle_t rx_packet_h = NULL;
    uint32 pcp = 3;
    uint32 vid = 100;
    uint64 rx_count, err_count, oor_count;
    rhhandle_t ctest_soc_set_h = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    sal_printf("Loading SOC properties!\n");
    {
        /* *INDENT-OFF* */
        ctest_soc_property_t ctest_soc_property[] = {
           {"port_init_speed_sat*", NULL},
           {"port_init_speed_sat", "400000"},
           {NULL}
        };
        /* *INDENT-ON* */
        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    }

    SH_SAND_GET_UINT32("size", packet_size);
    SH_SAND_GET_UINT32("duration", stream_duration);
    SH_SAND_GET_UINT32("rate", stream_rate);
    SH_SAND_GET_UINT32("burst", stream_burst);
    SH_SAND_GET_UINT32("granularity", stream_granularity);
    SH_SAND_GET_UINT32("delay", delay);
    SH_SAND_GET_UINT32("Deviation", deviation);
    SH_SAND_GET_UINT32("in_port", in_port);
    SH_SAND_GET_UINT32("out_port", out_port);
    SH_SAND_GET_ENUM("type", type);

    if (dnx_data_sat.general.feature_get(unit, dnx_data_sat_general_is_sat_supported) == 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_NONE);
    }

    /** Get GTF max bit rate in case the packet size is 1518B.*/
    if ((packet_size == 1518) && (stream_rate == 400000000))
    {
        stream_rate = dnx_data_sat.generator.max_bit_rate_in_1518B_get(unit);
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, in_port, &core));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, in_port, &flags, &interface_info, &mapping_info));

    /** Disable learning for the port. */
    SHR_IF_ERR_EXIT(bcm_port_learn_set(unit, in_port, BCM_PORT_LEARN_FWD));
    /*
     * Phase 1 - Create collector configuration and start collector
     */
    /*
     * Generate packet with fields of interest in receiving by collector. The SAT will collect packets based on the
     * configured ETH fields(PCP/VLAN) 
     */
    sal_printf("Create collector configuration!\n");
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &rx_packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, rx_packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, rx_packet_h, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, rx_packet_h, "ETH1.VLAN.PCP", &pcp, 3));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, rx_packet_h, "ETH1.VLAN.VID", &vid, 12));
    /*
     * In case the PCP/DEI fields of the ETH header have values different than 0, need to update also the
     * pkt_sat_identifier_info
     */
    pkt_sat_identifier_info.tc = pcp;
    SHR_IF_ERR_EXIT(diag_sand_packet_sat_rx_create(unit, rx_packet_h, out_port));

    /*
     * We have only one collector - so just start the collection
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_sat_rx_start(unit, NULL));

    /*
     * Nullify collector statistics
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_sat_rx_get_stats(unit, PACKET_CTF_ID(rx_packet_h),
                                                      &rx_count, &err_count, &oor_count));

    /*
     * Phase 2 - Send traffic
     */
    /** send packets */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_create(unit, core, packet_h, packet_size, stream_duration,
                                                   type, stream_rate, stream_burst, stream_granularity));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.PCP", &pcp, 3));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vid, 12));
    sal_printf("Sending packets for %d seconds\n", stream_duration);
    sal_sleep(delay);
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, &packet_h, 1));
    /*
     * Calculate supposed bytes count - rate * duration - count in kbits  * 1000 / 8 = count in bytes
     */
    if (type == BCM_SAT_GTF_RATE_IN_BYTES)
    {
        uint64 divisor;

        COMPILER_64_SET(divisor, 0, stream_rate);
        COMPILER_64_COPY(calc_bytes_count, divisor);
        COMPILER_64_UMUL_32(calc_bytes_count, 1000);
        COMPILER_64_UMUL_32(calc_bytes_count, stream_duration);
        COMPILER_64_SET(divisor, 0, 8);
        COMPILER_64_UDIV_64(calc_bytes_count, divisor);
        /*
         * Calculate supposed packet count - byte count / packet_size
         */
        COMPILER_64_COPY(calc_packet_count, calc_bytes_count);
        COMPILER_64_UDIV_32(calc_packet_count, packet_size);
    }
    else
    {
        /*
         * For PPS - just take rate and multiply it by time
         */
        uint64 tmp_64;

        COMPILER_64_SET(tmp_64, 0, stream_rate);
        COMPILER_64_COPY(calc_packet_count, tmp_64);
        COMPILER_64_UMUL_32(calc_packet_count, stream_duration);
        /*
         * Take packet sum
         */
        COMPILER_64_COPY(calc_bytes_count, calc_packet_count);
        COMPILER_64_UMUL_32(calc_bytes_count, packet_size);
    }
    sal_printf("Get GTF statistics.\n");
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_get_stats(unit, packet_h, &bytes_count, &packet_count));

    SH_SAND_GET_BOOL("show", show_flag);
    if (show_flag == TRUE)
    {
        int bit_rate, pps;
        bit_rate = type == BCM_SAT_GTF_RATE_IN_BYTES ?
            stream_rate / 1000000 : ((stream_rate / 1000) * packet_size * 8) / 1000000;
        pps = type == BCM_SAT_GTF_RATE_IN_BYTES ? (stream_rate * 1000) / (packet_size * 8) : stream_rate;
        PRT_TITLE_SET("SAT Test Results");
        PRT_INFO_ADD("InPort %d OutPort %d", in_port, out_port);
        if (stream_rate >= 1000000)
        {
            PRT_INFO_ADD("Rate        %d(Gbps)", bit_rate);
        }
        else
        {
            PRT_INFO_ADD("Rate        %d(Kbps)", stream_rate);
        }
        PRT_INFO_ADD("Granularity %d(Cycles)", PACKET_GRANULARITY(packet_h));
        PRT_INFO_ADD("PPS         %d", pps);
        PRT_INFO_ADD("Burst       %d(kb)", PACKET_BURST(packet_h));
        PRT_INFO_ADD("Packet Size %d(B)", packet_size);
        PRT_INFO_ADD("Duration    %d(sec)", stream_duration);
        PRT_COLUMN_ADD("");
        PRT_COLUMN_ADD("Bytes");
        PRT_COLUMN_ADD("Packets");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Calculated");
        PRT_CELL_SET("0x%x,0x%x", COMPILER_64_HI(calc_bytes_count), COMPILER_64_LO(calc_bytes_count));
        PRT_CELL_SET("0x%x,0x%x", COMPILER_64_HI(calc_packet_count), COMPILER_64_LO(calc_packet_count));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Measured");
        PRT_CELL_SET("0x%x,0x%x", COMPILER_64_HI(bytes_count), COMPILER_64_LO(bytes_count));
        PRT_CELL_SET("0x%x,0x%x", COMPILER_64_HI(packet_count), COMPILER_64_LO(packet_count));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Deviation(%%)");
        DNX_64_ABS_DEVIATION(calc_bytes_count, bytes_count, deviation_bytes);
        DNX_64_ABS_DEVIATION(calc_packet_count, packet_count, deviation_packets);
        PRT_CELL_SET("%d", COMPILER_64_LO(deviation_bytes));
        PRT_CELL_SET("%d", COMPILER_64_LO(deviation_packets));
        PRT_COMMITX;
    }
    /*
     * Success criteria - packets, byte counter can be overflowed (34b vs packet 32b)
     */
    if (COMPILER_64_LO(deviation_packets) > deviation)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Deviation is more than the expected one!\n");
    }
    /*
     * Wait time before getting statistics
     */
    if (delay != 0)
    {
        sal_sleep(delay);
    }
    /*
     * Check collector statistics
     */
    sal_printf("Check CTF statistics!\n");
    SHR_IF_ERR_EXIT(diag_sand_packet_sat_rx_get_stats(unit, PACKET_CTF_ID(rx_packet_h),
                                                      &rx_count, &err_count, &oor_count));

    LOG_CLI(("CTF:%d  RX  - 0x%x,0x%x\n", PACKET_CTF_ID(rx_packet_h), COMPILER_64_HI(rx_count),
             COMPILER_64_LO(rx_count)));
    LOG_CLI(("       Err - 0x%x,0x%x\n", COMPILER_64_HI(err_count), COMPILER_64_LO(err_count)));
    LOG_CLI(("       OOR - 0x%x,0x%x\n", COMPILER_64_HI(oor_count), COMPILER_64_LO(oor_count)));

    /*
     * Verify the received packets are equal to the transmitted packets
     * Skip the case when the received packets exceed the max received packets limitation
     */
    sal_printf("Compare transmitted packets count and received packets count\n");
    if (COMPILER_64_LO(packet_count) != COMPILER_64_LO(rx_count)
        &&
        (((packet_size == 1588)
          && (COMPILER_64_LO(rx_count) < dnx_data_sat.generator.max_ctf_recv_packet_rate_in_1518B_get(unit)))))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "The number of received packets is not expected! Expected no drops\n");
    }

    sal_printf("Clean up!\n");
    SHR_IF_ERR_EXIT(diag_sand_packet_sat_rx_destroy(unit));
exit:
    diag_sand_packet_free(unit, packet_h);
    PRT_FREE;
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_learning_test_options[] = {
    {"size", SAL_FIELD_TYPE_UINT32, "Packet Size(bytes)", "128"},
    {"duration", SAL_FIELD_TYPE_UINT32, "Stream Duration(seconds)", "10"},
    {"rate", SAL_FIELD_TYPE_UINT32, "Stream Rate(kbits per second)", "100000"},
    {"burst", SAL_FIELD_TYPE_UINT32, "Stream Rate(Kbits)", "100"},
    {"granularity", SAL_FIELD_TYPE_UINT32, "Clocks per cycle - set globally", "-1"},
    {"delay", SAL_FIELD_TYPE_UINT32, "Time to wait for taking stats after send", "1"},
    {"show", SAL_FIELD_TYPE_BOOL, "Print Result Table", "Yes"},
    {"in_port", SAL_FIELD_TYPE_UINT32, "SAT Source Logical Port", "218", NULL, "218-219"},
    {"out_port", SAL_FIELD_TYPE_UINT32, "Destination Port were to send flow", "219", NULL},
    {"type", SAL_FIELD_TYPE_ENUM, "Rate type - kbits or packets per second", "kbps", sh_enum_table_rate_types},
    {NULL}
};

static sh_sand_man_t dnx_learning_test_man = {
    "Test L2 Learning with SAT"
};

static sh_sand_invoke_t dnx_learning_tests[] = {
    {"default", "", CTEST_POSTCOMMIT},
    {"Rate_10M", "rate=10000 duration=10", CTEST_POSTCOMMIT},
    {"Rate_1G", "rate=1000000", CTEST_POSTCOMMIT},
    {"Rate_10G_67b", "rate=10000000 burst=1250 size=67", CTEST_POSTCOMMIT},
    {"Rate_40G_67b", "rate=40000000 burst=5000 size=67", CTEST_POSTCOMMIT},
    {"Rate_100G_67b", "rate=100000000 burst=12500 size=67", CTEST_POSTCOMMIT},
    {"Rate_200G_67b", "rate=200000000 burst=125000 size=67", SH_CMD_SKIP_EXEC},
    {"Rate_10G_512b", "rate=10000000 burst=1250 size=512", CTEST_POSTCOMMIT},
    {"Rate_40G_512b", "rate=40000000 burst=5000 size=512", CTEST_POSTCOMMIT},
    {"Rate_100G_512b", "rate=100000000 burst=12500 size=512", CTEST_POSTCOMMIT},
    {"Rate_200G_512b", "rate=200000000 burst=25000 size=512", CTEST_POSTCOMMIT},
    {"Rate_10G_2kb", "rate=10000000 burst=1250 size=2000", CTEST_POSTCOMMIT},
    {"Rate_40G_2kb", "rate=40000000 burst=5000 size=2000", CTEST_POSTCOMMIT},
    {"Rate_100G_2kb", "rate=100000000 burst=12500 size=2000", CTEST_POSTCOMMIT},
    {"Rate_200G_2kb", "rate=200000000 burst=25000 size=2000", CTEST_POSTCOMMIT},
    {"Rate_300G_2kb", "rate=300000000 burst=37500 size=2000", CTEST_POSTCOMMIT},
    {"Rate_400G_2kb", "rate=400000000 burst=50000 size=1000", CTEST_POSTCOMMIT},
    {NULL}
};

static shr_error_e
dnx_learning_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    rhhandle_t packet_h = NULL;
    uint32 packet_size, stream_duration, stream_rate, stream_burst, stream_granularity;
    int type;
    int show_flag;
    int global_cycle_time;
    uint32 in_port, out_port;
    uint32 flags;
    uint32 vid = 1;
    uint32 learned_entries = 0;
    uint32 nof_expected_entries = 65536;
    bcm_core_t core;

    uint64 packet_count, bytes_count;
    uint64 calc_packet_count;
    uint64 calc_bytes_count;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_l2_addr_t l2addr;

    sal_mac_addr_t src_mac_addr = { 0, 0, 0, 00, 00, 01 };
    sal_mac_addr_t dst_mac_addr = { 99, 99, 99, 99, 99, 99 };
    char mac_str[RHNAME_MAX_SIZE];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("size", packet_size);
    SH_SAND_GET_UINT32("duration", stream_duration);
    SH_SAND_GET_UINT32("rate", stream_rate);
    SH_SAND_GET_UINT32("burst", stream_burst);
    SH_SAND_GET_UINT32("granularity", stream_granularity);
    SH_SAND_GET_UINT32("in_port", in_port);
    SH_SAND_GET_UINT32("out_port", out_port);
    SH_SAND_GET_ENUM("type", type);

    if (dnx_data_sat.general.feature_get(unit, dnx_data_sat_general_is_sat_supported) == 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_NONE);
    }

    /*
     * Disable aging
     */
    SHR_IF_ERR_EXIT(bcm_l2_age_timer_meta_cycle_get(unit, &global_cycle_time));
    SHR_IF_ERR_EXIT(bcm_l2_age_timer_meta_cycle_set(unit, 0));

    in_port = 201;
    out_port = 1;
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, in_port, &core));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, in_port, &flags, &interface_info, &mapping_info));

    /**Add the destination entry */
    bcm_l2_addr_t_init(&l2addr, dst_mac_addr, 1);
    l2addr.port = out_port;
    SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));

    /*
     * Send traffic
     */
    /** send packets */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_create(unit, core, packet_h, packet_size, stream_duration,
                                                   type, stream_rate, stream_burst, stream_granularity));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vid, 12));
    sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                 src_mac_addr[0], src_mac_addr[1], src_mac_addr[2], src_mac_addr[3], src_mac_addr[4], src_mac_addr[5]);

    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", mac_str));
    sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                 dst_mac_addr[0], dst_mac_addr[1], dst_mac_addr[2], dst_mac_addr[3], dst_mac_addr[4], dst_mac_addr[5]);
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", mac_str));
    diag_sand_packet_flags_set(unit, packet_h, SAND_PACKET_STREAM_SA_INCREMENTAL_FIELD);
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, &packet_h, 1));
    sal_printf("Waiting 5 sec before getting the MACT entry count\n");
    sal_sleep(5);
    /*
     * Obtain MACT count
     */
    SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_count(unit, &learned_entries));
    sal_printf("The number of learned MACT entries:%u \n", learned_entries - 1);

    /*
     * Calculate supposed bytes count - rate * duration - count in kbits  * 1000 / 8 = count in bytes
     */
    if (type == BCM_SAT_GTF_RATE_IN_BYTES)
    {
        uint64 divisor;

        COMPILER_64_SET(divisor, 0, stream_rate);
        COMPILER_64_COPY(calc_bytes_count, divisor);
        COMPILER_64_UMUL_32(calc_bytes_count, 1000);
        COMPILER_64_UMUL_32(calc_bytes_count, stream_duration);
        COMPILER_64_SET(divisor, 0, 8);
        COMPILER_64_UDIV_64(calc_bytes_count, divisor);
        /*
         * Calculate supposed packet count - byte count / packet_size
         */
        COMPILER_64_COPY(calc_packet_count, calc_bytes_count);
        COMPILER_64_UDIV_32(calc_packet_count, packet_size);
    }
    else
    {
        /*
         * For PPS - just take rate and multiply it by time
         */
        uint64 tmp_64;

        COMPILER_64_SET(tmp_64, 0, stream_rate);
        COMPILER_64_COPY(calc_packet_count, tmp_64);
        COMPILER_64_UMUL_32(calc_packet_count, stream_duration);
        /*
         * Take packet sum
         */
        COMPILER_64_COPY(calc_bytes_count, calc_packet_count);
        COMPILER_64_UMUL_32(calc_bytes_count, packet_size);
    }
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_get_stats(unit, packet_h, &bytes_count, &packet_count));
    SH_SAND_GET_BOOL("show", show_flag);
    if (show_flag == TRUE)
    {
        int bit_rate, pps;
        bit_rate = type == BCM_SAT_GTF_RATE_IN_BYTES ?
            stream_rate / 1000000 : ((stream_rate / 1000) * packet_size * 8) / 1000000;
        pps = type == BCM_SAT_GTF_RATE_IN_BYTES ? (stream_rate * 1000) / (packet_size * 8) : stream_rate;
        PRT_TITLE_SET("SAT Test Results");
        PRT_INFO_ADD("InPort %d OutPort %d", in_port, out_port);
        if (stream_rate >= 1000000)
        {
            PRT_INFO_ADD("Rate        %d(Gbps)", bit_rate);
        }
        else
        {
            PRT_INFO_ADD("Rate        %d(Kbps)", stream_rate);
        }
        PRT_INFO_ADD("Granularity %d(Cycles)", PACKET_GRANULARITY(packet_h));
        PRT_INFO_ADD("PPS         %d", pps);
        PRT_INFO_ADD("Burst       %d(kb)", PACKET_BURST(packet_h));
        PRT_INFO_ADD("Packet Size %d(B)", packet_size);
        PRT_INFO_ADD("Duration    %d(sec)", stream_duration);
        PRT_INFO_ADD("Number of learned MACs   %d", learned_entries - 1);
        PRT_COLUMN_ADD("");
        PRT_COLUMN_ADD("Bytes");
        PRT_COLUMN_ADD("Packets");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Calculated");
        PRT_CELL_SET("0x%x,0x%x", COMPILER_64_HI(calc_bytes_count), COMPILER_64_LO(calc_bytes_count));
        PRT_CELL_SET("0x%x,0x%x", COMPILER_64_HI(calc_packet_count), COMPILER_64_LO(calc_packet_count));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Measured");
        PRT_CELL_SET("0x%x,0x%x", COMPILER_64_HI(bytes_count), COMPILER_64_LO(bytes_count));
        PRT_CELL_SET("0x%x,0x%x", COMPILER_64_HI(packet_count), COMPILER_64_LO(packet_count));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_COMMITX;
    }
    if ((learned_entries - 1) != nof_expected_entries)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "The number of learnt packets is not correct! Must be 65536 \n");
    }

    /*
     * Clear the table to allow next test to run
     */
    SHR_IF_ERR_EXIT(bcm_l2_age_timer_meta_cycle_set(unit, global_cycle_time));
    SHR_IF_ERR_EXIT(sh_dnx_l2_clear_all(unit, TRUE, TRUE));
exit:
    diag_sand_packet_free(unit, packet_h);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_l2_limit_test_options[] = {
    {"size", SAL_FIELD_TYPE_UINT32, "Packet Size(bytes)", "128"},
    {"duration", SAL_FIELD_TYPE_UINT32, "Stream Duration(seconds)", "5"},
    {"rate", SAL_FIELD_TYPE_UINT32, "Stream Rate(kbits per second)", "10000000"},
    {"burst", SAL_FIELD_TYPE_UINT32, "Stream Rate(Kbits)", "1000"},
    {"granularity", SAL_FIELD_TYPE_UINT32, "Clocks per cycle - set globally", "-1"},
    {"delay", SAL_FIELD_TYPE_UINT32, "Time to wait for taking stats after send", "1"},
    {"in_port", SAL_FIELD_TYPE_UINT32, "SAT Source Logical Port", "1"},
    {"out_port", SAL_FIELD_TYPE_UINT32, "Destination Port were to send flow", "13"},
    {NULL}
};

static sh_sand_man_t dnx_l2_limit_test_man = {
    "Test L2 limit/aging with SAT."
};

static sh_sand_invoke_t dnx_l2_limit_tests[] = {
    {"default", "", CTEST_POSTCOMMIT},
    {NULL}
};

/*
 * Test sequence:
 * 1. Set the MACT limit to 10000 packets.
 * 2. Configured the SAT stream(SA is incremented).
 * 3. Set the meta-cycle time to 5 sec.
 * 4. Send traffic.
 * 5. Verify that the entries were learned according to limit.
 * 6. Stop traffic.
 * 7. Wait time(the entries to aged-out).
 * 8. Repeat 2 - 3
 */
static shr_error_e
dnx_l2_limit_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    rhhandle_t packet_h = NULL;
    uint32 packet_size, stream_duration, stream_rate, stream_burst, stream_granularity;
    int global_cycle_time;
    uint32 in_port, out_port;
    uint32 flags;
    uint32 vid = 1;
    uint32 mact_entries = 0;
    int pkt_limit = 10000;
    int def_limit = 0;

    bcm_core_t core;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_l2_addr_t l2addr;
    bcm_l2_learn_limit_t limit;

    sal_mac_addr_t src_mac_addr = { 0, 0, 0, 00, 00, 01 };
    sal_mac_addr_t dst_mac_addr = { 99, 99, 99, 99, 99, 99 };
    char mac_str[RHNAME_MAX_SIZE];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("size", packet_size);
    SH_SAND_GET_UINT32("duration", stream_duration);
    SH_SAND_GET_UINT32("rate", stream_rate);
    SH_SAND_GET_UINT32("burst", stream_burst);
    SH_SAND_GET_UINT32("granularity", stream_granularity);
    SH_SAND_GET_UINT32("in_port", in_port);
    SH_SAND_GET_UINT32("out_port", out_port);

    if (dnx_data_sat.general.feature_get(unit, dnx_data_sat_general_is_sat_supported) == 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_NONE);
    }
    /*
     * Save the default cycle time and set new one(5 sec)
     */
    SHR_IF_ERR_EXIT(bcm_l2_age_timer_meta_cycle_get(unit, &global_cycle_time));
    SHR_IF_ERR_EXIT(bcm_l2_age_timer_meta_cycle_set(unit, 5));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, in_port, &core));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, in_port, &flags, &interface_info, &mapping_info));

    /** Add destination entry */
    bcm_l2_addr_t_init(&l2addr, dst_mac_addr, 1);
    l2addr.port = out_port;
    SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));

    /*
     * Save the default limit and set new MACT limit
     */
    bcm_l2_learn_limit_t_init(&limit);
    limit.flags = BCM_L2_LEARN_LIMIT_SYSTEM;
    SHR_IF_ERR_EXIT(bcm_l2_learn_limit_get(unit, &limit));
    def_limit = limit.limit;
    limit.limit = pkt_limit;
    SHR_IF_ERR_EXIT(bcm_l2_learn_limit_set(0, &limit));
    /*
     * Configure the packets
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_create(unit, core, packet_h, packet_size, stream_duration,
                                                   BCM_SAT_GTF_RATE_IN_BYTES, stream_rate, stream_burst,
                                                   stream_granularity));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vid, 12));
    sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                 src_mac_addr[0], src_mac_addr[1], src_mac_addr[2], src_mac_addr[3], src_mac_addr[4], src_mac_addr[5]);
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", mac_str));
    sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                 dst_mac_addr[0], dst_mac_addr[1], dst_mac_addr[2], dst_mac_addr[3], dst_mac_addr[4], dst_mac_addr[5]);
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", mac_str));
    diag_sand_packet_flags_set(unit, packet_h, SAND_PACKET_STREAM_SA_INCREMENTAL_FIELD);

    /*
     * Send traffic
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_start(unit, packet_h));
    sal_sleep(3);

    SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_count(unit, &mact_entries));
    sal_printf("The number of entries in the MACT is:%u\n\n", mact_entries);

    if (mact_entries != pkt_limit)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "The number of entries in the MACT must be %u, but it is %u", pkt_limit,
                     mact_entries);
    }
    /*
     * Stop traffic
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_stop(unit, packet_h));

    /*
     * Wait time for entries to aged-out
     */
    sal_sleep(50);

    mact_entries = 0;
    SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_count(unit, &mact_entries));
    if (mact_entries != 0)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "The number of entries in the MACT must be 0");
    }
    /*
     * Send traffic
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_start(unit, packet_h));
    sal_sleep(3);

    SHR_IF_ERR_EXIT(sh_dnx_l2_get_mact_count(unit, &mact_entries));
    if (mact_entries != pkt_limit)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "The number of entries in the MACT must be %u, but it is %u!", pkt_limit,
                     mact_entries);
    }
    /*
     * Stop traffic
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_stop(unit, packet_h));

    /*
     * Clean up
     * Clear the table to allow next test to run
     */
    limit.limit = def_limit;
    SHR_IF_ERR_EXIT(bcm_l2_learn_limit_set(0, &limit));
    SHR_IF_ERR_EXIT(bcm_l2_age_timer_meta_cycle_set(unit, global_cycle_time));
    SHR_IF_ERR_EXIT(sh_dnx_l2_clear_all(unit, TRUE, TRUE));
    sal_printf("L2 Limit test passed!\n");
exit:
    diag_sand_packet_stream_destroy(unit, packet_h);
    diag_sand_packet_free(unit, packet_h);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** List of SAT tests   */
/* *INDENT-OFF* */
sh_sand_cmd_t dnx_packet_test_cmds[] = {
    {"tx",         dnx_packet_test_cmd,    NULL, dnx_packet_test_options,     &dnx_packet_test_man,    NULL, NULL,                CTEST_PRECOMMIT},
    {"stream_jr2", dnx_stream_test_cmd,    NULL, dnx_stream_test_options,     &dnx_stream_test_man,    NULL, dnx_stream_tests,    CTEST_PRECOMMIT * SH_CMD_CONDITIONAL, sh_cmd_is_jr2_device},
    {"stream_q2a", dnx_stream_test_cmd,    NULL, dnx_stream_test_options,     &dnx_stream_test_man,    NULL, q2a_stream_tests,    CTEST_PRECOMMIT * SH_CMD_CONDITIONAL, sh_cmd_is_q2a_device},
    {"collector" , dnx_collector_test_cmd, NULL, dnx_collector_test_options,  &dnx_collector_test_man, NULL, dnx_collector_tests, CTEST_PRECOMMIT * SH_CMD_CONDITIONAL, sh_cmd_is_device},
    {"learning"  , dnx_learning_test_cmd,  NULL, dnx_learning_test_options,   &dnx_learning_test_man,  NULL, dnx_learning_tests,  CTEST_PRECOMMIT * SH_CMD_CONDITIONAL, sh_cmd_is_device},
    {"l2_limit"  , dnx_l2_limit_test_cmd,  NULL, dnx_l2_limit_test_options,   &dnx_l2_limit_test_man,  NULL, dnx_l2_limit_tests,  CTEST_PRECOMMIT * SH_CMD_CONDITIONAL, sh_cmd_is_device},
    {NULL}
};

/** \file diag_dnx_ipq_tests.c
 * 
 * Tests for IPQ
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

#include <bcm/cosq.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <src/bcm/dnx/dram/hbmc/hbmc_monitor.h>
#include <bcm_int/dnx/tune/tune.h>
#include <bcm_int/dnx_dispatch.h>

#define DNX_DRAM_TEST_STUB_ARR_LEN (256)
#define CTEST_DNX_DRAM_STUB_MODE_LOW (0)
#define CTEST_DNX_DRAM_STUB_MODE_HIGH (1)
#define CTEST_DNX_DRAM_STUB_MODE_PWR_DOWN (2)

/*
 * \brief
 *   Set the stub base on the input parameters. Driver will return the stub temperature values and opearte accordingly.
 */
static shr_error_e
diag_dnx_dram_stub_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    shr_error_e rv;
    int stub_idx;
    int test_duration_sec, temp_mode;
    int stub_size = 0;
    uint32 temp_stub[DNX_DRAM_TEST_STUB_ARR_LEN];
    uint32 temperature = 0;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("duration", test_duration_sec);
    SH_SAND_GET_INT32("mode", temp_mode);

    LOG_CLI((BSL_META("Test dram temperature, using stub started \n")));

    stub_size = test_duration_sec * SECOND_USEC / dnx_data_dram.hbm.usec_between_temp_samples_get(unit);

    if (stub_size > DNX_DRAM_TEST_STUB_ARR_LEN)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Test duration (%d sec) is too long. Max allowed=%d \n", test_duration_sec,
                     (stub_size * dnx_data_dram.hbm.usec_between_temp_samples_get(unit) / SECOND_USEC));
    }
    if (temp_mode != CTEST_DNX_DRAM_STUB_MODE_LOW && temp_mode != CTEST_DNX_DRAM_STUB_MODE_HIGH &&
        temp_mode != CTEST_DNX_DRAM_STUB_MODE_PWR_DOWN)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Invalid mode(=%d) \n", temp_mode);
    }

    /** set normat temp - traffic goes to DRAM */
    if (temp_mode == CTEST_DNX_DRAM_STUB_MODE_LOW)
    {
        temperature = dnx_data_dram.hbm.restore_traffic_temp_threshold_get(unit) - 1;
    }
    /** set high temp - traffic goes to OCB */
    if (temp_mode == CTEST_DNX_DRAM_STUB_MODE_HIGH)
    {
        temperature = dnx_data_dram.hbm.stop_traffic_temp_threshold_get(unit) + 1;
    }

    /** set one stub for getting to shut down */
    if (temp_mode == CTEST_DNX_DRAM_STUB_MODE_PWR_DOWN)
    {
        temp_stub[0] = dnx_data_dram.hbm.power_down_temp_threshold_get(unit) + 1;
        stub_size = 1;
    }
    else
    {
        for (stub_idx = 0; stub_idx < stub_size; stub_idx++)
        {
            temp_stub[stub_idx] = temperature;
        }
    }

    SHR_IF_ERR_EXIT(dnx_hbmc_temp_stub_set(unit, stub_size, temp_stub));
    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Set dram bound rate class for a given port if "bound_on=1",
 *   otherwise, set default rate class for that port, 
 */
static shr_error_e
diag_dnx_dram_bound_rc_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int port, bound_on;
    int sysport;
    int cosq, flags = 0;
    int rate_class;
    bcm_gport_t base_queue;
    bcm_gport_t voq_gport;
    bcm_gport_t rate_class_gport;
    bcm_switch_profile_mapping_t profile_mapping;
    bcm_cosq_resource_t res_type;
    bcm_cosq_rate_class_create_info_t create_info;
    bcm_port_resource_t port_resource;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("port", port);
    SH_SAND_GET_BOOL("enable", bound_on);
    SH_SAND_GET_INT32("rate_class", rate_class);
    SH_SAND_GET_ENUM("resource", res_type);

    LOG_CLI((BSL_META("Test dram bound rate class started (port=%d, bound_on=%d, resource=%d) \n"), port, bound_on,
             res_type));

    /** get port speed */
    SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, port, &port_resource));

    /** create the rate class. no special attributes are set. */
    BCM_GPORT_PROFILE_SET(rate_class_gport, rate_class);
    create_info.rate = port_resource.speed;
    create_info.attributes = BCM_COSQ_RATE_CLASS_CREATE_ATTR_SLOW_ENABLED;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_rate_class_create(unit, rate_class_gport, flags, &create_info));

    /** get system port from local_port */
    SHR_IF_ERR_EXIT(bcm_dnx_stk_gport_sysport_get(unit, port, &sysport));
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_sysport_ingress_queue_map_get(unit, 0, sysport, &base_queue));
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, base_queue);

    /** map VOQs to rate class */
    profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
    profile_mapping.mapped_profile = rate_class_gport;
    for (cosq = 0; cosq < 8; ++cosq)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_profile_mapping_set(unit, voq_gport, cosq, 0, &profile_mapping));
    }

    if (bound_on == TRUE)
    {
        bcm_cosq_fadt_info_t fadt_info;
        bcm_cosq_fadt_threshold_t fadt_threshold;
        bcm_cosq_threshold_t dram_bound_recovery_threshold;

        /** configure dram bound info */
        fadt_info.gport = rate_class_gport;
        fadt_info.cosq = 0;
        fadt_info.thresh_type = bcmCosqFadtDramBound;
        fadt_info.resource_type = res_type;
        fadt_threshold.thresh_min = 0;
        fadt_threshold.thresh_max = 0;
        fadt_threshold.alpha = 0;
        fadt_threshold.resource_range_min = 0;
        fadt_threshold.resource_range_max = 0;
        /** set configuration */
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_fadt_threshold_set(unit, 0, &fadt_info, &fadt_threshold));

        /** configure dram bound recovery info */
        dram_bound_recovery_threshold.type = bcmCosqThresholdBytes;
        dram_bound_recovery_threshold.flags = BCM_COSQ_THRESHOLD_INGRESS | BCM_COSQ_THRESHOLD_QSIZE_RECOVERY;
        dram_bound_recovery_threshold.value = 0xffffffff;
        /** set configuration */
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, rate_class_gport, 0, &dram_bound_recovery_threshold));
    }
    LOG_CLI((BSL_META("Test dram bound rate class ended \n")));

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t diag_dnx_dram_stub_test_man = {
    "Test hbmc temperature monitor using A stub.",
    "Driver refer to the stub (array of temperatue values) instead of the real temperature and operate accordigly. once all values from stub were read, driver return to normal operation",
    NULL,
    "ctest tm dram stub duration=1 mode=0",
};

static sh_sand_option_t diag_dnx_dram_stub_test_options[] = {
    {"duration", SAL_FIELD_TYPE_INT32, "test duration in seconds", "0"},
    {"mode", SAL_FIELD_TYPE_INT32, "temp stub mode: 0-low, 1-high, 2-power_down", "0"},
    {NULL}
};

static sh_sand_invoke_t diag_dnx_dram_stub_tests[] = {
    {"duration_0", "duration=0 mode=0"},
    {NULL}
};

static sh_sand_man_t diag_dnx_dram_bound_rc_test_man = {
    "Test set (or delete) dram bound rate class for a given port. option to set dram bound threshold on specific resource type",
    NULL,
    NULL,
    "ctest tm dram Rate_Class port=13 bound_on=0",
};

static sh_sand_enum_t diag_dnx_dram_bound_resource_enum_table[] = {
    {"bcmResourceOcbBuffers", 1, "OCB buffers resource"},
    {"bcmResourceOcbBytes", 4, "OCB bytes resource"},
    {"bcmResourceOcbPacketDescriptors", 5, "OCB PDs resource"},
    {NULL}
};

static sh_sand_option_t diag_dnx_dram_bound_rc_test_options[] = {
    {"port", SAL_FIELD_TYPE_INT32, "test duration in seconds", "0"},
    {"enable", SAL_FIELD_TYPE_BOOL, "TRUE for dram bound rate class, FALSE for default rate class", "0"},
    {"rate_class", SAL_FIELD_TYPE_INT32, "rate class profile id to use", "30"},
    {"resource", SAL_FIELD_TYPE_ENUM, "resource_type to set DRAM bound threshold for",
     "bcmResourceOcbPacketDescriptors", (void *) diag_dnx_dram_bound_resource_enum_table},
    {NULL}
};

static sh_sand_invoke_t diag_dnx_dram_bound_rc_tests[] = {
    {"bound_on", "port=13 enable=1"},
    {"bound_off", "port=13 enable=0"},
    {"bound_on_res_type_bytes", "port=14 enable=1 rate_class=5 resource=bcmResourceOcbBytes"},
    {"bound_on_res_type_buffers", "port=15 enable=1 rate_class=46 resource=bcmResourceOcbBuffers"},
    {NULL}
};

sh_sand_cmd_t sh_dnx_tm_dram_test_cmds[] = {
    {"stub", diag_dnx_dram_stub_test_cmd, NULL, diag_dnx_dram_stub_test_options, &diag_dnx_dram_stub_test_man, NULL,
     diag_dnx_dram_stub_tests, 0},
    {"Rate_Class", diag_dnx_dram_bound_rc_test_cmd, NULL, diag_dnx_dram_bound_rc_test_options,
     &diag_dnx_dram_bound_rc_test_man, NULL,
     diag_dnx_dram_bound_rc_tests, 0},
    {NULL}
};

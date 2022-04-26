/** \file ctest_dnx_fc_tests.c
 *
 * Tests for Flow Control
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_TESTS

/*************
 * INCLUDES  *
 *************/
/** appl */
#include <appl/diag/dnx/diag_dnx_utils.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
/** bcm */
#include <bcm/cosq.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/port/imb/imb.h>
/** soc */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
/** local */
#include "ctest_dnx_fc_tests.h"

/*************
 *   ENUMS   *
 *************/

/**
 * \brief - Enumerator for IMB types supported by the PFC Deadlock breaking mechanism
 */
typedef enum
{
/** Invalid IMB type */
    DNX_FC_TEST_IMB_TYPE_INVALID = -1,
/** IMB type first */
    DNX_FC_TEST_IMB_TYPE_FIRST = 0,
/** IMB type CDUM */
    DNX_FC_TEST_IMB_TYPE_CDUM = DNX_FC_TEST_IMB_TYPE_FIRST,
/** IMB type CDU */
    DNX_FC_TEST_IMB_TYPE_CDU = 1,
/** IMB type CLU */
    DNX_FC_TEST_IMB_TYPE_CLU = 2,
/** Number of IMB types */
    DNX_FC_TEST_IMB_TYPE_COUNT
} dnx_fc_test_imb_type_t;

/**************
 * STRUCTURES *
 **************/

/**
 * \brief - Data structure for the ports parameters in PFC Deadlock test
 */
typedef struct dnx_fc_test_port_parameters
{
    int first_phy;        /** First Phy */
    int number_of_lanes;  /** Number of lanes */
    int speed;            /** Speed in Mb/s */
    int nof_ports_in_bundle; /** Number of ports to add with these parameters. CDU/CDB have only 1 PM in block, so only 1 port is needed. But CLU has multiple PMs in block */
    int ports_array[DNX_DATA_MAX_NIF_ETH_NOF_PMS_IN_CLU]; /** Array of logical port numbers. CDU/CDB have only 1 PM in block, so only 1 port is needed. But CLU has multiple PMs in block */
} dnx_fc_test_port_parameters_t;


static shr_error_e
dnx_fc_pfc_deadlock_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int detection_time;
    int detection_time_actual;
    int recovery_time;
    uint8 need_to_add_cdum;
    uint8 need_to_add_cdu;
    uint8 need_to_add_clu;
    uint32 pcp;
    uint32 iterator;
    int current_phy;
    int ilkn_port = 0;
    int ilkn_ports_nof = 0;
    bcm_pbmp_t ilkn_ports;
    rhhandle_t ctest_soc_set_h = NULL;
    ctest_soc_property_t *ctest_soc_property = NULL;
    ctest_cint_argument_t cint_arguments[10];
    dnx_fc_test_imb_type_t imb_type;
    dnx_fc_test_port_parameters_t port_parameters[DNX_FC_TEST_IMB_TYPE_COUNT];
    bcm_instru_synced_counters_config_t config;
    bcm_cosq_fc_endpoint_t fc_reception;
    bcm_cosq_fc_endpoint_t fc_target;
    char port_to_remove[RHNAME_MAX_SIZE];
#if !defined(ADAPTER_SERVER_MODE)
    int occurences;
    int occurences_temp;
    int test_case;
    int test_duration;
    int sleep_time;
#endif
#ifdef BCM_DNX2_SUPPORT
    int ethu_index, table_index;
#endif

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Get input parameters
     */
    SH_SAND_GET_INT32("detection_time", detection_time);

    /** Set the port parameters strukt to 0 */
    sal_memset(&port_parameters, 0, sizeof(dnx_fc_test_port_parameters_t) * DNX_FC_TEST_IMB_TYPE_COUNT);

    /** Remove all ILKN ports using SOC property */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, 0, &ilkn_ports));
    BCM_PBMP_COUNT(ilkn_ports, ilkn_ports_nof);
    SHR_ALLOC_SET_ZERO(ctest_soc_property, sizeof(ctest_soc_property_t) * (ilkn_ports_nof + 1), "ctest_soc_property",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);
    iterator = 0;
    BCM_PBMP_ITER(ilkn_ports, ilkn_port)
    {
        sal_snprintf(port_to_remove, RHNAME_MAX_SIZE, "ucode_port_%d*", ilkn_port);
        SHR_ALLOC_SET_ZERO(ctest_soc_property[iterator].property, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                           "ctest_soc_property.property", "%s%s%s", EMPTY, EMPTY, EMPTY);
        sal_strncpy(ctest_soc_property[iterator].property, port_to_remove, RHNAME_MAX_SIZE);
        ctest_soc_property[iterator].value = NULL;
        iterator++;
    }
    /** Last member must be NULL */
    ctest_soc_property[iterator].property = NULL;
    ctest_soc_property[iterator].value = NULL;
    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));

    /** Load cints needed for ports dynamic procedures */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/port/cint_dynamic_port_add_remove.c"),
                        "cint_dynamic_port_add_remove.c Load Failed\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/internal/port/cint_test_dynamic_port_add.c"),
                        "cint_dynamic_port_add_remove.c Load Failed\n");
    /** Remove all NIF ports */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "cint_dyn_port_remove_all", NULL, 0),
                        "cint_dyn_port_remove_all Run Failed\n");

    /**
     * Configure pulse generator.
     * Detection timer must be be contained within the pulse generator period,
     * as each pulse zeroes the counters. So we make pulse period much bigger.
     * Also the period is in nanosecond, and detection_time is in milliseconds, so multiply by a million.
     * Start time must be immediate ,so it should be set to -1.
     */
    COMPILER_64_SET(config.period, 0, (detection_time * 4));
    COMPILER_64_UMUL_32(config.period, 1000000);
    config.enable = 1;
    config.is_eventor_collection = 0;
    config.is_continuous = 1;
    COMPILER_64_SET(config.start_time, 0xFFFFFFFF, 0xFFFFFFFF);
    SHR_IF_ERR_EXIT(bcm_instru_synced_counters_config_set(unit, 0, bcmInstruSyncedCountersTypeNif, &config));

    /** Configure the Chip behavior during recovery */
    SHR_IF_ERR_EXIT(bcm_switch_control_set
                    (unit, bcmSwitchPFCDeadlockRecoveryAction, bcmSwitchPFCDeadlockActionTransmit));

    /** There are 3 types of ETHUs that need to be tested - CDUM/CDBM, CDU/CDB and CLU.
     *  Determine which ones exist on the current device */
    need_to_add_cdum = (dnx_data_nif.eth.cdum_nof_get(unit) != 0) ? TRUE : FALSE;
    need_to_add_cdu = (dnx_data_nif.eth.cdu_nof_get(unit) != 0) ? TRUE : FALSE;
    need_to_add_clu = (dnx_data_nif.eth.clu_nof_get(unit) != 0) ? TRUE : FALSE;

    for (current_phy = 0; current_phy < dnx_data_nif.phys.nof_phys_get(unit); current_phy++)
    {
        if ((need_to_add_cdu == TRUE) || (need_to_add_cdum == TRUE) || (need_to_add_clu == TRUE))
        {
#ifdef BCM_DNX2_SUPPORT
            /** Get the PHY's ETHU index and check the IMB Type */
            table_index = current_phy / dnx_data_nif.eth.phy_map_granularity_get(unit);
            ethu_index = dnx_data_nif.eth.phy_map_get(unit, table_index)->ethu_index;
            if ((dnx_data_nif.eth.ethu_properties_get(unit, ethu_index)->type == imbDispatchTypeImb_cdu))
            {
                /** IMB Type is CDU/CDB, but it could also be CDUM/CDBM */
                if ((ethu_index % dnx_data_nif.eth.ethu_nof_per_core_get(unit) == 0) && (need_to_add_cdum == TRUE))
                {
                    /** This means the ETHU type is CDUM/CDBM. If CDUM/CDBM exists, we assume that is is always the first ETHU of the core.
                     *  Mark that we no longer need to add CDUM/CDBM, because we are adding it at this iteration. */
                    need_to_add_cdum = FALSE;
                    port_parameters[DNX_FC_TEST_IMB_TYPE_CDUM].ports_array[0] = current_phy + 10;
                    port_parameters[DNX_FC_TEST_IMB_TYPE_CDUM].nof_ports_in_bundle = 1;
                    port_parameters[DNX_FC_TEST_IMB_TYPE_CDUM].first_phy = current_phy;
                    port_parameters[DNX_FC_TEST_IMB_TYPE_CDUM].number_of_lanes =
                        dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);
                    port_parameters[DNX_FC_TEST_IMB_TYPE_CDUM].speed = 400000;
                }

                if ((ethu_index % dnx_data_nif.eth.ethu_nof_per_core_get(unit) > 0) && (need_to_add_cdu == TRUE))
                {
                    /** This means the ETHU type is CDU/CDB.
                     *  Mark that we no longer need to add CDU/CDB, because we are adding it at this iteration. */
                    need_to_add_cdu = FALSE;
                    port_parameters[DNX_FC_TEST_IMB_TYPE_CDU].ports_array[0] = current_phy + 10;;
                    port_parameters[DNX_FC_TEST_IMB_TYPE_CDU].nof_ports_in_bundle = 1;
                    port_parameters[DNX_FC_TEST_IMB_TYPE_CDU].first_phy = current_phy;
                    port_parameters[DNX_FC_TEST_IMB_TYPE_CDU].number_of_lanes =
                        dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);
                    port_parameters[DNX_FC_TEST_IMB_TYPE_CDU].speed = 400000;
                }
            }
            if ((dnx_data_nif.eth.ethu_properties_get(unit, ethu_index)->type == imbDispatchTypeImb_clu)
                && (need_to_add_clu == TRUE))
            {
                /** This means the ETHU type is CLU.
                 *  Mark that we no longer need to add CLU, because we are adding it at this iteration.
                 *  CLU blocks contain 4 PMs, 4 lanes each. Since this test is using fore PFC, it forces the signal to the entire block .
                 *  This means that there should be ports defined on all lanes, but the only port that spans on that many lanes is ILKN.
                 *  Since ILKN ports are not supported by the PFC Deadlock Breaking mechanism, we need to use several ports */
                need_to_add_clu = FALSE;
                port_parameters[DNX_FC_TEST_IMB_TYPE_CLU].nof_ports_in_bundle =
                    dnx_data_nif.eth.nof_pms_in_clu_get(unit);
                port_parameters[DNX_FC_TEST_IMB_TYPE_CLU].first_phy = current_phy;
                port_parameters[DNX_FC_TEST_IMB_TYPE_CLU].number_of_lanes =
                    dnx_data_nif.eth.nof_clu_lanes_in_pm_get(unit);
                port_parameters[DNX_FC_TEST_IMB_TYPE_CLU].speed = 100000;
                for (iterator = 0; iterator < dnx_data_nif.eth.nof_pms_in_clu_get(unit); iterator++)
                {
                    port_parameters[DNX_FC_TEST_IMB_TYPE_CLU].ports_array[iterator] = current_phy + iterator + 10;;
                }
            }
#endif
        }
        else
        {
            /** If all the needed port types were already added, no need to continue */
            break;
        }
    }

    /** Set the cint arguments structure with initial values */
    for (iterator = 0; iterator < 10; iterator++)
    {
        cint_arguments[iterator].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[iterator].value.value_int32 = 0;
    }

    /** Add the needed ports */
    for (imb_type = DNX_FC_TEST_IMB_TYPE_FIRST; imb_type < DNX_FC_TEST_IMB_TYPE_COUNT; imb_type++)
    {
        if (port_parameters[imb_type].nof_ports_in_bundle == 0)
        {
            /** If no ports of the current IMB type need to be tested, skip */
            continue;
        }

        /** Add all needed ports of the current type and configure them to receive PFC */
        for (iterator = 0; iterator < port_parameters[imb_type].nof_ports_in_bundle; iterator++)
        {
            /** Logical Port */
            cint_arguments[0].value.value_int32 = port_parameters[imb_type].ports_array[iterator];
            /** Channel */
            cint_arguments[1].value.value_int32 = 0;
            /** TM Port */
            cint_arguments[2].value.value_int32 = port_parameters[imb_type].ports_array[iterator];
            /** Number of priorities */
            cint_arguments[3].value.value_int32 = 8;
            /** First PHY */
            cint_arguments[4].value.value_int32 =
                port_parameters[imb_type].first_phy + (port_parameters[imb_type].number_of_lanes * iterator);
            /** Interface type*/
            cint_arguments[5].value.value_int32 = BCM_PORT_IF_NIF_ETH;
            /** Number of lanes */
            cint_arguments[6].value.value_int32 = port_parameters[imb_type].number_of_lanes;
            /** Port speed in  Mb/s */
            cint_arguments[7].value.value_int32 = port_parameters[imb_type].speed;
            /** Flags */
            cint_arguments[8].value.value_int32 = 0;
            /** Interface Offset */
            cint_arguments[9].value.value_int32 = 0;
            SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "dynamic_port_add", cint_arguments, 10),
                                "dynamic_port_add Run Failed\n");

            /**
             * Configure the in_port to be able to receive PFC
             */
            SHR_IF_ERR_EXIT(bcm_port_control_set
                            (unit, port_parameters[imb_type].ports_array[iterator], bcmPortControlPFCReceive, 1));
        }

        for (pcp = 0; pcp < BCM_COS_COUNT; pcp++)
        {
            /** Configure all ports in the current bundle */
            for (iterator = 0; iterator < port_parameters[imb_type].nof_ports_in_bundle; iterator++)
            {
                /**
                 * Set path for FC reception on the in_port. The frames that it generated will; be received back at it
                 */
                BCM_GPORT_LOCAL_SET(fc_target.port, port_parameters[imb_type].ports_array[iterator]);
                BCM_GPORT_LOCAL_SET(fc_reception.port, port_parameters[imb_type].ports_array[iterator]);
                fc_target.flags = fc_reception.flags = 0;
                fc_reception.calender_index = -1;
                fc_target.cosq = pcp;
                fc_reception.cosq = pcp;
                SHR_IF_ERR_EXIT(bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target));

                /**
                 * Configure the Detection timer's granularity to 1 ms.
                 */
                SHR_IF_ERR_EXIT(bcm_cosq_pfc_deadlock_control_set
                                (unit, port_parameters[imb_type].ports_array[iterator], pcp,
                                 bcmCosqPFCDeadlockTimerGranularity, bcmCosqPFCDeadlockTimerInterval1MiliSecond));

                /**
                 * Configure the Detection timer (in milliseconds)
                 */
                SHR_IF_ERR_EXIT(bcm_cosq_pfc_deadlock_control_set
                                (unit, port_parameters[imb_type].ports_array[iterator], pcp,
                                 bcmCosqPFCDeadlockDetectionTimer, detection_time));

                /**
                 * Get the Detection timer (in milliseconds) to have the actual value that is configured in HW.
                 * It can be slightly different from what was set due to converting to internal time (clocks)
                 */
                SHR_IF_ERR_EXIT(bcm_cosq_pfc_deadlock_control_get
                                (unit, port_parameters[imb_type].ports_array[iterator], pcp,
                                 bcmCosqPFCDeadlockDetectionTimer, &detection_time_actual));

                /**
                 * Configure the Recovery timer in milliseconds.
                 * Must be bigger than the detection time, so that if the interrupt is triggered one time it will not be triggered again
                 * during the test duration.
                 */
                recovery_time = detection_time * 2;
                SHR_IF_ERR_EXIT(bcm_cosq_pfc_deadlock_control_set
                                (unit, port_parameters[imb_type].ports_array[iterator], pcp,
                                 bcmCosqPFCDeadlockRecoveryTimer, recovery_time));

                /**
                 * Enable the PFC Deadlock recovery after all configuration is done
                 */
                SHR_IF_ERR_EXIT(bcm_cosq_pfc_deadlock_control_set
                                (unit, port_parameters[imb_type].ports_array[iterator], pcp,
                                 bcmCosqPFCDeadlockDetectionAndRecoveryEnable, 1));
            }
#if !defined(ADAPTER_SERVER_MODE)
            /**
             * Test the two cases (traffic skipped in adapter)
             * Case 0: the test_duration to be less than the detection time.
             * Case 1: the test_duration to be more than the detection time.
             */
            for (test_case = 0; test_case < 2; test_case++)
            {
                /**
                 * Reset the occurrences to 0 on all ports of the current bundle
                 */
                for (iterator = 0; iterator < port_parameters[imb_type].nof_ports_in_bundle; iterator++)
                {
                    SHR_IF_ERR_EXIT(bcm_cosq_pfc_deadlock_control_set
                                    (unit, port_parameters[imb_type].ports_array[iterator], pcp,
                                     bcmCosqPFCDeadlockRecoveryOccurrences, 0));
                }

                if (test_case == 0)
                {
                    /**
                     * For the first test scenario set the test_duration to be less than the detection time by 50%.
                     */
                    test_duration = detection_time_actual - (detection_time_actual * 50 / 100);
                }
                else
                {
                    /**
                     * For the second test scenario set the test_duration to be more than the detection time by 100% (double).
                     * This is done so that even if the pulse generator will happen during the testing, the interrupt will still
                     * have time to be triggered
                     */
                    test_duration = detection_time_actual * 2;
                }

                /**
                 * Set the enable for forcing PFC signal on the first port of the bundle.
                 * The Force signal is for the entire block, so no need to go over all ports in the bundle
                 */
                SHR_IF_ERR_EXIT(imb_port_pfc_force_enable_set
                                (unit, port_parameters[imb_type].ports_array[0], pcp, TRUE));

                /**
                 * After the needed duration stop the forcing of PFC signal
                 */
                sal_msleep(test_duration);
                SHR_IF_ERR_EXIT(imb_port_pfc_force_enable_set
                                (unit, port_parameters[imb_type].ports_array[0], pcp, FALSE));

                /**
                 * Get the occurrences on all ports of the current bundle to check if the interrupt was triggered
                 */
                occurences = 0;
                for (iterator = 0; iterator < port_parameters[imb_type].nof_ports_in_bundle; iterator++)
                {
                    occurences_temp = 0;
                    SHR_IF_ERR_EXIT(bcm_cosq_pfc_deadlock_control_get
                                    (unit, port_parameters[imb_type].ports_array[iterator], pcp,
                                     bcmCosqPFCDeadlockRecoveryOccurrences, &occurences_temp));
                    occurences += occurences_temp;
                }
                LOG_CLI(("Testing case %d and duration %d with TC %d and detection time %d...interrupt was triggered %d times.\n", test_case, test_duration, pcp, detection_time_actual, occurences));

                /**
                 * Check test results
                 */
                if ((test_case == 0) && (occurences != 0))
                {
                    /**
                     * For the first test scenario set the test_duration to be less than the detection time.
                     * Interrupt should not be triggered.
                     */
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                                 "FAILURE - Interrupt should not be triggered, but it was, got %d occurrences.\n",
                                 occurences);
                }

                if ((test_case == 1) && (occurences != 1))
                {
                    /**
                     * For the second test scenario set the test_duration to be more than the detection time.
                     * Interrupt should be triggered exactly 1 time.
                     */
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                                 "FAILURE - Interrupt should be triggered exactly 1 time, but got %d occurrences.\n",
                                 occurences);
                }

                /**
                 * Sleep to allow the recovery time to expire before the next test
                 */
                sleep_time = recovery_time * 2;
                sal_msleep(sleep_time);
            }
#endif
            /** Clear configuration for all ports in the bundle */
            for (iterator = 0; iterator < port_parameters[imb_type].nof_ports_in_bundle; iterator++)
            {
                /**
                 * Disable the PFC Deadlock recovery after all testing is done
                 */
                SHR_IF_ERR_EXIT(bcm_cosq_pfc_deadlock_control_set
                                (unit, port_parameters[imb_type].ports_array[iterator], pcp,
                                 bcmCosqPFCDeadlockDetectionAndRecoveryEnable, 0));

                /**
                 * Delete the path for FC reception
                 */
                BCM_GPORT_LOCAL_SET(fc_target.port, port_parameters[imb_type].ports_array[iterator]);
                BCM_GPORT_LOCAL_SET(fc_reception.port, port_parameters[imb_type].ports_array[iterator]);
                SHR_IF_ERR_EXIT(bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_reception, &fc_target));
            }
        }

        /** Delete the ports of the current bundle */
        for (iterator = 0; iterator < port_parameters[imb_type].nof_ports_in_bundle; iterator++)
        {
            /** Logical Port */
            cint_arguments[0].value.value_int32 = port_parameters[imb_type].ports_array[iterator];
            /** Flags */
            cint_arguments[1].value.value_int32 = 0;
            SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "cint_dyn_port_remove_port_full_example", cint_arguments, 2),
                                "cint_dyn_port_remove_port_full_example Run Failed\n");
        }
    }

exit:
    if (ctest_soc_property != NULL)
    {
        for (iterator = 0; iterator < (ilkn_ports_nof + 1); iterator++)
        {
            SHR_FREE(ctest_soc_property[iterator].property);
            SHR_FREE(ctest_soc_property[iterator].value);
        }
    }
    SHR_FREE(ctest_soc_property);
    if (ctest_soc_set_h != NULL)
    {
        ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    }
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_fc_pfc_deadlock_list_options[] = {
    {"detection_time", SAL_FIELD_TYPE_INT32, "Detection timer in milliseconds"},
    {NULL}
};

static sh_sand_man_t dnx_fc_pfc_deadlock_man = {
    .brief = "Run a test for PFC Deadlock detection mechanism.",
    .full =
        "Run a test for PFC Deadlock detection mechanism- Configure PFC Deadlock detection mechanism and test with traffic. Make sure mechanism is triggered only when the PFC XOFF state last longer than the specified detection time",
    .synopsis = NULL,
    .examples = "detection_time=1000"
};

static sh_sand_invoke_t dnx_fc_pfc_deadlock_tests[] = {
    {"detection_time", "detection_time=1000"},
    {NULL}
};

/**
 * \brief DNX Flow Control ctests
 * List of the supported ctests, pointer to command function and
 * command usage function. This is the entry point for Flow Control ctest commands
 */
sh_sand_cmd_t dnx_fc_test_cmds[] = {
    {"pfc_deadlock", dnx_fc_pfc_deadlock_cmd, NULL, dnx_fc_pfc_deadlock_list_options, &dnx_fc_pfc_deadlock_man, NULL,
     dnx_fc_pfc_deadlock_tests},
    {NULL}
};

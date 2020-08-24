/** \file diag_dnx_ipq_tests.c
 * 
 * Tests for IPQ
 * 
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_TESTDRAM

#include <shared/shrextend/shrextend_debug.h>
#include <sal/appl/sal.h>
#include <sal/core/boot.h>
#include <bcm/cosq.h>
#include <bcm/sat.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <src/bcm/dnx/dram/hbmc/hbmc_monitor.h>
#include <bcm_int/dnx/tune/tune.h>
#include <bcm_int/dnx_dispatch.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <appl/diag/dnxc/diag_dnxc_diag.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <sal/compiler.h>
#include "ctest_dnx_dram_tests.h"
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/tune/tune.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ingress_congestion_access.h>
#include <bcm_int/dnx/init/init.h>

#define DNX_DRAM_TEST_STUB_ARR_LEN                 (256)
#define CTEST_DNX_DRAM_STUB_MODE_NORMAL            (0)
#define CTEST_DNX_DRAM_STUB_MODE_HIGH              (1)
#define CTEST_DNX_DRAM_STUB_MODE_LOW               (2)
#define CTEST_DNX_DRAM_STUB_MODE_PWR_DOWN_ON_HIGH  (3)
#define CTEST_DNX_DRAM_STUB_MODE_PWR_DOWN_ON_LOW   (4)
/*
 * Default values for the control structure 'operation_buffer_params' in
 * ctest_dnx_dram_operation_buffer_test_cmd().
 * The cycles are set as follows:
 * Cycle no.             Description
 *            Rate              using DRAM?   Enq/Deq
 *    0       High (starter)      Yes          Yes
 *    1       Low                 No           No
 *    2       High (starter)      Yes          Yes
 *    3       Low                 No           No
 *    4       High (starter)      Yes          Yes
 *    5       Low                 No           No
 */
static const ctest_dnx_dram_operation_buffer_params_t Default_buffer_params = {
    .ctest_dnx_dram_operation_buffer_cycles = 6,
    .ctest_dnx_dram_num_port_in_snake = 2,
    .sat_injection_rate_kbps = {11000000, 1000000, 11000000, 1000000, 11000000, 1000000},
    .sat_burst_size = {100, 100, 100, 100, 100, 100},
    .sat_granularity_size = {200000, 200000, 200000, 200000, 200000, 200000, 0},
    .delay_before_inspect_in_milli = {10000, 10000, 10000, 10000, 10000, 10000},
    .num_sec_to_wait_for_rate = {20, 20, 20, 20, 20, 20},
    .expected_rate_in_mbps = {10000, 1000, 10000, 1000, 10000, 1000, 0},
    .expected_rate_deviation_in_percents = {5, 5, 5, 5, 5, 5, 0},
    .verify_clear_at_end_of_cycle = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0},
    .expect_dram_to_be_used = {TRUE, FALSE, TRUE, FALSE, TRUE, FALSE, 0},
    .expect_enq_deq_dram = {TRUE, FALSE, TRUE, FALSE, TRUE, FALSE, 0}
};
/*
 * Default values for the control structure 'operation_rate_params' in
 * ctest_dnx_dram_operation_rate_test_cmd(). For J2C and Q2A, PM4x25 only support CGE
 * Port rates:
 *   CDGE ==> 400G
 *   CCGE ==> 200G
 *   CGE  ==> 100G
 */
static const ctest_dnx_dram_operation_rate_params_t Default_rate_params = {
    .ctest_dnx_dram_operation_rate_cycles = 1,
    .ctest_dnx_dram_num_port_in_snake = 96,
    .ctest_dnx_num_phys_ports_in_one_combined_port = 4,
    .ctest_dnx_text_ports = "CCGE",
    .ctest_dnx_use_dram_only = {TRUE, TRUE, TRUE},
    .ctest_dnx_apply_debug_severity = FALSE,
    .sat_injection_rate_kbps = {98500000, 200000000, 0},
    .sat_packet_size_bytes = {512, 512, 0},
    .sat_burst_size = {1000, 1000, 0},
    .sat_granularity_size = {2000000, 2000000, 0},
    .delay_before_inspect_in_milli = {10000, 10000, 0},
    .num_sec_to_wait_for_rate = {20, 20, 0},
    .expected_rate_in_mbps = {98500, 98000, 0},
    .expected_rate_deviation_in_percents = {5, 5, 0},
    .verify_clear_at_end_of_cycle = {FALSE, FALSE, 0},
    .expect_dram_to_be_used = {TRUE, FALSE, 0},
    .expected_dram_rate_in_mbps = {1850000, 2000000, 0},
    .expected_dram_rate_deviation_in_percents = {5, 5, 0},
    .expect_enq_deq_dram = {TRUE, FALSE, 0}
};

/**
 * \brief
 *   This function check hbm is available
 *
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] test_list -
 *   test list
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   ctest_dnx_dram_operation_buffer_test_cmd
 */
static shr_error_e
ctest_dnx_dram_hbm_available(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_init_is_init_done_get(unit))
    {
        SHR_EXIT();
    }
    /** Not supported for devices without hbm support */
    if (!dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function loads default parameters into structure of
 *   type ctest_dnx_dram_operation_buffer_params_t
 *
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [out] buffer_params_p -
 *   Pointer to ctest_dnx_dram_operation_buffer_params_t. This procedure loads pointed
 *   memory by default parameters. Used when starting 'operation buffer' test.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   ctest_dnx_dram_operation_buffer_test_cmd
 */
static shr_error_e
diag_dnx_dram_buffer_defaults_load(
    int unit,
    ctest_dnx_dram_operation_buffer_params_t * buffer_params_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(buffer_params_p, _SHR_E_INTERNAL, "buffer_params_p");
    if (Default_buffer_params.ctest_dnx_dram_operation_buffer_cycles > CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Specified number of cycles (%d) is larger than allowed (%d). Quit test. \r\n",
                     Default_buffer_params.ctest_dnx_dram_operation_buffer_cycles,
                     CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES);
    }
    *buffer_params_p = Default_buffer_params;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function Loads default parameters into structure of
 *   type ctest_dnx_dram_operation_rate_params_t
 *
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [out] rate_params_p -
 *   Pointer to ctest_dnx_dram_operation_rate_params_t. This procedure loads pointed
 *   memory by default parameters. Used when starting 'operation rate' test.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   ctest_dnx_dram_operation_rate_test_cmd
 */
static shr_error_e
diag_dnx_dram_rate_defaults_load(
    int unit,
    ctest_dnx_dram_operation_rate_params_t * rate_params_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(rate_params_p, _SHR_E_INTERNAL, "rate_params_p");
    if (Default_rate_params.ctest_dnx_dram_operation_rate_cycles > CTEST_DNX_OPERATION_RATE_MAX_CYCLES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Specified number of cycles (%d) is larger than allowed (%d). Quit test. \r\n",
                     Default_rate_params.ctest_dnx_dram_operation_rate_cycles, CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES);
    }
    *rate_params_p = Default_rate_params;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Verify, on specified core, that all DRAM 'delete/drop' counters are zero
 *   and that all DRAM activity counters are also zero.
 *   Make sure DRAM size (number of bytes in use) is zero.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core_id -
 *   int. Identifier of core for which to retrieve info.
 * \param [out] all_activity_is_halted_p -
 *   Pointer to int. This procedure loads pointed memory by an indication:
 *   If TRUE then all counters have reached their zero values.
 * \return
 *   Error code (as per 'shr_error_e').
 * \remark
 *   This procedure checks activity counters, via DBAL tables, and waits
 *   some time to let them reach their zero values.
 * \see
 *   shr_error_e
 *   ctest_dnx_dram_operation_buffer_test_cmd
 *   ctest_dnx_dram_operation_rate_test_cmd
 *   DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS
 */
static shr_error_e
ctest_dnx_dram_inactivity_verify(
    int unit,
    int core_id,
    int *all_activity_is_halted_p)
{
    int nof_cores;

    SHR_FUNC_INIT_VARS(unit);
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (core_id >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input core (%d). Must be smaller than %d. Quit.\r\n", core_id, nof_cores);
    }
    *all_activity_is_halted_p = FALSE;
    {
        int loop_again;
        int max_loop, loop_counter;
        dbal_tables_e dbal_table;
        uint32 uint64_array[SAL_UINT64_NOF_BITS / SAL_UINT32_NOF_BITS];
        uint32 sram_to_dram_deq_pkt_ctr, dram_deq_bundle_ctr;
        uint32 sram_enq_rjct_pkt_ctr, sram_del_pkt_ctr, dram_del_bundle_ctr;

        loop_again = TRUE;
        max_loop = CTEST_DNX_DRAM_NUM_LOOPS_FOR_INACTIVITY;
        loop_counter = 0;
        while (loop_again)
        {
            if (loop_counter >= max_loop)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Core %d, loop_counter (%d) has reached its limit (%d) %s\r\n",
                             core_id, loop_counter, max_loop, EMPTY);
                break;
            }
            dbal_table = DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS;
            SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, dbal_table, 1, 5,
                                             /*
                                              * key construction.
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core_id,
                                             /*
                                              * Getting value of VOQ_SRAM_ENQ_RJCT_PKT_CTRf, VOQ_SRAM_DEL_PKT_CTRf, VOQ_DRAM_DEL_BUNDLE_CTRf.
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_SRAM_ENQ_RJCT_PKT_CTR, INST_SINGLE,
                                             &sram_enq_rjct_pkt_ctr, GEN_DBAL_FIELD32, DBAL_FIELD_SRAM_DEL_PKT_CTR,
                                             INST_SINGLE, &sram_del_pkt_ctr, GEN_DBAL_FIELD32,
                                             DBAL_FIELD_DRAM_DEL_BUNDLE_CTR, INST_SINGLE, &dram_del_bundle_ctr,
                                             /*
                                              * Getting value of VOQ_SRAM_DEQ_TO_DRAM_PKT_CTRf, VOQ_DRAM_DEQ_TO_FABRIC_BUNDLE_CTRf.
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_SRAM_TO_DRAM_DEQ_PKT_CTR, INST_SINGLE,
                                             &sram_to_dram_deq_pkt_ctr, GEN_DBAL_FIELD32,
                                             DBAL_FIELD_DRAM_DEQ_BUNDLE_CTR, INST_SINGLE, &dram_deq_bundle_ctr,
                                             GEN_DBAL_FIELD_LAST_MARK));
            if ((sram_enq_rjct_pkt_ctr != 0) || (sram_del_pkt_ctr != 0) || (dram_del_bundle_ctr != 0)
                || (sram_to_dram_deq_pkt_ctr != 0) || (dram_deq_bundle_ctr != 0))
            {
                /*
                 * Some 'reject' counters are non zero or DRAM enq/deq activity was found. Loop again.
                 */
                loop_again = TRUE;
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Empty DRAM. loop_counter %d. core %d. Some 'reject' counters or DRAM activity cunters are non zero. Loop again %s%s\r\n",
                             loop_counter, core_id, EMPTY, EMPTY);
                sal_msleep(1000);
            }
            else
            {
                /*
                 * All 'reject' counters are zero. No DRAM activity. Consider exitting loop.
                 */
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Empty DRAM. loop_counter %d. core %d. All 'reject' counters are zero. No DRAM activity. Consider exitting loop. %s%s\r\n",
                             loop_counter, core_id, EMPTY, EMPTY);
                loop_again = FALSE;
            }
            if (loop_again == FALSE)
            {
                int core_dram_was_used;
                /*
                 * Total size in Bytes of current VOQ
                 */
                uint32 queue_size;
                /**
                 * Total size in Bytes of current VOQ in SRAM
                 */
                uint32 queue_size_sram;
                /**
                 * Total size in Bytes of current VOQ in DRAM
                 */
                uint32 queue_size_dram;
                int bits_in_num_voqs;
                uint32 num_voqs;
                uint32 voq_index;

                dbal_table = DBAL_TABLE_INGRESS_CONG_CGM_VOQ_OCCUPANCY;
                core_dram_was_used = FALSE;
                /*
                 * Get number of VOQs by getting maximal value of 'VOQ' field.
                 * (This should equal SAL_BIT(dnx_data_ipq.queues.queue_id_bits_get(unit)))
                 */
                SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                                (unit, dbal_table, DBAL_FIELD_VOQ, TRUE, 0, 0, &bits_in_num_voqs));
                num_voqs = SAL_BIT(bits_in_num_voqs);
                for (voq_index = 0; voq_index < num_voqs; voq_index++)
                {
                    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, dbal_table, 2, 2,
                                                     /*
                                                      * key construction.
                                                      */
                                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core_id,
                                                     GEN_DBAL_FIELD32, DBAL_FIELD_VOQ, voq_index,
                                                     /*
                                                      * Getting value of WORDS_SIZEf, SRAM_WORDS_SIZEf.
                                                      */
                                                     GEN_DBAL_FIELD32, DBAL_FIELD_SIZE_IN_WORDS, INST_SINGLE,
                                                     &queue_size, GEN_DBAL_FIELD32, DBAL_FIELD_SIZE_IN_WORDS_SRAM,
                                                     INST_SINGLE, &queue_size_sram, GEN_DBAL_FIELD_LAST_MARK));
                    /*
                     * Translate Words to Bytes
                     */
                    queue_size *= dnx_data_ingr_congestion.info.words_resolution_get(unit);
                    queue_size_sram *= dnx_data_ingr_congestion.info.words_resolution_get(unit);
                    queue_size_dram = queue_size - queue_size_sram;
                    if (queue_size_dram != 0)
                    {
                        /*
                         * Enter if this queue is occupying DRAM (as well as SRAM, which is OCB)
                         */
                        char uint_as_string[MAX_UINT64_SIZE_AS_STRING];
                        uint64_array[1] = 0;
                        uint64_array[0] = queue_size_dram;
                        SHR_IF_ERR_EXIT(format_value_to_str(uint64_array, SAL_UINT32_NOF_BITS, FALSE, uint_as_string));
                        LOG_DEBUG_EX(BSL_LOG_MODULE, "Empty DRAM. Core %d. VOQ %d is using DRAM (%s bytes) %s\r\n",
                                     core_id, voq_index, uint_as_string, EMPTY);
                        core_dram_was_used = TRUE;
                        break;
                    }
                }
                if (core_dram_was_used == FALSE)
                {
                    *all_activity_is_halted_p = TRUE;
                    LOG_DEBUG_EX(BSL_LOG_MODULE,
                                 "Empty DRAM. Core %d. NO DRAM was used. Exit loop %s%s%s\r\n", core_id, EMPTY, EMPTY,
                                 EMPTY);
                }
                else
                {
                    loop_again = TRUE;
                    sal_msleep(1000);
                }
            }
            loop_counter++;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Detect, on specified core, and inform whether DRAM 'VOQ' counters indicate DRAM
 *   is empty or in use.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core_id -
 *   int. Identifier of core for which to retrieve info.
 * \param [out] some_dram_was_used_p -
 *   Pointer to int. This procedure loads pointed memory by an indication:
 *   If TRUE then DRAM was detected as NOT empty (on one or more cores).
 * \return
 *   Error code (as per 'shr_error_e').
 * \remark
 *   This procedure checks DRAM VOQ counters, via DBAL tables.
 *   If VOQ, on any index or core, contains more bytes than OCB (on chip[ buffer)
 *   can hold then it indicates DRAM is in use.
 * \see
 *   shr_error_e
 *   ctest_dnx_dram_operation_buffer_test_cmd
 *   ctest_dnx_dram_operation_rate_test_cmd
 *   DBAL_TABLE_INGRESS_CONG_CGM_VOQ_OCCUPANCY
 */
static shr_error_e
ctest_dnx_dram_empty_sense(
    int unit,
    int core_id,
    int *some_dram_was_used_p)
{
    int nof_cores;
    dbal_tables_e dbal_table;

    SHR_FUNC_INIT_VARS(unit);
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (core_id >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input core (%d). Must be smaller than %d. Quit.\r\n", core_id, nof_cores);
    }
    dbal_table = DBAL_TABLE_INGRESS_CONG_CGM_VOQ_OCCUPANCY;

    *some_dram_was_used_p = FALSE;
    {
        int core_dram_was_used;
        int bits_in_num_voqs;
        uint32 num_voqs;
        uint32 voq_index;
        /*
         * Total size in Bytes of current VOQ
         */
        uint32 queue_size;
        /**
         * Total size in Bytes of current VOQ in SRAM
         */
        uint32 queue_size_sram;
        /**
         * Total size in Bytes of current VOQ in DRAM
         */
        uint32 queue_size_dram;

        /*
         * Get number of VOQs by getting maximal value of 'VOQ' field.
         * (This should equal SAL_BIT(dnx_data_ipq.queues.queue_id_bits_get(unit)))
         */
        SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, dbal_table, DBAL_FIELD_VOQ, TRUE, 0, 0, &bits_in_num_voqs));
        num_voqs = SAL_BIT(bits_in_num_voqs);
        core_dram_was_used = FALSE;
        for (voq_index = 0; voq_index < num_voqs; voq_index++)
        {
            SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, dbal_table, 2, 2,
                                             /*
                                              * key construction.
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core_id,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_VOQ, voq_index,
                                             /*
                                              * Getting value of WORDS_SIZEf, SRAM_WORDS_SIZEf.
                                              */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_SIZE_IN_WORDS, INST_SINGLE, &queue_size,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_SIZE_IN_WORDS_SRAM, INST_SINGLE,
                                             &queue_size_sram, GEN_DBAL_FIELD_LAST_MARK));
            /*
             * Translate Words to Bytes
             */
            queue_size *= dnx_data_ingr_congestion.info.words_resolution_get(unit);
            queue_size_sram *= dnx_data_ingr_congestion.info.words_resolution_get(unit);
            queue_size_dram = queue_size - queue_size_sram;
            if (queue_size_dram != 0)
            {
                /*
                 * Enter if this queue is occupying DRAM (as well as SRAM, which is OCB)
                 */
                char uint_as_string[MAX_UINT64_SIZE_AS_STRING];
                uint32 uint64_array[SAL_UINT64_NOF_BITS / SAL_UINT32_NOF_BITS];

                uint64_array[1] = 0;
                uint64_array[0] = queue_size_dram;
                SHR_IF_ERR_EXIT(format_value_to_str(uint64_array, SAL_UINT32_NOF_BITS, FALSE, uint_as_string));
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Core %d. VOQ %d is using DRAM (%s bytes) %s\r\n",
                             core_id, voq_index, uint_as_string, EMPTY);
                core_dram_was_used = TRUE;
                *some_dram_was_used_p = TRUE;
            }
        }
        if (core_dram_was_used == FALSE)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Core %d. NO DRAM was used. %s%s%s\r\n", core_id, EMPTY, EMPTY, EMPTY);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Detect, on specified core, and inform 'enq/deq' counters into/from DRAM.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core_id -
 *   int. Identifier of core for which to retrieve info.
 * \param [out] some_enq_deq_dram_p -
 *   Pointer to int. This procedure loads pointed memory by an indication:
 *   If TRUE then enqueue/dequeue activity was detected on DRAM (on one or more cores).
 * \return
 *   Error code (as per 'shr_error_e').
 * \remark
 *   This procedure checks DRAM enq/deq counters, via DBAL tables.
 * \see
 *   shr_error_e
 *   ctest_dnx_dram_operation_buffer_test_cmd
 *   ctest_dnx_dram_operation_rate_test_cmd
 *   DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS
 */
static shr_error_e
ctest_dnx_dram_enq_deq_sense(
    int unit,
    int core_id,
    int *some_enq_deq_dram_p)
{
    int nof_cores;
    dbal_tables_e dbal_table;

    SHR_FUNC_INIT_VARS(unit);
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (core_id >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input core (%d). Must be smaller than %d. Quit.\r\n", core_id, nof_cores);
    }
    dbal_table = DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS;
    /*
     * Detect and display 'enq/deq' counters into/from DRAM.
     */
    *some_enq_deq_dram_p = FALSE;
    {
        int enq_to_dram;
        int deq_from_dram;
        uint32 sram_to_dram_deq_pkt_ctr, dram_deq_bundle_ctr;

        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, dbal_table, 1, 2,
                                         /*
                                          * key construction.
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core_id,
                                         /*
                                          * Getting value of CGM_VOQ_SRAM_DEQ_TO_DRAM_PKT_CTRr, CGM_VOQ_DRAM_DEQ_TO_FABRIC_BUNDLE_CTRr.
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_SRAM_TO_DRAM_DEQ_PKT_CTR, INST_SINGLE,
                                         &sram_to_dram_deq_pkt_ctr, GEN_DBAL_FIELD32, DBAL_FIELD_DRAM_DEQ_BUNDLE_CTR,
                                         INST_SINGLE, &dram_deq_bundle_ctr, GEN_DBAL_FIELD_LAST_MARK));
        enq_to_dram = deq_from_dram = FALSE;
        if (sram_to_dram_deq_pkt_ctr != 0)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Core %d. SRAM_TO_DRAM_DEQ_PKT_CTR is non-zero. Moved %d packets to DRAM %s%s\r\n",
                         core_id, sram_to_dram_deq_pkt_ctr, EMPTY, EMPTY);
            enq_to_dram = TRUE;
        }
        if (dram_deq_bundle_ctr != 0)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Core %d. DRAM_DEQ_BUNDLE_CTR is non-zero. Dequeued %d bundles from DRAM to FABRIC %s%s\r\n",
                         core_id, dram_deq_bundle_ctr, EMPTY, EMPTY);
            deq_from_dram = TRUE;
        }
        if ((enq_to_dram == FALSE) || (deq_from_dram == FALSE))
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Core %d. No bundles were enqueued/dequeued to/from DRAM %s%s%s\r\n", core_id, EMPTY, EMPTY,
                         EMPTY);
        }
        else
        {
            *some_enq_deq_dram_p = TRUE;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Inform, on specified core, the 'enq/deq' counters into/from DRAM.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core_id -
 *   int. Identifier of core for which to retrieve info.
 * \param [out] enq_counter_in_mb_p -
 *   Pointer to uint32. This procedure loads pointed memory by the value of the
 *   'number of bits enqueued from SRAM to DRAM' (in Mega bits)
 * \param [out] enq_overflow_p -
 *   Pointer to int. This procedure loads pointed memory by a flag:
 *   If non zero then 'enq_counter_in_mb_p' is meaningless since it has been
 *   overflown.
 * \param [out] deq_counter_in_mb_p -
 *   Pointer to uint32. This procedure loads pointed memory by the value of the
 *   'number of bits dequeued from DRAM to FABRIC' (in Mega bits)
 * \param [out] deq_overflow_p -
 *   Pointer to int. This procedure loads pointed memory by a flag:
 *   If non zero then 'deq_counter_in_mb_p' is meaningless since it has been
 *   overflown.
 * \return
 *   Error code (as per 'shr_error_e').
 * \remark
 *   This procedure extracts DRAM enq/deq counters, via DBAL tables.
 * \see
 *   shr_error_e
 *   ctest_dnx_dram_operation_buffer_test_cmd
 *   ctest_dnx_dram_operation_rate_test_cmd
 *   DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS
 */
static shr_error_e
ctest_dnx_dram_enq_deq_inform(
    int unit,
    int core_id,
    uint32 *enq_counter_in_mb_p,
    int *enq_overflow_p,
    uint32 *deq_counter_in_mb_p,
    int *deq_overflow_p)
{
    int nof_cores;
    dbal_tables_e dbal_table;
    int is_gtimer_enabled;
    int counter_size;
    COMPILER_UINT64 sram_to_dram_deq_byte_ctr, dram_deq_byte_ctr;
    COMPILER_UINT64 work_reg_64;
    uint32 work_reg_32;

    SHR_FUNC_INIT_VARS(unit);
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (core_id >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input core (%d). Must be smaller than %d. Quit.\r\n", core_id, nof_cores);
    }
    dbal_table = DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS;
    /*
     * Detect and display 'enq/deq' counters into/from DRAM.
     */
    *enq_overflow_p = *deq_overflow_p = FALSE;
    *enq_counter_in_mb_p = *deq_counter_in_mb_p = 0;
    is_gtimer_enabled = 0;
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, dbal_table, 1, 2,
                                     /*
                                      * key construction.
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core_id,
                                     /*
                                      * Getting value of CGM_VOQ_SRAM_DEQ_TO_DRAM_BYTE_CTRr, CGM_VOQ_DRAM_DEQ_TO_FABRIC_BYTE_CTRr.
                                      */
                                     GEN_DBAL_FIELD64, DBAL_FIELD_SRAM_TO_DRAM_DEQ_BYTE_CTR, INST_SINGLE,
                                     &sram_to_dram_deq_byte_ctr, GEN_DBAL_FIELD64,
                                     DBAL_FIELD_DRAM_DEQ_TO_FABRIC_BYTE_CTR, INST_SINGLE, &dram_deq_byte_ctr,
                                     GEN_DBAL_FIELD_LAST_MARK));
    /*
     * Get the gtimer status of block CGM
     */
    SHR_IF_ERR_EXIT(dnx_gtimer_get(unit, SOC_BLK_CGM, core_id, &is_gtimer_enabled));
    /*
     * Get the overflow status and real counter value for SRAM to DRAM Dequeued byte count
     */
    dbal_tables_field_size_get(unit, dbal_table, DBAL_FIELD_SRAM_TO_DRAM_DEQ_BYTE_CTR, 0, 0, 0, &counter_size);
    COMPILER_64_SET(work_reg_64, COMPILER_64_HI(sram_to_dram_deq_byte_ctr), COMPILER_64_LO(sram_to_dram_deq_byte_ctr));
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    COMPILER_64_TO_32_LO(work_reg_32, work_reg_64);
    work_reg_32 &= 0x01;
    *enq_overflow_p = is_gtimer_enabled ? 0 : work_reg_32;
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(sram_to_dram_deq_byte_ctr, work_reg_64);
    COMPILER_64_UMUL_32(sram_to_dram_deq_byte_ctr, SAL_UINT8_NOF_BITS);
    COMPILER_64_UDIV_32(sram_to_dram_deq_byte_ctr, 1000000);
    COMPILER_64_TO_32_LO(work_reg_32, sram_to_dram_deq_byte_ctr);
    *enq_counter_in_mb_p = work_reg_32;
    /*
     * Get the overflow status and real counter value for DRAM to FABRIC Dequeued byte count
     */
    dbal_tables_field_size_get(unit, dbal_table, DBAL_FIELD_DRAM_DEQ_TO_FABRIC_BYTE_CTR, 0, 0, 0, &counter_size);
    COMPILER_64_SET(work_reg_64, COMPILER_64_HI(dram_deq_byte_ctr), COMPILER_64_LO(dram_deq_byte_ctr));
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    COMPILER_64_TO_32_LO(work_reg_32, work_reg_64);
    work_reg_32 &= 0x01;
    *deq_overflow_p = is_gtimer_enabled ? 0 : work_reg_32;
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(dram_deq_byte_ctr, work_reg_64);
    COMPILER_64_UMUL_32(dram_deq_byte_ctr, SAL_UINT8_NOF_BITS);
    COMPILER_64_UDIV_32(dram_deq_byte_ctr, 1000000);
    COMPILER_64_TO_32_LO(work_reg_32, dram_deq_byte_ctr);
    *deq_counter_in_mb_p = work_reg_32;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Inform, on specified core, the 'read/write' counters from/into DDP (DRAM).
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core_id -
 *   int. Identifier of core for which to retrieve info.
 * \param [out] read_counter_in_mb_p -
 *   Pointer to uint32. This procedure loads pointed memory by the value of the
 *   'number of bits read from DRAM' (in Mega bits)
 * \param [out] read_overflow_p -
 *   Pointer to int. This procedure loads pointed memory by a flag:
 *   If non zero then 'read_counter_in_mb_p' is meaningless since it has been
 *   overflown.
 * \param [out] write_counter_in_mb_p -
 *   Pointer to uint32. This procedure loads pointed memory by the value of the
 *   'number of bits written to DRAM' (in Mega bits)
 * \param [out] write_overflow_p -
 *   Pointer to int. This procedure loads pointed memory by a flag:
 *   If non zero then 'write_counter_in_mb_p' is meaningless since it has been
 *   overflown.
 * \return
 *   Error code (as per 'shr_error_e').
 * \remark
 *   This procedure extracts DDP/DRAM read/write counters, via DBAL tables.
 * \see
 *   shr_error_e
 *   ctest_dnx_dram_operation_rate_test_cmd
 *   DBAL_TABLE_TDU_COUNTERS
 */
static shr_error_e
ctest_dnx_ddp_read_write_inform(
    int unit,
    int core_id,
    uint32 *read_counter_in_mb_p,
    int *read_overflow_p,
    uint32 *write_counter_in_mb_p,
    int *write_overflow_p)
{
    int nof_cores;
    dbal_tables_e dbal_table;
    int is_gtimer_enabled;
    int counter_size;
    COMPILER_UINT64 dram_write_byte_ctr, dram_read_byte_ctr;
    COMPILER_UINT64 work_reg_64;
    uint32 work_reg_32;

    SHR_FUNC_INIT_VARS(unit);
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (core_id >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input core (%d). Must be smaller than %d. Quit.\r\n", core_id, nof_cores);
    }
    dbal_table = DBAL_TABLE_TDU_COUNTERS;
    /*
     * Detect and display 'enq/deq' counters into/from DRAM.
     */
    *read_overflow_p = *write_overflow_p = FALSE;
    *read_counter_in_mb_p = *write_counter_in_mb_p = 0;
    is_gtimer_enabled = 0;
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, dbal_table, 1, 2,
                                     /*
                                      * key construction.
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core_id,
                                     /*
                                      * Getting value of WRITE_REQUEST_32_BYTE_COUNTf, READ_REQUEST_32_BYTE_COUNTf.
                                      */
                                     GEN_DBAL_FIELD64, DBAL_FIELD_WRITE_REQUEST_32_BYTE_COUNT, INST_SINGLE,
                                     &dram_write_byte_ctr, GEN_DBAL_FIELD64, DBAL_FIELD_READ_REQUEST_32_BYTE_COUNT,
                                     INST_SINGLE, &dram_read_byte_ctr, GEN_DBAL_FIELD_LAST_MARK));
    /*
     * Get the gtimer status of block TDU
     */
    SHR_IF_ERR_EXIT(dnx_gtimer_get(unit, SOC_BLK_TDU, core_id, &is_gtimer_enabled));
    /*
     * Get the overflow status and real counter value for DRAM read byte (chunk) count
     */
    dbal_tables_field_size_get(unit, dbal_table, DBAL_FIELD_READ_REQUEST_32_BYTE_COUNT, 0, 0, 0, &counter_size);
    COMPILER_64_SET(work_reg_64, COMPILER_64_HI(dram_read_byte_ctr), COMPILER_64_LO(dram_read_byte_ctr));
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    COMPILER_64_TO_32_LO(work_reg_32, work_reg_64);
    work_reg_32 &= 0x01;
    *read_overflow_p = is_gtimer_enabled ? 0 : work_reg_32;
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(dram_read_byte_ctr, work_reg_64);
    COMPILER_64_UMUL_32(dram_read_byte_ctr, SAL_UINT8_NOF_BITS * 32);
    COMPILER_64_UDIV_32(dram_read_byte_ctr, 1000000);
    COMPILER_64_TO_32_LO(work_reg_32, dram_read_byte_ctr);
    *read_counter_in_mb_p = work_reg_32;
    /*
     * Get the overflow status and real counter value for DRAM write byte (chunk) count
     */
    dbal_tables_field_size_get(unit, dbal_table, DBAL_FIELD_WRITE_REQUEST_32_BYTE_COUNT, 0, 0, 0, &counter_size);
    COMPILER_64_SET(work_reg_64, COMPILER_64_HI(dram_write_byte_ctr), COMPILER_64_LO(dram_write_byte_ctr));
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    COMPILER_64_TO_32_LO(work_reg_32, work_reg_64);
    work_reg_32 &= 0x01;
    *write_overflow_p = is_gtimer_enabled ? 0 : work_reg_32;
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(dram_write_byte_ctr, work_reg_64);
    COMPILER_64_UMUL_32(dram_write_byte_ctr, SAL_UINT8_NOF_BITS * 32);
    COMPILER_64_UDIV_32(dram_write_byte_ctr, 1000000);
    COMPILER_64_TO_32_LO(work_reg_32, dram_write_byte_ctr);
    *write_counter_in_mb_p = work_reg_32;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Inform, on specified  core, the 'enq/deq' counters into/from SRAM.
 *   Only dequeue from SRAM to FABRIC is counted.
 *   Note that some bytes, which are enqueued into SRAM, are dequeued into DRAM.
 *   See ctest_dnx_dram_enq_deq_inform().
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] core_id -
 *   int. Identifier of core for which to retrieve info.
 * \param [out] enq_counter_in_mb_p -
 *   Pointer to uint32. This procedure loads pointed memory by the value of the
 *   'number of bits enqueued from RX to SRAM' (in Mega bits)
 * \param [out] enq_overflow_p -
 *   Pointer to int. This procedure loads pointed memory by a flag:
 *   If non zero then 'enq_counter_in_mb_p' is meaningless since it has been
 *   overflown.
 * \param [out] deq_counter_in_mb_p -
 *   Pointer to uint32. This procedure loads pointed memory by the value of the
 *   'number of bits dequeued from SRAM to FABRIC' (in Mega bits)
 * \param [out] deq_overflow_p -
 *   Pointer to int. This procedure loads pointed memory by a flag:
 *   If non zero then 'deq_counter_in_mb_p' is meaningless since it has been
 *   overflown.
 * \return
 *   Error code (as per 'shr_error_e').
 * \remark
 *   This procedure extracts SRAM enq/deq counters, via DBAL tables.
 * \see
 *   shr_error_e
 *   ctest_dnx_dram_operation_buffer_test_cmd
 *   ctest_dnx_dram_operation_rate_test_cmd
 *   DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS
 */
static shr_error_e
ctest_dnx_sram_enq_deq_inform(
    int unit,
    int core_id,
    uint32 *enq_counter_in_mb_p,
    int *enq_overflow_p,
    uint32 *deq_counter_in_mb_p,
    int *deq_overflow_p)
{
    int nof_cores;
    dbal_tables_e dbal_table;
    int is_gtimer_enabled;
    int counter_size;
    COMPILER_UINT64 sram_enq_byte_ctr, sram_deq_byte_ctr;
    COMPILER_UINT64 work_reg_64;
    uint32 work_reg_32;

    SHR_FUNC_INIT_VARS(unit);
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (core_id >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input core (%d). Must be smaller than %d. Quit.\r\n", core_id, nof_cores);
    }
    dbal_table = DBAL_TABLE_INGRESS_CONG_CGM_VOQ_PRG_CTR_STATS;
    /*
     * Detect and display 'enq/deq' counters into/from DRAM.
     */
    *enq_overflow_p = *deq_overflow_p = FALSE;
    *enq_counter_in_mb_p = *deq_counter_in_mb_p = 0;
    is_gtimer_enabled = 0;
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, dbal_table, 1, 2,
                                     /*
                                      * key construction.
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core_id,
                                     /*
                                      * Getting value of CGM_VOQ_SRAM_ENQ_BYTE_CTRr, CGM_VOQ_SRAM_DEQ_TO_FABRIC_BYTE_CTRr.
                                      */
                                     GEN_DBAL_FIELD64, DBAL_FIELD_SRAM_ENQ_BYTE_CTR, INST_SINGLE, &sram_enq_byte_ctr,
                                     GEN_DBAL_FIELD64, DBAL_FIELD_SRAM_TO_FABRIC_DEQ_BYTE_CTR, INST_SINGLE,
                                     &sram_deq_byte_ctr, GEN_DBAL_FIELD_LAST_MARK));
    /*
     * Get the gtimer status of block CGM
     */
    SHR_IF_ERR_EXIT(dnx_gtimer_get(unit, SOC_BLK_CGM, core_id, &is_gtimer_enabled));
    /*
     * Get the overflow status and real counter value for SRAM Enqueued byte count
     */
    dbal_tables_field_size_get(unit, dbal_table, DBAL_FIELD_SRAM_ENQ_BYTE_CTR, 0, 0, 0, &counter_size);
    COMPILER_64_SET(work_reg_64, COMPILER_64_HI(sram_enq_byte_ctr), COMPILER_64_LO(sram_enq_byte_ctr));
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    COMPILER_64_TO_32_LO(work_reg_32, work_reg_64);
    work_reg_32 &= 0x01;
    *enq_overflow_p = is_gtimer_enabled ? 0 : work_reg_32;
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(sram_enq_byte_ctr, work_reg_64);
    COMPILER_64_UMUL_32(sram_enq_byte_ctr, SAL_UINT8_NOF_BITS);
    COMPILER_64_UDIV_32(sram_enq_byte_ctr, 1000000);
    COMPILER_64_TO_32_LO(work_reg_32, sram_enq_byte_ctr);
    *enq_counter_in_mb_p = work_reg_32;
    /*
     * Get the overflow status and real counter value for SRAM to FABRIC Dequeued byte count
     */
    dbal_tables_field_size_get(unit, dbal_table, DBAL_FIELD_SRAM_TO_FABRIC_DEQ_BYTE_CTR, 0, 0, 0, &counter_size);
    COMPILER_64_SET(work_reg_64, COMPILER_64_HI(sram_deq_byte_ctr), COMPILER_64_LO(sram_deq_byte_ctr));
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    COMPILER_64_TO_32_LO(work_reg_32, work_reg_64);
    work_reg_32 &= 0x01;
    *deq_overflow_p = is_gtimer_enabled ? 0 : work_reg_32;
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(sram_deq_byte_ctr, work_reg_64);
    COMPILER_64_UMUL_32(sram_deq_byte_ctr, SAL_UINT8_NOF_BITS);
    COMPILER_64_UDIV_32(sram_deq_byte_ctr, 1000000);
    COMPILER_64_TO_32_LO(work_reg_32, sram_deq_byte_ctr);
    *deq_counter_in_mb_p = work_reg_32;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Set all VOQs to use one of the modes: dram only, sram only, both (normal mode).
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] dram_use_mode -
 *   dbal_enum_value_field_dram_use_mode_e. The mode to set for ALL VOQs.
 *   Either 
 * \return
 *   Error code (as per 'shr_error_e').
 * \remark
 *   This setup works on both cores (SBC)
 *   This is a debug aid and not intended to be used during normal operation.
 * \see
 *   shr_error_e
 *   dbal_enum_value_field_dram_use_mode_e
 *   INGRESS_CONG_VOQ_RATE_CLASS_DRAM_USE_MODE
 */
static shr_error_e
ctest_dnx_dram_use_mode_set(
    int unit,
    dbal_enum_value_field_dram_use_mode_e dram_use_mode)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_USE_MODE,
                                     1, 1,
                                     GEN_DBAL_RANGE32, DBAL_FIELD_VOQ_RATE_CLASS, (uint32) DBAL_RANGE_ALL,
                                     (uint32) DBAL_RANGE_ALL, GEN_DBAL_FIELD32, DBAL_FIELD_DRAM_USE_MODE, INST_SINGLE,
                                     (uint32) (dram_use_mode), GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function calculates the rate of inconing and outgoung traffic
 *   from specified port using SNMP counters. It also calculates the
 *   rate of enqueue and dequeue of bytes into and out of the dram.
 *
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] port -
 *   bcm_port_t. Local port for which calculation is carried out.
 * \param [in] inject_period_in_seconds -
 *   uint32. Number of seconds to wait between the collection of the two
 *   samples. The larger this time is, the more exact the result.
 * \param [out] tx_rate_in_mega_bits_per_sec_p -
 *   Pointer to uint32. This procedure loads pointed memory by the calculated
 *   tx rate, in mega bits per second.
 * \param [out] rx_rate_in_mega_bits_per_sec_p -
 *   Pointer to uint32. This procedure loads pointed memory by the calculated
 *   rx rate, in mega bits per second.
 * \param [out] dram_enq_rate_in_mega_bits_per_sec_p -
 *   Pointer to uint32. This procedure loads pointed memory by the calculated
 *   enqueue-into-dram rate, in mega bits per second.
 * \param [out] dram_deq_rate_in_mega_bits_per_sec_p -
 *   Pointer to uint32. This procedure loads pointed memory by the calculated
 *   dequeue-from-dram rate, in mega bits per second.
 * \param [out] sram_enq_rate_in_mega_bits_per_sec_p -
 *   Pointer to uint32. This procedure loads pointed memory by the calculated
 *   enqueue-into-sram rate, in mega bits per second.
 * \param [out] sram_deq_rate_in_mega_bits_per_sec_p -
 *   Pointer to uint32. This procedure loads pointed memory by the calculated
 *   dequeue-from-sram rate, in mega bits per second.
 * \param [out] dram_read_rate_in_mega_bits_per_sec_p -
 *   Pointer to uint32. This procedure loads pointed memory by the calculated
 *   read-from-dram rate, in mega bits per second.
 * \param [out] dram_write_rate_in_mega_bits_per_sec_p -
 *   Pointer to uint32. This procedure loads pointed memory by the calculated
 *   write-into-dram rate, in mega bits per second.
 * \return
 *   Error code (as per 'shr_error_e').
 * \remark
 *   This procedure assumes that some traffic is being injected into and out
 *   of indicated port. Otherwise, the output rate will be '0'.
 * \see
 *   shr_error_e
 *   ctest_dnx_dram_operation_buffer_test_cmd
 *   ctest_dnx_dram_operation_rate_test_cmd
 */
static shr_error_e
ctest_dnx_dram_rate_retrieve(
    int unit,
    bcm_port_t port,
    uint32 inject_period_in_seconds,
    uint32 *tx_rate_in_mega_bits_per_sec_p,
    uint32 *rx_rate_in_mega_bits_per_sec_p,
    uint32 *dram_enq_rate_in_mega_bits_per_sec_p,
    uint32 *dram_deq_rate_in_mega_bits_per_sec_p,
    uint32 *sram_enq_rate_in_mega_bits_per_sec_p,
    uint32 *sram_deq_rate_in_mega_bits_per_sec_p,
    uint32 *dram_read_rate_in_mega_bits_per_sec_p,
    uint32 *dram_write_rate_in_mega_bits_per_sec_p)
{
    COMPILER_UINT64 uint64_value_tx[2];
    COMPILER_UINT64 uint64_value_rx[2];
    int tdu_block_ids[BLOCK_MAX_INDEX];
    int cgm_block_ids[BLOCK_MAX_INDEX];
    int tdu_nof_block;
    int cgm_nof_block;
    uint32 inject_period_in_milli_seconds;
    int nof_cores;
    int core_id;
    uint32
        read_dram_counter_in_mb[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][2],
        write_dram_counter_in_mb[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][2];
    uint32 sum_read_dram_counter_in_mb, sum_write_dram_counter_in_mb;
    uint32
        enq_dram_counter_in_mb[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][2],
        deq_dram_counter_in_mb[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][2];
    uint32 sum_enq_dram_counter_in_mb, sum_deq_dram_counter_in_mb;
    uint32
        enq_sram_counter_in_mb[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][2],
        deq_sram_counter_in_mb[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][2];
    uint32 sum_enq_sram_counter_in_mb, sum_deq_sram_counter_in_mb;
    int read_overflow, write_overflow;
    int enq_overflow, deq_overflow;
    int tdu_gtimer_was_started = FALSE;
    int cgm_gtimer_was_started = FALSE;
    uint32 gtimer_period_in_sec;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * We set, hard coded, the time period for gtimer, to be 2 seconds.
     */
    gtimer_period_in_sec = 2;
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    inject_period_in_milli_seconds = inject_period_in_seconds * 1000;
    *tx_rate_in_mega_bits_per_sec_p = *rx_rate_in_mega_bits_per_sec_p = 0;
    *sram_enq_rate_in_mega_bits_per_sec_p = *sram_deq_rate_in_mega_bits_per_sec_p = 0;
    {
        char *tdu_match_n;

        *dram_read_rate_in_mega_bits_per_sec_p = *dram_write_rate_in_mega_bits_per_sec_p = 0;
        sum_read_dram_counter_in_mb = sum_write_dram_counter_in_mb = 0;
        tdu_match_n = "TDU";
        sal_memset(tdu_block_ids, 0, sizeof(tdu_block_ids));
        if (counter_block_name_match(unit, tdu_match_n, &tdu_nof_block, tdu_block_ids) != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "No matched block found for %s. Quit.\r\n", tdu_match_n);
        }
        /*
         * Clear sram read/write TDU/DRAM counters.
         */
        for (core_id = 0; core_id < nof_cores; core_id++)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_ddp_read_write_inform
                            (unit, core_id, &read_dram_counter_in_mb[0][0], &read_overflow,
                             &write_dram_counter_in_mb[0][0], &write_overflow));
        }
        for (core_id = 0; core_id < nof_cores; core_id++)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_ddp_read_write_inform
                            (unit, core_id, &read_dram_counter_in_mb[core_id][0], &read_overflow,
                             &write_dram_counter_in_mb[core_id][0], &write_overflow));
            if ((read_overflow != FALSE) || (write_overflow != FALSE))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "\r\n"
                             "Overflow in counters of read/write to/from dram (read_overflow %d/write_overflow %d). Quit.\r\n",
                             read_overflow, write_overflow);
            }
        }
        SHR_IF_ERR_EXIT(sh_dnxc_diag_block_gtimer_start(unit, tdu_nof_block, tdu_block_ids, gtimer_period_in_sec));
        tdu_gtimer_was_started = TRUE;
    }
    {
        char *cgm_match_n;

        *dram_enq_rate_in_mega_bits_per_sec_p = *dram_deq_rate_in_mega_bits_per_sec_p = 0;
        sum_enq_dram_counter_in_mb = sum_deq_dram_counter_in_mb = 0;
        *sram_enq_rate_in_mega_bits_per_sec_p = *sram_deq_rate_in_mega_bits_per_sec_p = 0;
        sum_enq_sram_counter_in_mb = sum_deq_sram_counter_in_mb = 0;
        cgm_match_n = "CGM";
        sal_memset(cgm_block_ids, 0, sizeof(cgm_block_ids));
        if (counter_block_name_match(unit, cgm_match_n, &cgm_nof_block, cgm_block_ids) != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "No matched block found for %s. Quit.\r\n", cgm_match_n);
        }
        /*
         * Clear dram enq/deq counters.
         * Clear sram enq/deq counters.
         */
        for (core_id = 0; core_id < nof_cores; core_id++)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_dram_enq_deq_inform
                            (unit, core_id, &enq_dram_counter_in_mb[0][0], &enq_overflow, &deq_dram_counter_in_mb[0][0],
                             &deq_overflow));
            SHR_IF_ERR_EXIT(ctest_dnx_sram_enq_deq_inform
                            (unit, core_id, &enq_sram_counter_in_mb[0][0], &enq_overflow, &deq_sram_counter_in_mb[0][0],
                             &deq_overflow));
        }
        for (core_id = 0; core_id < nof_cores; core_id++)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_dram_enq_deq_inform
                            (unit, core_id, &enq_dram_counter_in_mb[core_id][0], &enq_overflow,
                             &deq_dram_counter_in_mb[core_id][0], &deq_overflow));
            if ((enq_overflow != FALSE) || (deq_overflow != FALSE))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "\r\n"
                             "Overflow in counters of enq/deq to/from dram (enq_overflow %d/deq_overflow %d). Quit.\r\n",
                             enq_overflow, deq_overflow);
            }
            SHR_IF_ERR_EXIT(ctest_dnx_sram_enq_deq_inform
                            (unit, core_id, &enq_sram_counter_in_mb[core_id][0], &enq_overflow,
                             &deq_sram_counter_in_mb[core_id][0], &deq_overflow));
            if ((enq_overflow != FALSE) || (deq_overflow != FALSE))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "\r\n"
                             "Overflow in counters of enq/deq to/from sram (enq_overflow %d/deq_overflow %d). Quit.\r\n",
                             enq_overflow, deq_overflow);
            }
        }
        SHR_IF_ERR_EXIT(sh_dnxc_diag_block_gtimer_start(unit, cgm_nof_block, cgm_block_ids, gtimer_period_in_sec));
        cgm_gtimer_was_started = TRUE;
    }
    {
        bcm_stat_val_t stat_type;

        stat_type = snmpIfInOctets;
        SHR_IF_ERR_EXIT(bcm_dnx_stat_get(unit, port, stat_type, &uint64_value_tx[0]));
        stat_type = snmpIfOutOctets;
        SHR_IF_ERR_EXIT(bcm_dnx_stat_get(unit, port, stat_type, &uint64_value_rx[0]));
    }
    sal_msleep(inject_period_in_milli_seconds);
    {
        bcm_stat_val_t stat_type;

        stat_type = snmpIfInOctets;
        SHR_IF_ERR_EXIT(bcm_dnx_stat_get(unit, port, stat_type, &uint64_value_tx[1]));
        stat_type = snmpIfOutOctets;
        SHR_IF_ERR_EXIT(bcm_dnx_stat_get(unit, port, stat_type, &uint64_value_rx[1]));
    }
    /*
     * Just making sure all Gtimers have completed their cycle.
     */
    sal_msleep(gtimer_period_in_sec);
    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        /*
         * TDU counters (DRAM)
         */
        SHR_IF_ERR_EXIT(ctest_dnx_ddp_read_write_inform
                        (unit, core_id, &read_dram_counter_in_mb[core_id][1], &read_overflow,
                         &write_dram_counter_in_mb[core_id][1], &write_overflow));
        if ((read_overflow != FALSE) || (write_overflow != FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Overflow in counters of read/write to/from dram (read_overflow %d/write_overflow %d). Quit.\r\n",
                         read_overflow, write_overflow);
        }
        sum_read_dram_counter_in_mb += (read_dram_counter_in_mb[core_id][1] - read_dram_counter_in_mb[core_id][0]);
        sum_write_dram_counter_in_mb += (write_dram_counter_in_mb[core_id][1] - write_dram_counter_in_mb[core_id][0]);
        read_dram_counter_in_mb[core_id][0] = read_dram_counter_in_mb[core_id][1];
        write_dram_counter_in_mb[core_id][0] = write_dram_counter_in_mb[core_id][1];
        /*
         * CGM counters (DRAM)
         */
        SHR_IF_ERR_EXIT(ctest_dnx_dram_enq_deq_inform(unit, core_id, &enq_dram_counter_in_mb[core_id][1], &enq_overflow,
                                                      &deq_dram_counter_in_mb[core_id][1], &deq_overflow));
        if ((enq_overflow != FALSE) || (deq_overflow != FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Overflow in counters of enq/deq to/from dram (enq_overflow %d/deq_overflow %d). Quit.\r\n",
                         enq_overflow, deq_overflow);
        }
        sum_enq_dram_counter_in_mb += (enq_dram_counter_in_mb[core_id][1] - enq_dram_counter_in_mb[core_id][0]);
        sum_deq_dram_counter_in_mb += (deq_dram_counter_in_mb[core_id][1] - deq_dram_counter_in_mb[core_id][0]);
        enq_dram_counter_in_mb[core_id][0] = enq_dram_counter_in_mb[core_id][1];
        deq_dram_counter_in_mb[core_id][0] = deq_dram_counter_in_mb[core_id][1];
        /*
         * CGM counters (SRAM)
         */
        SHR_IF_ERR_EXIT(ctest_dnx_sram_enq_deq_inform(unit, core_id, &enq_sram_counter_in_mb[core_id][1], &enq_overflow,
                                                      &deq_sram_counter_in_mb[core_id][1], &deq_overflow));
        if ((enq_overflow != FALSE) || (deq_overflow != FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "\r\n"
                         "Overflow in counters of enq/deq to/from sram (enq_overflow %d/deq_overflow %d). Quit.\r\n",
                         enq_overflow, deq_overflow);
        }
        sum_enq_sram_counter_in_mb += (enq_sram_counter_in_mb[core_id][1] - enq_sram_counter_in_mb[core_id][0]);
        sum_deq_sram_counter_in_mb += (deq_sram_counter_in_mb[core_id][1] - deq_sram_counter_in_mb[core_id][0]);
        enq_sram_counter_in_mb[core_id][0] = enq_sram_counter_in_mb[core_id][1];
        deq_sram_counter_in_mb[core_id][0] = deq_sram_counter_in_mb[core_id][1];
    }
    /*
     * TX/RX rate, via port
     */
    COMPILER_64_SUB_64(uint64_value_tx[1], uint64_value_tx[0]);
    COMPILER_64_UMUL_32(uint64_value_tx[1], SAL_UINT8_NOF_BITS);
    COMPILER_64_UDIV_32(uint64_value_tx[1], inject_period_in_seconds);
    COMPILER_64_UDIV_32(uint64_value_tx[1], 1000000);
    *tx_rate_in_mega_bits_per_sec_p = COMPILER_64_LO(uint64_value_tx[1]);
    COMPILER_64_SUB_64(uint64_value_rx[1], uint64_value_rx[0]);
    COMPILER_64_UMUL_32(uint64_value_rx[1], SAL_UINT8_NOF_BITS);
    COMPILER_64_UDIV_32(uint64_value_rx[1], inject_period_in_seconds);
    COMPILER_64_UDIV_32(uint64_value_rx[1], 1000000);
    *rx_rate_in_mega_bits_per_sec_p = COMPILER_64_LO(uint64_value_rx[1]);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Port %d. Collected statistics: RX rate %u Mega bits per sec., TX rate %u Mega bits per sec. %s\r\n",
                 port, *rx_rate_in_mega_bits_per_sec_p, *tx_rate_in_mega_bits_per_sec_p, EMPTY);
    /*
     * DRAM read/write
     */
    *dram_read_rate_in_mega_bits_per_sec_p = sum_read_dram_counter_in_mb / gtimer_period_in_sec;
    *dram_write_rate_in_mega_bits_per_sec_p = sum_write_dram_counter_in_mb / gtimer_period_in_sec;
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Collected DDP  statistics: WR  rate %08u Mega bits per sec., Rd  rate %08u Mega bits per sec. %s%s\r\n",
                 *dram_write_rate_in_mega_bits_per_sec_p, *dram_read_rate_in_mega_bits_per_sec_p, EMPTY, EMPTY);
    /*
     * DRAM enq/deq
     */
    *dram_enq_rate_in_mega_bits_per_sec_p = sum_enq_dram_counter_in_mb / gtimer_period_in_sec;
    *dram_deq_rate_in_mega_bits_per_sec_p = sum_deq_dram_counter_in_mb / gtimer_period_in_sec;
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Collected dram statistics: DEQ rate %08u Mega bits per sec., ENQ rate %08u Mega bits per sec. %s%s\r\n",
                 *dram_deq_rate_in_mega_bits_per_sec_p, *dram_enq_rate_in_mega_bits_per_sec_p, EMPTY, EMPTY);
    /*
     * SRAM enq/deq
     */
    *sram_enq_rate_in_mega_bits_per_sec_p = sum_enq_sram_counter_in_mb / gtimer_period_in_sec;
    *sram_deq_rate_in_mega_bits_per_sec_p = sum_deq_sram_counter_in_mb / gtimer_period_in_sec;
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Collected sram statistics: DEQ rate %08u Mega bits per sec., ENQ rate %08u Mega bits per sec. %s%s\r\n",
                 *sram_deq_rate_in_mega_bits_per_sec_p, *sram_enq_rate_in_mega_bits_per_sec_p, EMPTY, EMPTY);
exit:
    if (tdu_gtimer_was_started)
    {
        SHR_IF_ERR_EXIT(sh_dnxc_diag_block_gtimer_stop(unit, tdu_nof_block, tdu_block_ids));
    }
    if (cgm_gtimer_was_started)
    {
        SHR_IF_ERR_EXIT(sh_dnxc_diag_block_gtimer_stop(unit, cgm_nof_block, cgm_block_ids));
    }
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function part of the chain of  basic 'dram operation' testing
 *   application.
 *   This function validate Dram buffer usage (just checking whether or not
 *   it is being used under varoius scenarios).
 *
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 *   sh_process_command
 */
static shr_error_e
ctest_dnx_dram_operation_buffer_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t supported_phys;
    uint32 maximal_potential_num_phys;
    uint32 phys_per_core;
    /*
     * This is the name of the family of chips for which ports are set but may just be
     * unit number. Appears on each line.
     */
    char collective_name[CTEST_DNX_SIZEOF_COLLECTIVE_NAME];
    /*
     * This is the command as put on the BCM prompt line.
     */
    char bcm_command_line[CTEST_DNX_SIZEOF_BCM_COMMAND_LINE];
    /*
     * This is the text describing all ports including injected port. Appears on each line.
     */
    char *text_ports;
    /*
     * This is the index of each physical port that is considered. For example, if there are
     * 96 ports then it will go from 0 to 95.-
     */
    int index_phys_port;
    /*
     * This is the number of active physical ports on this device.
     * Must be smaller than 'maximal_potential_num_phys'
     */
    int num_active_phys_ports;
    /*
     * This is the identifying numer of the 'ucode_port' and, also, the last number on the line.
     */
    int index_ucode_port;
    int nof_cores;
    int core_id;
    /*
     * Hard coded source tm ports for SAT.
     */
    bcm_port_t *from_port_p = NULL;
    /*
     * Array of ports incorporated in the 'snake' pattern.
     */
    bcm_port_t *to_port_p = NULL;
    /*
     * Array of cores corresponding, each, to a port on the 'snake' pattern.
     */
    int *to_port_core_p = NULL;
    int to_port_index;
    rhhandle_t packet_h = NULL;
    bcm_pbmp_t nif_ports_bitmap;
    bcm_port_t nif_port;

    SHR_FUNC_INIT_VARS(unit);
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    /*
     * Get command line parameters.
     */
    {
        /*
         * { 
         */
        /* *INDENT-OFF* */
        /*
         * This is the general scheme of this test (ctest tm dram exercise operation type=buffer). Each cycle is:
         * - Set injection rate to 11 G bits-per-sec
         * - Inject for some time to let everything stabilize.
         * - Get rate counters, wait, get rate counters -> caculate rate
         * - Verify DRAM was or was not used (as expected)
         * - Verify packets were moved from OCB to DRAM or not (as expected) 
         * - Verify packets were moved from DRAM to fabric or not (as expected) - start next cycle
         * 
         *             ^
         *             |                                                                                       
         *             |                                                                                       
         *             |                                                                                       
         *             |                                                                                       
         *      11 Gbps|----------------------                     -----------------------                     
         *             |                     |                     |                     |                     
         * Injection   |                     |                     |                     |                     
         *   rate      |                     |                     |                     |                     
         *             |                     |                     |                     |                     
         *             |                     |                     |                     |                     
         *             |                     |                     |                     |                     
         *             |                     |                     |                     |                     
         *             |                     |                     |                     |                     
         *       1 GBPS|                     -----------------------                     ---------------...... 
         *             |                                                                                       
         *             |                                                                                       
         *             |------------------------------------------------------------------------------------->  
         *                       |           |         |           |                                           
         *                       |           |         |           |                         Time -->              
         *              stabilize  Collect    Stabilize  Collect    ......                                     
         *                         rate                  rate                                                 
         *                         statistics            statistics                                            
         */
        /* *INDENT-ON* */
        /*
         * } 
         */
        static ctest_dnx_dram_operation_buffer_params_t operation_buffer_params;
        /*
         * Flag indicating whether default parameters have already been loaded into
         * operation_buffer_params.
         */
        static int buffer_params_were_loaded = FALSE;

        if (buffer_params_were_loaded == FALSE)
        {
            SHR_IF_ERR_EXIT(diag_dnx_dram_buffer_defaults_load(unit, &operation_buffer_params));
            buffer_params_were_loaded = TRUE;
        }
        SHR_ALLOC(from_port_p, sizeof(*from_port_p) * nof_cores, "DRAM testing", "%s%s%s", "from_port_p", EMPTY, EMPTY);
        from_port_p[0] = SOURCE_TM_PORT_CORE_0_ON_SAT;
        if (nof_cores > 1)
        {
            from_port_p[1] = SOURCE_TM_PORT_CORE_1_ON_SAT;
        }
        supported_phys = dnx_data_nif.phys.general_get(unit)->supported_phys;
        maximal_potential_num_phys = dnx_data_nif.phys.nof_phys_get(unit);
        SHR_ALLOC(to_port_p, sizeof(*to_port_p) * maximal_potential_num_phys, "DRAM testing", "%s%s%s", "to_port_p",
                  EMPTY, EMPTY);
        SHR_ALLOC(to_port_core_p, sizeof(*to_port_core_p) * maximal_potential_num_phys, "DRAM testing", "%s%s%s",
                  "to_port_core_p", EMPTY, EMPTY);
        phys_per_core = dnx_data_nif.phys.nof_phys_per_core_get(unit);
        sal_snprintf(collective_name, sizeof(collective_name) - 1, ".%d", unit);
        collective_name[sizeof(collective_name) - 1] = 0;
        text_ports = "XE";

        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &nif_ports_bitmap));
        /** remove all nif ports */
        BCM_PBMP_ITER(nif_ports_bitmap, nif_port)
        {
            snprintf(bcm_command_line, sizeof(bcm_command_line) - 1, "config delete ucode_port_%d.*", nif_port);
            LOG_CLI_EX("%s %s%s%s\r\n", bcm_command_line, EMPTY, EMPTY, EMPTY);
            SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, bcm_command_line),
                                "Failed on sh_process_command(), delete\r\n");
        }

        to_port_index = 0;
        /*
         * Issue 'config' cli commands. One per port. Example:
         * "config add ucode_port_1.BCM8869X=XE0:core_0.1"
         */
        /*
         * By default, use all available physical ports.
         * If the required number of ports is smaller than number of available physical ports, use
         * it.
         */
        if (operation_buffer_params.ctest_dnx_dram_num_port_in_snake < maximal_potential_num_phys)
        {
            maximal_potential_num_phys = operation_buffer_params.ctest_dnx_dram_num_port_in_snake;
        }
        for (index_phys_port = 0, index_ucode_port = 1; index_phys_port < maximal_potential_num_phys;
             index_phys_port++, index_ucode_port++)
        {
            if (BCM_PBMP_MEMBER(supported_phys, index_phys_port))
            {
                core_id = index_phys_port / phys_per_core;
                snprintf(bcm_command_line, sizeof(bcm_command_line) - 1,
                         "config add ucode_port_%d%s=%s%d:core_%d.%d",
                         index_ucode_port, collective_name, text_ports, index_phys_port, core_id, index_ucode_port);
                SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, bcm_command_line),
                                    "Failed on sh_process_command(), add\r\n");
                LOG_CLI_EX("%s %s%s%s\r\n", bcm_command_line, EMPTY, EMPTY, EMPTY);
                to_port_p[to_port_index] = index_ucode_port;
                to_port_core_p[to_port_index] = core_id;
                to_port_index++;
            }
        }
        num_active_phys_ports = to_port_index;
        /*
         * At this point, 'num_active_phys_ports' is the number of physical ports that have been
         * issues a 'config' command and that will be configured on the next 'reboot'.
         */
        LOG_CLI_EX("Number of active physical ports is %d. %s%s%s\r\n", num_active_phys_ports, EMPTY, EMPTY, EMPTY);
        snprintf(bcm_command_line, sizeof(bcm_command_line) - 1, "config add port_init_speed_xe%s=10000",
                 collective_name);
        SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, bcm_command_line),
                            "Failed on sh_process_command(), config add port\r\n");
        LOG_CLI_EX("%s %s%s%s\r\n", bcm_command_line, EMPTY, EMPTY, EMPTY);
        snprintf(bcm_command_line, sizeof(bcm_command_line) - 1, "config add appl_enable_l2%s=0", collective_name);
        SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, bcm_command_line),
                            "Failed on sh_process_command(), ext_ram_enabled_bitmap\r\n");
        LOG_CLI_EX("%s %s%s%s\r\n", bcm_command_line, EMPTY, EMPTY, EMPTY);
        /*
         * Run Deinit-Init DNX DATA - The SoC properties, above, are loaded upon init.
         */
        SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, "tr 141"), "failed on tr 141\r\n");
        {
            COMPILER_UINT64 packet_count, bytes_count;
            bcm_port_mapping_info_t mapping_info;
            bcm_port_interface_info_t interface_info;
            uint32 flags;
            bcm_gport_t gp;
            uint32 dummy_min_rate, dummy_flags;
            uint32 rate;
            int cycle_index;
            int core_under_test;
            bcm_port_t port_under_test;

            port_under_test = to_port_p[0];
            BCM_COSQ_GPORT_E2E_PORT_SET(gp, port_under_test);
            SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_get(unit, gp, 0, &dummy_min_rate, &rate, &dummy_flags));
            LOG_CLI_EX("Rate assigned to port %d: %d Kilo bits per sec %s%s\r\n", port_under_test, rate, EMPTY, EMPTY);
            core_under_test = to_port_core_p[0];
            LOG_CLI_EX("Core assigned to port %d: %d %s%s\r\n", port_under_test, core_under_test, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, from_port_p[core_under_test], port_under_test, 1));
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Force connect ports %d->%d on core %d %s\r\n", from_port_p[core_under_test], port_under_test,
                         core_under_test, EMPTY);
            /*
             * Config for 'snake' in order to activate all ports
             * we assume the number of active ports is larger than '1'
             */
            for (to_port_index = 0; to_port_index < (num_active_phys_ports - 1); to_port_index++)
            {
                bcm_port_loopback_set(unit, to_port_p[to_port_index], BCM_PORT_LOOPBACK_MAC);
                bcm_port_force_forward_set(unit, to_port_p[to_port_index], to_port_p[to_port_index + 1], 1);
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Force connect ports %d->%d %s%s\r\n",
                             to_port_p[to_port_index], to_port_p[to_port_index + 1], EMPTY, EMPTY);
            }
            /*
             * Carry out a few cycles of injection of traffic on various rates.
             */
            for (cycle_index = 0; cycle_index < operation_buffer_params.ctest_dnx_dram_operation_buffer_cycles;
                 cycle_index++)
            {
                uint32 uint64_array[SAL_UINT64_NOF_BITS / SAL_UINT32_NOF_BITS];
                char uint_as_string[MAX_UINT64_SIZE_AS_STRING];

                SHR_IF_ERR_EXIT(bcm_port_get
                                (unit, from_port_p[core_under_test], &flags, &interface_info, &mapping_info));
                SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
                SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
                SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                                (unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
                LOG_CLI_EX("%s%s%s%s\r\n", "==================== ", "========== ", "========== ", "========== ");
                uint64_array[1] = 0;
                uint64_array[0] = operation_buffer_params.sat_injection_rate_kbps[cycle_index] / 1000;
                SHR_IF_ERR_EXIT(format_value_to_str(uint64_array, SAL_UINT32_NOF_BITS, FALSE, uint_as_string));
                LOG_CLI_EX("Cycle %d. START. Injected SAT rate is %s Mega Bits Per Second. %s%s\r\n",
                           cycle_index, uint_as_string, EMPTY, EMPTY);
                /*
                 * Inject traffic of 10G/1G (per 'operation_buffer_params')
                 */
                SHR_IF_ERR_EXIT(diag_sand_packet_stream_create(unit, 0, packet_h, 1024, 0,
                                                               BCM_SAT_GTF_RATE_IN_BYTES,
                                                               operation_buffer_params.sat_injection_rate_kbps
                                                               [cycle_index],
                                                               operation_buffer_params.sat_burst_size[cycle_index],
                                                               operation_buffer_params.sat_granularity_size
                                                               [cycle_index]));
                SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, from_port_p[core_under_test], packet_h, 0));
                SHR_IF_ERR_EXIT(diag_sand_packet_stream_get_stats(unit, packet_h, &bytes_count, &packet_count));
                LOG_CLI_EX("Cycle %d. Wait %d seconds %s%s\r\n", cycle_index,
                           operation_buffer_params.delay_before_inspect_in_milli[cycle_index] / 1000, EMPTY, EMPTY);
                sal_msleep(operation_buffer_params.delay_before_inspect_in_milli[cycle_index] / 2);
                {
                    /*
                     * Just clear 'enq/deq' (sticky) counters upon starting a new cycle (under traffic).
                     * This erases the influence of a prvious cycle, if any.
                     */
                    int some_enq_deq_dram;
                    SHR_IF_ERR_EXIT(ctest_dnx_dram_enq_deq_sense(unit, core_under_test, &some_enq_deq_dram));
                }
                sal_msleep(operation_buffer_params.delay_before_inspect_in_milli[cycle_index] / 2);
                {
                    /*
                     * Get and display info related to DRAM activity and DRAM occupation (by looking at VOQ occupation)
                     */
                    int some_dram_was_used;
                    int some_enq_deq_dram;

                    SHR_IF_ERR_EXIT(ctest_dnx_dram_empty_sense(unit, core_under_test, &some_dram_was_used));
                    if (some_dram_was_used == TRUE)
                    {
                        if (operation_buffer_params.expect_dram_to_be_used[cycle_index] != TRUE)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL,
                                         "Cycle %d. core %d. DRAM has been used. This is not as expected. Quit.\r\n",
                                         cycle_index, core_under_test);
                        }
                        else
                        {
                            LOG_CLI_EX("Cycle %d. core %d. DRAM has been used. This is as expected. %s%s\r\n",
                                       cycle_index, core_under_test, EMPTY, EMPTY);
                        }
                    }
                    else
                    {
                        if (operation_buffer_params.expect_dram_to_be_used[cycle_index] == TRUE)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL,
                                         "\r\n"
                                         "Cycle %d. Core %d. DRAM has NOT been used. This is not as expected. Quit.\r\n",
                                         cycle_index, core_under_test);
                        }
                        else
                        {
                            LOG_CLI_EX("Cycle %d. Core %d. DRAM has NOT been used. This is as expected. %s%s\r\n",
                                       cycle_index, core_under_test, EMPTY, EMPTY);
                        }
                    }
                    /*
                     * Check DRAM enqueue/dequeue activity.
                     */
                    SHR_IF_ERR_EXIT(ctest_dnx_dram_enq_deq_sense(unit, core_under_test, &some_enq_deq_dram));
                    if (some_enq_deq_dram == TRUE)
                    {
                        if (operation_buffer_params.expect_enq_deq_dram[cycle_index] != TRUE)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL,
                                         "Cycle %d. Core %d.'enq_deq_dram' operations were detected. This is not as expected. Quit.\r\n",
                                         cycle_index, core_under_test);
                        }
                        else
                        {
                            LOG_CLI_EX
                                ("Cycle %d. Core %d. 'enq_deq_dram' operations were detected. This is as expected.%s%s\r\n",
                                 cycle_index, core_under_test, EMPTY, EMPTY);
                        }
                    }
                    else
                    {
                        if (operation_buffer_params.expect_enq_deq_dram[cycle_index] == TRUE)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL,
                                         "Cycle %d. Core %d.'enq_deq_dram' operations were NOT detected. This is not as expected. Quit.\r\n",
                                         cycle_index, core_under_test);
                        }
                        else
                        {
                            LOG_CLI_EX
                                ("Cycle %d. Core %d. 'enq_deq_dram' operations were NOT detected. This is as expected. %s%s\r\n",
                                 cycle_index, core_under_test, EMPTY, EMPTY);
                        }
                    }
                }
                {
                    /*
                     * Verify rate is as expected.
                     */
                    uint32 inject_period_in_seconds;
                    uint32 tx_rate_in_mega_bits_per_sec;
                    uint32 rx_rate_in_mega_bits_per_sec;
                    uint32 min_rate, max_rate;
                    uint32 dram_enq_rate_in_mega_bits_per_sec;
                    uint32 dram_deq_rate_in_mega_bits_per_sec;
                    uint32 sram_enq_rate_in_mega_bits_per_sec;
                    uint32 sram_deq_rate_in_mega_bits_per_sec;
                    uint32 dram_read_rate_in_mega_bits_per_sec;
                    uint32 dram_write_rate_in_mega_bits_per_sec;

                    inject_period_in_seconds = operation_buffer_params.num_sec_to_wait_for_rate[cycle_index];
                    LOG_CLI_EX("Cycle %d. port %d. Collecting statistics for %d seconds. %s\r\n",
                               cycle_index, port_under_test, inject_period_in_seconds, EMPTY);
                    SHR_IF_ERR_EXIT(ctest_dnx_dram_rate_retrieve
                                    (unit, port_under_test, inject_period_in_seconds, &tx_rate_in_mega_bits_per_sec,
                                     &rx_rate_in_mega_bits_per_sec,
                                     &dram_enq_rate_in_mega_bits_per_sec, &dram_deq_rate_in_mega_bits_per_sec,
                                     &sram_enq_rate_in_mega_bits_per_sec, &sram_deq_rate_in_mega_bits_per_sec,
                                     &dram_read_rate_in_mega_bits_per_sec, &dram_write_rate_in_mega_bits_per_sec));
                    min_rate =
                        (operation_buffer_params.expected_rate_in_mbps[cycle_index] *
                         (100 - operation_buffer_params.expected_rate_deviation_in_percents[cycle_index])) / 100;
                    max_rate =
                        (operation_buffer_params.expected_rate_in_mbps[cycle_index] *
                         (100 + operation_buffer_params.expected_rate_deviation_in_percents[cycle_index])) / 100;
                    if ((tx_rate_in_mega_bits_per_sec < min_rate) || (tx_rate_in_mega_bits_per_sec > max_rate))
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "\r\n"
                                     "==> Cycle %d. Measured TX rate (%d mega bits per sec) is out of range (%d,%d). Quit\r\n",
                                     cycle_index, tx_rate_in_mega_bits_per_sec, min_rate, max_rate);
                    }
                    else if ((rx_rate_in_mega_bits_per_sec < min_rate) || (rx_rate_in_mega_bits_per_sec > max_rate))
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "\r\n"
                                     "==> Cycle %d. Measured RX rate (%d mega bits per sec) is out of range (%d,%d). Quit\r\n",
                                     cycle_index, rx_rate_in_mega_bits_per_sec, min_rate, max_rate);
                    }
                    LOG_CLI_EX("Cycle %d. port %d. TX/RX rates are as expected %s%s\r\n", cycle_index, port_under_test,
                               EMPTY, EMPTY);

                }
                /*
                 * Stop traffic
                 */
                sal_msleep(1000);
                LOG_CLI_EX("Cycle %d. Stop traffic injection %s%s%s\r\n", cycle_index, EMPTY, EMPTY, EMPTY);
                SHR_CLI_EXIT_IF_ERR(diag_sand_packet_send(unit, 0, packet_h, SAND_PACKET_STOP), "");
                LOG_CLI_EX("Cycle %d ended SUCCESSFULLY. %s%s%s\r\n", cycle_index, EMPTY, EMPTY, EMPTY);
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "SAT generated {0x%08X,0x%08X} Packets which are equal to {0x%08X,0x%08X} Bytes\n",
                             COMPILER_64_HI(packet_count), COMPILER_64_LO(packet_count), COMPILER_64_HI(bytes_count),
                             COMPILER_64_LO(bytes_count));
                diag_sand_packet_free(unit, packet_h);
                /*
                 * Make sure 'packet_h', which is a pointer to allocated memory, is marked as 'free'
                 * so that the code below will not try to release it, again.
                 */
                packet_h = NULL;
                if (operation_buffer_params.verify_clear_at_end_of_cycle[cycle_index] != FALSE)
                {
                    /*
                     * Verify that all DRAM activity is halted.
                     */
                    int all_activity_is_halted;

                    SHR_IF_ERR_EXIT(ctest_dnx_dram_inactivity_verify(unit, core_under_test, &all_activity_is_halted));
                    if (all_activity_is_halted == FALSE)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "\r\n"
                                     "==> Cycle %d. Either DRAM 'Reject' counters or activity counters are not zero or\r\n"
                                     "==> DRAM queue size is not zero after waiting. Quit test. \r\n", cycle_index);
                    }
                }
            }
        }
        /*
         * If we get here, then test has been SUCCESSFUL.
         */
        LOG_CLI_EX("%s%s%s%s\r\n", "==================== ", "========== ", "========== ", "========== ");
        LOG_CLI_EX("Test type '%s' has been SUCCESSFUL!!! %s%s%s\r\n", "buffer", EMPTY, EMPTY, EMPTY);
    }
exit:
    /*
     * Set system to work with both sram and dram.
     */
    diag_sand_packet_free(unit, packet_h);
    SHR_FREE(from_port_p);
    SHR_FREE(to_port_core_p);
    SHR_FREE(to_port_p);
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Container of the static control structure 'operation_rate_params'
 *   used by 'operation rate' test.
 *
 * \param [in] unit -
 *   HW identifier of unit.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 *   ctest_dnx_dram_operation_rate_test_cmd
 */
static shr_error_e
get_ptr_to_operation_rate_params(
    int unit,
    ctest_dnx_dram_operation_rate_params_t ** operation_rate_params_p_p)
{
    static ctest_dnx_dram_operation_rate_params_t operation_rate_params;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(operation_rate_params_p_p, _SHR_E_PARAM, "operation_rate_params_p_p");
    *operation_rate_params_p_p = &operation_rate_params;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Display current contents of the control structure 'operation_rate_params'.
 *
 * \param [in] unit -
 *   HW identifier of unit.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 *   sh_process_command
 */
static shr_error_e
display_operation_rate_params(
    int unit)
{
    ctest_dnx_dram_operation_rate_params_t *operation_rate_params_p;
    int test_cycle;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(get_ptr_to_operation_rate_params(unit, &operation_rate_params_p));
    LOG_CLI_EX("operation_rate_params %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    LOG_CLI_EX("===================== %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    LOG_CLI_EX("Total number of cycles    : %d.%s%s%s\r\n",
               operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles, EMPTY, EMPTY, EMPTY);
    LOG_CLI_EX("Apply 'debug' severity    : %s %s%s%s\r\n",
               (operation_rate_params_p->ctest_dnx_apply_debug_severity ? "TRUE" : "FALSE"), EMPTY, EMPTY, EMPTY);
    LOG_CLI_EX("SAT injection rate (Mbps) : %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    for (test_cycle = 0; test_cycle < operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles; test_cycle++)
    {
        LOG_CLI_EX("  Cycle %d: Injection rate (Mbps) : %d %s%s\r\n",
                   test_cycle, operation_rate_params_p->sat_injection_rate_kbps[test_cycle] / 1000, EMPTY, EMPTY);
    }
    LOG_CLI_EX("SAT packet size           : %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    for (test_cycle = 0; test_cycle < operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles; test_cycle++)
    {
        LOG_CLI_EX("  Cycle %d: Packet size (Bytes)   : %d %s%s\r\n",
                   test_cycle, operation_rate_params_p->sat_packet_size_bytes[test_cycle], EMPTY, EMPTY);
    }
    LOG_CLI_EX("Expected port rate (Mbps) : %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    for (test_cycle = 0; test_cycle < operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles; test_cycle++)
    {
        LOG_CLI_EX("  Cycle %d: Port rate (Mbps)      : %d %s%s\r\n",
                   test_cycle, operation_rate_params_p->expected_rate_in_mbps[test_cycle], EMPTY, EMPTY);
    }
    LOG_CLI_EX("Work with DRAM only       : %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    for (test_cycle = 0; test_cycle < operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles; test_cycle++)
    {
        LOG_CLI_EX("  Cycle %d: Use DRAM only         : %s %s%s\r\n",
                   test_cycle,
                   (operation_rate_params_p->ctest_dnx_use_dram_only[test_cycle] ? "TRUE" : "FALSE"), EMPTY, EMPTY);
    }
    LOG_CLI_EX("Expect DRAM to be used    : %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    for (test_cycle = 0; test_cycle < operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles; test_cycle++)
    {
        LOG_CLI_EX("  Cycle %d: Expect DRAM to be used: %s %s%s\r\n",
                   test_cycle,
                   (operation_rate_params_p->expect_dram_to_be_used[test_cycle] ? "TRUE" : "FALSE"), EMPTY, EMPTY);
    }
    LOG_CLI_EX("Expect enq/deq DRAM       : %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    for (test_cycle = 0; test_cycle < operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles; test_cycle++)
    {
        LOG_CLI_EX("  Cycle %d: Expect enq/deq DRAM   : %s %s%s\r\n",
                   test_cycle,
                   (operation_rate_params_p->expect_enq_deq_dram[test_cycle] ? "TRUE" : "FALSE"), EMPTY, EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function analyzes command line parameters for an 'operation rate'
 *   command. If required, it updates the static/global 'operation_rate_params'
 *   structure and indicates to the caller whether or not to actually carry
 *   out the 'operation rate' test.
 *   If 'operation_rate_params' is updated then it is also fully displayed
 *   and caller is required to not carry out the test.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \param [out] carry_out_test_p -
 *   Pointer to int. This procedure loads pointed memory by an indication on
 *   whether test is to be carried out or caller is required to just quit.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 *   sh_process_command
 */
static shr_error_e
ctest_dnx_dram_operation_rate_analyze(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    int *carry_out_test_p)
{
    ctest_dnx_dram_operation_rate_params_t *operation_rate_params_p;
    int dram_only;
    int dram_only_is_present;
    int expect_dram_to_be_used;
    int expect_dram_to_be_used_is_present;
    int expect_enq_deq_dram;
    int expect_enq_deq_dram_is_present;
    int test_cycle;
    int test_cycle_is_present;
    int show_debug;
    int show_debug_is_present;
    int disp_params;
    int disp_params_is_present;
    uint32 sat_injection_rate;
    int sat_injection_rate_is_present;
    uint32 expected_port_rate;
    int expected_port_rate_is_present;
    int execute_test;

    SHR_FUNC_INIT_VARS(unit);
    execute_test = TRUE;
    SHR_IF_ERR_EXIT(get_ptr_to_operation_rate_params(unit, &operation_rate_params_p));
    /*
     * Get command line parameters.
     */
    SH_SAND_GET_BOOL(CTEST_DNX_DRAM_DISP_PARAMS, disp_params);
    SH_SAND_IS_PRESENT(CTEST_DNX_DRAM_DISP_PARAMS, disp_params_is_present);
    if (disp_params_is_present)
    {
        /*
         * If 'disp_params' has been specified as 'TRUE' then just display all
         * control parameters and do not carry out test.
         */
        if (disp_params != FALSE)
        {
            execute_test = FALSE;
        }
    }
    SH_SAND_GET_BOOL(CTEST_DNX_DRAM_SHOW_DEBUG, show_debug);
    SH_SAND_IS_PRESENT(CTEST_DNX_DRAM_SHOW_DEBUG, show_debug_is_present);
    if (show_debug_is_present)
    {
        /*
         * If 'disp_params' has been specified as 'TRUE' then change severity of this module
         * to show all 'debug' messages
         */
        if (show_debug != FALSE)
        {
            operation_rate_params_p->ctest_dnx_apply_debug_severity = TRUE;
        }
        else
        {
            operation_rate_params_p->ctest_dnx_apply_debug_severity = FALSE;
        }
        execute_test = FALSE;
    }
    SH_SAND_GET_BOOL(CTEST_DNX_DRAM_DRAM_ONLY, dram_only);
    SH_SAND_IS_PRESENT(CTEST_DNX_DRAM_DRAM_ONLY, dram_only_is_present);
    if (dram_only_is_present)
    {
        SH_SAND_GET_UINT32(CTEST_DNX_DRAM_TEST_CYCLE, test_cycle);
        SH_SAND_IS_PRESENT(CTEST_DNX_DRAM_TEST_CYCLE, test_cycle_is_present);
        if (test_cycle_is_present == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "==> When '%s' is used then '%s' must also be specified. Quit.\r\n",
                         CTEST_DNX_DRAM_DRAM_ONLY, CTEST_DNX_DRAM_TEST_CYCLE);
        }
        if (test_cycle >= operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "==> Specified cycle (%d) must be smaller than the total number of cycles (%d).\r\n"
                         "==> Consider changing the total number of cycles. Quit.\r\n",
                         test_cycle, operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles);
        }
        execute_test = FALSE;
        operation_rate_params_p->ctest_dnx_use_dram_only[test_cycle] = dram_only;
    }
    SH_SAND_GET_UINT32(CTEST_DNX_DRAM_EXPECTED_PORT_RATE, expected_port_rate);
    SH_SAND_IS_PRESENT(CTEST_DNX_DRAM_EXPECTED_PORT_RATE, expected_port_rate_is_present);
    if (expected_port_rate_is_present)
    {
        SH_SAND_GET_UINT32(CTEST_DNX_DRAM_TEST_CYCLE, test_cycle);
        SH_SAND_IS_PRESENT(CTEST_DNX_DRAM_TEST_CYCLE, test_cycle_is_present);
        if (test_cycle_is_present == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "==> When '%s' is used then '%s' must also be specified. Quit.\r\n",
                         CTEST_DNX_DRAM_EXPECTED_PORT_RATE, CTEST_DNX_DRAM_TEST_CYCLE);
        }
        if (test_cycle >= operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "==> Specified cycle (%d) must be smaller than the total number of cycles (%d).\r\n"
                         "==> Consider changing the total number of cycles. Quit.\r\n",
                         test_cycle, operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles);
        }
        execute_test = FALSE;
        operation_rate_params_p->expected_rate_in_mbps[test_cycle] = expected_port_rate;
    }
    SH_SAND_GET_UINT32(CTEST_DNX_DRAM_SAT_INJECTION_RATE, sat_injection_rate);
    SH_SAND_IS_PRESENT(CTEST_DNX_DRAM_SAT_INJECTION_RATE, sat_injection_rate_is_present);
    if (sat_injection_rate_is_present)
    {
        SH_SAND_GET_UINT32(CTEST_DNX_DRAM_TEST_CYCLE, test_cycle);
        SH_SAND_IS_PRESENT(CTEST_DNX_DRAM_TEST_CYCLE, test_cycle_is_present);
        if (test_cycle_is_present == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "==> When '%s' is used then '%s' must also be specified. Quit.\r\n",
                         CTEST_DNX_DRAM_SAT_INJECTION_RATE, CTEST_DNX_DRAM_TEST_CYCLE);
        }
        if (test_cycle >= operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "==> Specified cycle (%d) must be smaller than the total number of cycles (%d).\r\n"
                         "==> Consider changing the total number of cycles. Quit.\r\n",
                         test_cycle, operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles);
        }
        execute_test = FALSE;
        operation_rate_params_p->sat_injection_rate_kbps[test_cycle] = sat_injection_rate * 1000;
    }
    SH_SAND_GET_BOOL(CTEST_DNX_DRAM_EXPECT_ENQ_DEQ_DRAM, expect_enq_deq_dram);
    SH_SAND_IS_PRESENT(CTEST_DNX_DRAM_EXPECT_ENQ_DEQ_DRAM, expect_enq_deq_dram_is_present);
    if (expect_enq_deq_dram_is_present)
    {
        SH_SAND_GET_UINT32(CTEST_DNX_DRAM_TEST_CYCLE, test_cycle);
        SH_SAND_IS_PRESENT(CTEST_DNX_DRAM_TEST_CYCLE, test_cycle_is_present);
        if (test_cycle_is_present == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "==> When '%s' is used then '%s' must also be specified. Quit.\r\n",
                         CTEST_DNX_DRAM_EXPECT_ENQ_DEQ_DRAM, CTEST_DNX_DRAM_TEST_CYCLE);
        }
        if (test_cycle >= operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "==> Specified cycle (%d) must be smaller than the total number of cycles (%d).\r\n"
                         "==> Consider changing the total number of cycles. Quit.\r\n",
                         test_cycle, operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles);
        }
        execute_test = FALSE;
        operation_rate_params_p->expect_enq_deq_dram[test_cycle] = expect_enq_deq_dram;
    }
    SH_SAND_GET_BOOL(CTEST_DNX_DRAM_EXPECT_DRAM, expect_dram_to_be_used);
    SH_SAND_IS_PRESENT(CTEST_DNX_DRAM_EXPECT_DRAM, expect_dram_to_be_used_is_present);
    if (expect_dram_to_be_used_is_present)
    {
        SH_SAND_GET_UINT32(CTEST_DNX_DRAM_TEST_CYCLE, test_cycle);
        SH_SAND_IS_PRESENT(CTEST_DNX_DRAM_TEST_CYCLE, test_cycle_is_present);
        if (test_cycle_is_present == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "==> When '%s' is used then '%s' must also be specified. Quit.\r\n",
                         CTEST_DNX_DRAM_EXPECT_DRAM, CTEST_DNX_DRAM_TEST_CYCLE);
        }
        if (test_cycle >= operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "==> Specified cycle (%d) must be smaller than the total number of cycles (%d).\r\n"
                         "==> Consider changing the total number of cycles. Quit.\r\n",
                         test_cycle, operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles);
        }
        execute_test = FALSE;
        operation_rate_params_p->expect_dram_to_be_used[test_cycle] = expect_dram_to_be_used;
    }
    if (execute_test == FALSE)
    {
        /*
         * If this procedure was called to only update/display configuration parameters
         * then quit here. To execute the test itself, caller need to not add any
         * configuration parameter.
         */
        display_operation_rate_params(unit);
    }
    *carry_out_test_p = execute_test;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function part of the chain of  basic 'dram operation' testing
 *   application.
 *   This function validate Dram buffer usage (by checking enqueue/dequeue
 *   rate under varoius scenarios).
 *
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 *   sh_process_command
 */
static shr_error_e
ctest_dnx_dram_operation_rate_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t supported_phys;
    uint32 maximal_potential_num_phys;
    uint32 phys_per_core;
    /*
     * This is the name of the family of chips for which ports are set but may just be
     * unit number. Appears on each line.
     */
    char collective_name[CTEST_DNX_SIZEOF_COLLECTIVE_NAME];
    /*
     * This is the command as put on the BCM prompt line.
     */
    char bcm_command_line[CTEST_DNX_SIZEOF_BCM_COMMAND_LINE];
    /*
     * This is the text describing all ports including injected port. Appears on each line.
     */
    char *text_ports;
    /*
     * This is the index of each physical port that is considered. For example, if there are
     * 96 ports then it will go from 0 to 95.-
     */
    int index_phys_port;
    /*
     * This is the number of active physical ports on this device.
     * Must be smaller than 'maximal_potential_num_phys'
     */
    int num_active_phys_ports;
    /*
     * This is the identifying numer of the 'ucode_port' and, also, the last number on the line.
     */
    int index_ucode_port;
    int nof_cores;
    int core_id;
    /*
     * Hard coded source tm ports for SAT.
     */
    bcm_port_t *from_port_p = NULL;
    /*
     * Array of ports incorporated in the 'snake' pattern.
     */
    bcm_port_t *to_port_p = NULL;
    int execute_test;
    /*
     * Array of cores corresponding, each, to a port on the 'snake' pattern.
     */
    int *to_port_core_p = NULL;
    int to_port_index;
    rhhandle_t packet_h = NULL;
    ctest_dnx_dram_operation_rate_params_t *operation_rate_params_p;
    int num_phys_ports_in_one_combined_port;
    /**
     * This variables stores the severity of the dnx APPL DRAM module
     */
    bsl_severity_t original_severity_testdram;
    /*
     * Flag indicating whether default parameters have already been loaded into
     * operation_rate_params.
     */
    static int rate_params_were_loaded = FALSE;
    int index_combined_port;
    bcm_pbmp_t nif_ports_bitmap;
    bcm_port_t nif_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPLDNX_TESTDRAM, original_severity_testdram);
    SHR_IF_ERR_EXIT(get_ptr_to_operation_rate_params(unit, &operation_rate_params_p));
    if (rate_params_were_loaded == FALSE)
    {
        /*
         * On first entry (only), load default parameters.
         */
        SHR_IF_ERR_EXIT(diag_dnx_dram_rate_defaults_load(unit, operation_rate_params_p));
        rate_params_were_loaded = TRUE;
    }
    SHR_IF_ERR_EXIT(ctest_dnx_dram_operation_rate_analyze(unit, args, sand_control, &execute_test));
    if (execute_test == FALSE)
    {
        SHR_EXIT();
    }
    if (operation_rate_params_p->ctest_dnx_apply_debug_severity)
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPLDNX_TESTDRAM, bslSeverityDebug);
    }
    {
        nof_cores = dnx_data_device.general.nof_cores_get(unit);
        SHR_ALLOC(from_port_p, sizeof(*from_port_p) * nof_cores, "DRAM testing", "%s%s%s", "from_port_p", EMPTY, EMPTY);
        from_port_p[0] = SOURCE_TM_PORT_CORE_0_ON_SAT;
        if (nof_cores > 1)
        {
            from_port_p[1] = SOURCE_TM_PORT_CORE_1_ON_SAT;
        }
        /*
         * 'supported_phys' is the bitmap of supported physical ports.
         */
        supported_phys = dnx_data_nif.phys.general_get(unit)->supported_phys;
        /*
         * 'maximal_potential_num_phys' is the number of supported physical ports.
         */
        maximal_potential_num_phys = dnx_data_nif.phys.nof_phys_get(unit);
        if (soc_is(unit, Q2A_DEVICE))
        {
            /** Q2A have 2 BH, so 16 lanes can be configured for CCGE */
            operation_rate_params_p->ctest_dnx_dram_num_port_in_snake = 16;
            /** according current throughput, rate can be 54G */
            operation_rate_params_p->expected_rate_in_mbps[0] = 54000;
            operation_rate_params_p->expected_rate_deviation_in_percents[0] = 10;
            operation_rate_params_p->expected_dram_rate_in_mbps[0] = 220000;
            operation_rate_params_p->expected_dram_rate_deviation_in_percents[0] = 10;
        }
        else if (soc_is(unit, J2C_DEVICE))
        {
            /** J2C have 4 BHs, so 32 lanes can be configured for CCGE */
            operation_rate_params_p->ctest_dnx_dram_num_port_in_snake = 32;
            operation_rate_params_p->expected_rate_in_mbps[0] = 100000;
            operation_rate_params_p->expected_dram_rate_in_mbps[0] = 520000;
        }
        /*
         * 'to_port_p' is the array of all ports involved in the formed snake.
         * 'to_port_core_p' is the array of cores correspnding, each, to a port involved in the formed snake.
         */
        SHR_ALLOC(to_port_p, sizeof(*to_port_p) * maximal_potential_num_phys, "DRAM testing", "%s%s%s", "to_port_p",
                  EMPTY, EMPTY);
        SHR_ALLOC(to_port_core_p, sizeof(*to_port_core_p) * maximal_potential_num_phys, "DRAM testing", "%s%s%s",
                  "to_port_core_p", EMPTY, EMPTY);
        /*
         * 'phys_per_core' is the number of physical ports on each of the cores.
         */
        phys_per_core = dnx_data_nif.phys.nof_phys_per_core_get(unit);
        sal_snprintf(collective_name, sizeof(collective_name) - 1, ".%d", unit);
        collective_name[sizeof(collective_name) - 1] = 0;
        text_ports = operation_rate_params_p->ctest_dnx_text_ports;

        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &nif_ports_bitmap));
        /** remove all nif ports */
        BCM_PBMP_ITER(nif_ports_bitmap, nif_port)
        {
            snprintf(bcm_command_line, sizeof(bcm_command_line) - 1, "config delete ucode_port_%d.*", nif_port);
            LOG_CLI_EX("%s %s%s%s\r\n", bcm_command_line, EMPTY, EMPTY, EMPTY);
            SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, bcm_command_line),
                                "Failed on sh_process_command(), delete\r\n");
        }
        to_port_index = 0;
        /*
         * Issue 'config' cli commands. One per port. Example:
         * "config add ucode_port_1.0=CDGE0:core_0.1"
         */
        /*
         * By default, use all available physical ports.
         * If the required number of ports is smaller than number of available physical ports, use
         * it.
         */
        if (operation_rate_params_p->ctest_dnx_dram_num_port_in_snake < maximal_potential_num_phys)
        {
            maximal_potential_num_phys = operation_rate_params_p->ctest_dnx_dram_num_port_in_snake;
        }
        num_phys_ports_in_one_combined_port = operation_rate_params_p->ctest_dnx_num_phys_ports_in_one_combined_port;
        for (index_phys_port = 0, index_ucode_port = 1, index_combined_port = 0;
             index_phys_port < maximal_potential_num_phys; index_phys_port++)
        {
            if (BCM_PBMP_MEMBER(supported_phys, index_phys_port))
            {
                if ((index_phys_port % num_phys_ports_in_one_combined_port) == 0)
                {
                    core_id = index_phys_port / phys_per_core;
                    snprintf(bcm_command_line, sizeof(bcm_command_line) - 1,
                             "config add ucode_port_%d%s=%s%d:core_%d.%d",
                             index_ucode_port, collective_name, text_ports, index_combined_port, core_id,
                             index_ucode_port);
                    SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, bcm_command_line),
                                        "Failed on sh_process_command(), add\r\n");
                    LOG_CLI_EX("%s %s%s%s\r\n", bcm_command_line, EMPTY, EMPTY, EMPTY);
                    to_port_p[to_port_index] = index_ucode_port;
                    to_port_core_p[to_port_index] = core_id;
                    to_port_index++;
                    index_combined_port++;
                    index_ucode_port++;
                }
            }
        }
        num_active_phys_ports = to_port_index;
        /*
         * At this point, 'num_active_phys_ports' is the number of physical ports that have been
         * issues a 'config' command and that will be configured on the next 'reboot'.
         */
        LOG_CLI_EX("Number of active physical combined ports is %d. %s%s%s\r\n", num_active_phys_ports, EMPTY, EMPTY,
                   EMPTY);
        /*
         * Disable L2.
         */
        snprintf(bcm_command_line, sizeof(bcm_command_line) - 1, "config add appl_enable_l2%s=0", collective_name);
        SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, bcm_command_line), "Failed on appl_enable_l2(), add\r\n");
        LOG_CLI_EX("%s %s%s%s\r\n", bcm_command_line, EMPTY, EMPTY, EMPTY);
        /*
         * Run Deinit-Init DNX DATA - The SoC properties, above, are loaded upon init.
         */
        SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, "tr 141"), "failed on tr 141\r\n");
        {
            COMPILER_UINT64 packet_count, bytes_count;
            bcm_port_mapping_info_t mapping_info;
            bcm_port_interface_info_t interface_info;
            uint32 flags;
            bcm_gport_t gp;
            uint32 dummy_min_rate, dummy_flags;
            uint32 rate;
            int cycle_index;
            int core_under_test;
            bcm_port_t port_under_test;

            port_under_test = to_port_p[0];
            BCM_COSQ_GPORT_E2E_PORT_SET(gp, port_under_test);
            SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_get(unit, gp, 0, &dummy_min_rate, &rate, &dummy_flags));
            LOG_CLI_EX("Rate assigned to port %d: %d Kilo bits per sec %s%s\r\n", port_under_test, rate, EMPTY, EMPTY);
            core_under_test = to_port_core_p[0];
            LOG_CLI_EX("Core assigned to port %d: %d %s%s\r\n", port_under_test, core_under_test, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, from_port_p[core_under_test], port_under_test, 1));
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Force connect ports %d->%d on core %d %s\r\n", from_port_p[core_under_test], port_under_test,
                         core_under_test, EMPTY);
            /*
             * Config for 'snake' in order to activate all ports
             * we assume the number of active ports is larger than '1'
             */
            for (to_port_index = 0; to_port_index < (num_active_phys_ports - 1); to_port_index++)
            {
                bcm_port_loopback_set(unit, to_port_p[to_port_index], BCM_PORT_LOOPBACK_MAC);
                bcm_port_force_forward_set(unit, to_port_p[to_port_index], to_port_p[to_port_index + 1], 1);
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Force connect ports %d->%d %s%s\r\n",
                             to_port_p[to_port_index], to_port_p[to_port_index + 1], EMPTY, EMPTY);
            }
            /*
             * Carry out a few cycles of injection of traffic on various rates.
             */
            for (cycle_index = 0; cycle_index < operation_rate_params_p->ctest_dnx_dram_operation_rate_cycles;
                 cycle_index++)
            {
                uint32 uint64_array[SAL_UINT64_NOF_BITS / SAL_UINT32_NOF_BITS];
                char uint_as_string[MAX_UINT64_SIZE_AS_STRING];

                if (operation_rate_params_p->ctest_dnx_use_dram_only[cycle_index] != FALSE)
                {
                    /*
                     * Set system to work with dram only.
                     */
                    LOG_CLI_EX("Set system to work with dram only (No sram). %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
                    SHR_IF_ERR_EXIT(ctest_dnx_dram_use_mode_set(unit, DBAL_ENUM_FVAL_DRAM_USE_MODE_DRAM_ONLY));
                }
                else
                {
                    LOG_CLI_EX("Set system to work normally (Both sram and dram). %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY,
                               EMPTY);
                    SHR_IF_ERR_EXIT(ctest_dnx_dram_use_mode_set(unit, DBAL_ENUM_FVAL_DRAM_USE_MODE_NORMAL));
                }
                SHR_IF_ERR_EXIT(bcm_port_get
                                (unit, from_port_p[core_under_test], &flags, &interface_info, &mapping_info));
                SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
                SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
                SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                                (unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
                LOG_CLI_EX("%s%s%s%s\r\n", "==================== ", "========== ", "========== ", "========== ");
                uint64_array[1] = 0;
                uint64_array[0] = operation_rate_params_p->sat_injection_rate_kbps[cycle_index] / 1000;
                SHR_IF_ERR_EXIT(format_value_to_str(uint64_array, SAL_UINT32_NOF_BITS, FALSE, uint_as_string));
                LOG_CLI_EX
                    ("Cycle %d. START. Injected SAT rate is %s Mega Bits Per Second. SAT packet size is %d byte %s\r\n",
                     cycle_index, uint_as_string, operation_rate_params_p->sat_packet_size_bytes[cycle_index], EMPTY);
                /*
                 * Clear (and display)counters
                 */
                SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, "diag counter graphical coreDiSPlay"),
                                    "failed on diag counter\r\n");
                SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, "tm ingress queue non_empty"),
                                    "failed on tm ingress queue \r\n");
                /*
                 * Inject traffic of 400G (or other, as per 'operation_rate_params')
                 */
                SHR_IF_ERR_EXIT(diag_sand_packet_stream_create(unit, 0, packet_h,
                                                               operation_rate_params_p->sat_packet_size_bytes
                                                               [cycle_index], 0, BCM_SAT_GTF_RATE_IN_BYTES,
                                                               operation_rate_params_p->sat_injection_rate_kbps
                                                               [cycle_index],
                                                               operation_rate_params_p->sat_burst_size[cycle_index],
                                                               operation_rate_params_p->sat_granularity_size
                                                               [cycle_index]));
                SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, from_port_p[core_under_test], packet_h, 0));
                SHR_IF_ERR_EXIT(diag_sand_packet_stream_get_stats(unit, packet_h, &bytes_count, &packet_count));
                LOG_CLI_EX("Cycle %d. Wait %d seconds %s%s\r\n", cycle_index,
                           operation_rate_params_p->delay_before_inspect_in_milli[cycle_index] / 1000, EMPTY, EMPTY);
                sal_msleep(operation_rate_params_p->delay_before_inspect_in_milli[cycle_index] / 2);
                {
                    /*
                     * Just clear 'enq/deq' (sticky) counters upon starting a new cycle (under traffic).
                     * This erases the influence of a prvious cycle, if any.
                     */
                    int some_enq_deq_dram;
                    for (core_id = 0; core_id < nof_cores; core_id++)
                    {
                        SHR_IF_ERR_EXIT(ctest_dnx_dram_enq_deq_sense(unit, core_id, &some_enq_deq_dram));
                    }
                }
                sal_msleep(operation_rate_params_p->delay_before_inspect_in_milli[cycle_index] / 2);
                {
                    /*
                     * Get and display info related to DRAM activity and DRAM occupation (by looking at VOQ occupation)
                     */
                    int some_dram_was_used;
                    int some_enq_deq_dram;

                    SHR_IF_ERR_EXIT(ctest_dnx_dram_empty_sense(unit, core_under_test, &some_dram_was_used));
                    if (some_dram_was_used == TRUE)
                    {
                        if (operation_rate_params_p->expect_dram_to_be_used[cycle_index] != TRUE)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL,
                                         "\r\n"
                                         "==> Cycle %d. core %d. DRAM has been used. This is not as expected. Quit.\r\n",
                                         cycle_index, core_under_test);
                        }
                        else
                        {
                            LOG_CLI_EX("Cycle %d. core %d. DRAM has been used. This is as expected. %s%s\r\n",
                                       cycle_index, core_under_test, EMPTY, EMPTY);
                        }
                    }
                    else
                    {
                        if (operation_rate_params_p->expect_dram_to_be_used[cycle_index] == TRUE)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL,
                                         "\r\n"
                                         "==> Cycle %d. Core %d. DRAM has NOT been used. This is not as expected. Quit.\r\n",
                                         cycle_index, core_under_test);
                        }
                        else
                        {
                            LOG_CLI_EX("Cycle %d. Core %d. DRAM has NOT been used. This is as expected. %s%s\r\n",
                                       cycle_index, core_under_test, EMPTY, EMPTY);
                        }
                    }
                    /*
                     * Check DRAM enqueue/dequeue activity.
                     */
                    SHR_IF_ERR_EXIT(ctest_dnx_dram_enq_deq_sense(unit, core_under_test, &some_enq_deq_dram));
                    if (some_enq_deq_dram == TRUE)
                    {
                        if (operation_rate_params_p->expect_enq_deq_dram[cycle_index] != TRUE)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL,
                                         "\r\n"
                                         "==> Cycle %d. Core %d.'enq_deq_dram' operations were detected. This is not as expected. Quit.\r\n",
                                         cycle_index, core_under_test);
                        }
                        else
                        {
                            LOG_CLI_EX
                                ("Cycle %d. Core %d. 'enq_deq_dram' operations were detected. This is as expected.%s%s\r\n",
                                 cycle_index, core_under_test, EMPTY, EMPTY);
                        }
                    }
                    else
                    {
                        if (operation_rate_params_p->expect_enq_deq_dram[cycle_index] == TRUE)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL,
                                         "\r\n"
                                         "==> Cycle %d. Core %d.'enq_deq_dram' operations were NOT detected. This is not as expected. Quit.\r\n",
                                         cycle_index, core_under_test);
                        }
                        else
                        {
                            LOG_CLI_EX
                                ("Cycle %d. Core %d. 'enq_deq_dram' operations were NOT detected. This is as expected. %s%s\r\n",
                                 cycle_index, core_under_test, EMPTY, EMPTY);
                        }
                    }
                }
                {
                    /*
                     * Verify rates are as expected.
                     */
                    uint32 inject_period_in_seconds;
                    uint32 tx_rate_in_mega_bits_per_sec;
                    uint32 rx_rate_in_mega_bits_per_sec;
                    uint32 min_rate, max_rate;
                    uint32 dram_enq_rate_in_mega_bits_per_sec;
                    uint32 dram_deq_rate_in_mega_bits_per_sec;
                    uint32 sram_enq_rate_in_mega_bits_per_sec;
                    uint32 sram_deq_rate_in_mega_bits_per_sec;
                    uint32 dram_read_rate_in_mega_bits_per_sec;
                    uint32 dram_write_rate_in_mega_bits_per_sec;

                    inject_period_in_seconds = operation_rate_params_p->num_sec_to_wait_for_rate[cycle_index];
                    LOG_CLI_EX("Cycle %d. port %d. Collecting statistics for %d seconds. %s\r\n",
                               cycle_index, port_under_test, inject_period_in_seconds, EMPTY);
                    SHR_IF_ERR_EXIT(ctest_dnx_dram_rate_retrieve
                                    (unit, port_under_test, inject_period_in_seconds, &tx_rate_in_mega_bits_per_sec,
                                     &rx_rate_in_mega_bits_per_sec,
                                     &dram_enq_rate_in_mega_bits_per_sec, &dram_deq_rate_in_mega_bits_per_sec,
                                     &sram_enq_rate_in_mega_bits_per_sec, &sram_deq_rate_in_mega_bits_per_sec,
                                     &dram_read_rate_in_mega_bits_per_sec, &dram_write_rate_in_mega_bits_per_sec));

                    LOG_CLI_EX("tx_rate %dMbit, rx_rate %dMbit, dram_read_rate %dMbit, dram_write_rate %dMbit\r\n",
                               tx_rate_in_mega_bits_per_sec, rx_rate_in_mega_bits_per_sec,
                               dram_read_rate_in_mega_bits_per_sec, dram_write_rate_in_mega_bits_per_sec);

                    LOG_CLI_EX
                        ("dram_enq_rate %dMbit, dram_deq_rate %dMbit, sram_enq_rate %dMbit, sram_deq_rate %dMbit\r\n",
                         dram_enq_rate_in_mega_bits_per_sec, dram_deq_rate_in_mega_bits_per_sec,
                         sram_enq_rate_in_mega_bits_per_sec, sram_deq_rate_in_mega_bits_per_sec);

                    min_rate =
                        (operation_rate_params_p->expected_rate_in_mbps[cycle_index] *
                         (100 - operation_rate_params_p->expected_rate_deviation_in_percents[cycle_index])) / 100;
                    max_rate =
                        (operation_rate_params_p->expected_rate_in_mbps[cycle_index] *
                         (100 + operation_rate_params_p->expected_rate_deviation_in_percents[cycle_index])) / 100;
                    if ((tx_rate_in_mega_bits_per_sec < min_rate) || (tx_rate_in_mega_bits_per_sec > max_rate))
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "\r\n"
                                     "==> Cycle %d. Measured TX rate (%d mega bits per sec) is out of range (%d,%d). Quit\r\n",
                                     cycle_index, tx_rate_in_mega_bits_per_sec, min_rate, max_rate);
                    }
                    else if ((rx_rate_in_mega_bits_per_sec < min_rate) || (rx_rate_in_mega_bits_per_sec > max_rate))
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "\r\n"
                                     "==> Cycle %d. Measured RX rate (%d mega bits per sec) is out of range (%d,%d). Quit\r\n",
                                     cycle_index, rx_rate_in_mega_bits_per_sec, min_rate, max_rate);
                    }
                    LOG_CLI_EX("Cycle %d. port %d. TX/RX rates are as expected %s%s\r\n", cycle_index, port_under_test,
                               EMPTY, EMPTY);
                    min_rate =
                        (operation_rate_params_p->expected_dram_rate_in_mbps[cycle_index] *
                         (100 - operation_rate_params_p->expected_dram_rate_deviation_in_percents[cycle_index])) / 100;
                    max_rate =
                        (operation_rate_params_p->expected_dram_rate_in_mbps[cycle_index] *
                         (100 + operation_rate_params_p->expected_dram_rate_deviation_in_percents[cycle_index])) / 100;
                    if ((dram_read_rate_in_mega_bits_per_sec < min_rate)
                        || (dram_read_rate_in_mega_bits_per_sec > max_rate))
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "\r\n"
                                     "==> Cycle %d. Measured DRAM READ rate (%d mega bits per sec) is out of range (%d,%d). Quit\r\n",
                                     cycle_index, dram_read_rate_in_mega_bits_per_sec, min_rate, max_rate);
                    }
                    else if ((dram_write_rate_in_mega_bits_per_sec < min_rate)
                             || (dram_write_rate_in_mega_bits_per_sec > max_rate))
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "\r\n"
                                     "==> Cycle %d. Measured DRAM WRITE rate (%d mega bits per sec) is out of range (%d,%d). Quit\r\n",
                                     cycle_index, dram_write_rate_in_mega_bits_per_sec, min_rate, max_rate);
                    }
                    LOG_CLI_EX("Cycle %d. DRAM READ/WRITE rates are as expected %s%s%s\r\n",
                               cycle_index, EMPTY, EMPTY, EMPTY);

                }
                /*
                 * Stop traffic
                 */
                sal_msleep(1000);
                LOG_CLI_EX("Cycle %d. Stop traffic injection %s%s%s\r\n", cycle_index, EMPTY, EMPTY, EMPTY);
                SHR_CLI_EXIT_IF_ERR(diag_sand_packet_send(unit, 0, packet_h, SAND_PACKET_STOP), "");
                LOG_CLI_EX("Cycle %d ended SUCCESSFULLY. %s%s%s\r\n", cycle_index, EMPTY, EMPTY, EMPTY);
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "SAT generated {0x%08X,0x%08X} Packets which are equal to {0x%08X,0x%08X} Bytes\n",
                             COMPILER_64_HI(packet_count), COMPILER_64_LO(packet_count), COMPILER_64_HI(bytes_count),
                             COMPILER_64_LO(bytes_count));
                diag_sand_packet_free(unit, packet_h);
                /*
                 * Make sure 'packet_h', which is a pointer to allocated memory, is marked as 'free'
                 * so that the code below will not try to release it, again.
                 */
                packet_h = NULL;
                if (operation_rate_params_p->verify_clear_at_end_of_cycle[cycle_index] != FALSE)
                {
                    /*
                     * Verify that all DRAM activity is halted.
                     */
                    int all_activity_is_halted;

                    SHR_IF_ERR_EXIT(ctest_dnx_dram_inactivity_verify(unit, core_under_test, &all_activity_is_halted));
                    if (all_activity_is_halted == FALSE)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "\r\n"
                                     "==> Cycle %d. Either DRAM 'Reject' counters or activity counters are not zero or\r\n"
                                     "==> DRAM queue size is not zero after waiting. Quit test. \r\n", cycle_index);
                    }
                }
            }
        }
        /*
         * If we get here, then test has been SUCCESSFUL.
         */
        LOG_CLI_EX("%s%s%s%s\r\n", "==================== ", "========== ", "========== ", "========== ");
        LOG_CLI_EX("Test type '%s' has been SUCCESSFUL!!! %s%s%s\r\n", "rate", EMPTY, EMPTY, EMPTY);
    }
exit:
    /*
     *  Restore the original severity at the end of this test.
     */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPLDNX_TESTDRAM, original_severity_testdram);
    /*
     * Set system to work with both sram and dram.
     */
    ctest_dnx_dram_use_mode_set(unit, DBAL_ENUM_FVAL_DRAM_USE_MODE_NORMAL);
    diag_sand_packet_free(unit, packet_h);
    SHR_FREE(from_port_p);
    SHR_FREE(to_port_core_p);
    SHR_FREE(to_port_p);
    SHR_FUNC_EXIT;
}

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
    int stub_idx;
    int test_duration_sec, temp_mode;
    int stub_size = 0;
    uint32 temp_stub[DNX_DRAM_TEST_STUB_ARR_LEN];
    uint32 temperature = 0;
    int fill_temp_stub;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("duration", test_duration_sec);
    SH_SAND_GET_INT32("mode", temp_mode);

    LOG_CLI_EX("Test dram temperature, using stub: started %s%s%s%s \r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    stub_size = test_duration_sec * SECOND_USEC / dnx_data_dram.hbm.usec_between_temp_samples_get(unit);
    if (stub_size > DNX_DRAM_TEST_STUB_ARR_LEN)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Test duration (%d sec) is too long. Max allowed=%d \n", test_duration_sec,
                     (DNX_DRAM_TEST_STUB_ARR_LEN * dnx_data_dram.hbm.usec_between_temp_samples_get(unit) /
                      SECOND_USEC));
    }
    /*
     * Just fill up with initial values. Good practice.
     */
    temperature = dnx_data_dram.hbm.power_down_temp_threshold_get(unit) + 1;
    for (stub_idx = 0; stub_idx < (sizeof(temp_stub) / sizeof(temp_stub[0])); stub_idx++)
    {
        temp_stub[stub_idx] = temperature;
    }
    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_low_temperature_dram_protction))
    {
        if (temp_mode != CTEST_DNX_DRAM_STUB_MODE_NORMAL && (temp_mode != CTEST_DNX_DRAM_STUB_MODE_HIGH) &&
            (temp_mode != CTEST_DNX_DRAM_STUB_MODE_LOW) &&
            (temp_mode != CTEST_DNX_DRAM_STUB_MODE_PWR_DOWN_ON_HIGH) &&
            (temp_mode != CTEST_DNX_DRAM_STUB_MODE_PWR_DOWN_ON_LOW))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid mode(=%d) \n", temp_mode);
        }
    }
    else
    {
        if (temp_mode != CTEST_DNX_DRAM_STUB_MODE_NORMAL && (temp_mode != CTEST_DNX_DRAM_STUB_MODE_HIGH) &&
            (temp_mode != CTEST_DNX_DRAM_STUB_MODE_PWR_DOWN_ON_HIGH))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid mode(=%d). Note that 'low_temperature_dram_protction'\r\n"
                         "feature is NOT enabled so that CTEST_DNX_DRAM_STUB_MODE_PWR_DOWN_ON_LOW (%d) and \r\n"
                         "CTEST_DNX_DRAM_STUB_MODE_LOW (%d) are not valid.\r\n\n",
                         temp_mode, CTEST_DNX_DRAM_STUB_MODE_PWR_DOWN_ON_LOW, CTEST_DNX_DRAM_STUB_MODE_LOW);
        }
    }
    /*
     * By default, fill the simulated-temperatures-table by 'temperature', as
     * set below. Do not do that for 'power down' case.
     */
    fill_temp_stub = 1;
    if (temp_mode == CTEST_DNX_DRAM_STUB_MODE_NORMAL)
    {
        /** set normal temp - traffic goes to DRAM */
        temperature = dnx_data_dram.hbm.restore_traffic_temp_threshold_get(unit) - 1;
    }
    else if (temp_mode == CTEST_DNX_DRAM_STUB_MODE_HIGH)
    {
        /** set high temp - traffic goes to OCB */
        temperature = dnx_data_dram.hbm.stop_traffic_temp_threshold_get(unit) + 1;
    }
    else if (temp_mode == CTEST_DNX_DRAM_STUB_MODE_PWR_DOWN_ON_HIGH)
    {
        /** set one stub for getting to shut down */
        temp_stub[0] = dnx_data_dram.hbm.power_down_temp_threshold_get(unit) + 1;
        stub_size = 1;
        fill_temp_stub = 0;
    }
    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_low_temperature_dram_protction))
    {
        /*
         * Low temperature protection is only there if the corresponding feature is valid.
         */
        if (temp_mode == CTEST_DNX_DRAM_STUB_MODE_LOW)
        {
            /** set low temp - traffic goes to OCB */
            temperature = dnx_data_dram.hbm.stop_traffic_low_temp_threshold_get(unit) - 1;
        }
        else if (temp_mode == CTEST_DNX_DRAM_STUB_MODE_PWR_DOWN_ON_LOW)
        {
            /** set one stub for getting to shut down */
            temp_stub[0] = dnx_data_dram.hbm.power_down_low_temp_threshold_get(unit);
            stub_size = 1;
            fill_temp_stub = 0;
        }
    }
    if (fill_temp_stub)
    {
        for (stub_idx = 0; stub_idx < stub_size; stub_idx++)
        {
            temp_stub[stub_idx] = temperature;
        }
    }
    SHR_IF_ERR_EXIT(dnx_hbmc_temp_stub_set(unit, stub_size, temp_stub));

exit:
    if (SHR_FUNC_ERR())
    {
        LOG_CLI_EX("Dram temperature, using stub. Test FAIL. Error: '%s' %s%s%s\r\n",
                   _SHR_ERRMSG(SHR_GET_CURRENT_ERR()), EMPTY, EMPTY, EMPTY);
    }
    else
    {
        LOG_CLI_EX("Dram temperature, using stub. Test PASS %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }
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
        dram_bound_recovery_threshold.value =
            dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES)->max;
        /** set configuration */
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, rate_class_gport, 0, &dram_bound_recovery_threshold));
    }
    LOG_CLI((BSL_META("Test dram bound rate class ended \n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is is used to filter out of regression tests which
 *   are only meant for Jr2 revision B1.
 *   If unit is NOT 'jr2_b1' then an error _SHR_E_UNIT is returned.
 *
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] test_list -
 *   Ignored by this procedre.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 *   sh_process_command
 */
static shr_error_e
ctest_dnx_dram_is_available(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (soc_is(unit, JERICHO2_A0_DEVICE))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }
    /**
     * if init is not done, return that dram is available.
     * this is done to prevent access to DNX-Data before it is init
     */
    if (!dnx_init_is_init_done_get(unit))
    {
        SHR_EXIT();
    }
    if ((dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap == 0) || (SAL_BOOT_PLISIM))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT;
}


/* *INDENT-OFF* */

static sh_sand_man_t ctest_dnx_dram_operation_test_man = {
    .brief =
        "Test DRAM operation under various traffic loads.",
    .full =
        "Buffer:\r\n"
            "Inject traffic such that DRAM will be used (as well as OCB) and see that it fills up\r\n"
            "and empties properly. Also test redirection of over-subscribed dram and recovery.\r\n"
        "Rate:\r\n"
            "Inject high capacity traffic and inspect rate of bytes into and out-of DRAM.\r\n"
            "See that it corresponds with high capacity rate.\r\n",
    .synopsis =
        "",
    .examples =
        "",
};

static sh_sand_man_t ctest_dnx_dram_operation_test_buffer_man = {
    .brief =
        "Test DRAM very usage (yes or no) under various traffic loads.",
    .full =
        "Inject traffic such that DRAM will be used (as well as OCB) and see that it fills up\r\n"
        "and empties properly. Also test redirection of over-subscribed dram and recovery.\r\n",
    .synopsis =
        "",
    .examples =
        "",
};

static sh_sand_man_t ctest_dnx_dram_operation_test_rate_man = {
    .brief =
        "Test DRAM usage (via enqueue/dequeue rate) under various traffic loads.",
    .full =
        "Inject high capacity traffic and inspect rate of bytes into and out-of DRAM.\r\n"
        "See that it corresponds with high capacity rate.\r\n"
        "If any parameter is added following 'rate' then this parameter is changed and "
        "control returns to the user to decide on whether to carry out the test (with"
        "updated parameter) or not.\r\n"
        "dram_only - \r\n"
        "  Boolean flag. Use DRAM only (TRUE) or let SRAM join as well (False).\r\n"
        "expect_dram - \r\n"
        "  Boolean flag. Expect DRAM to be used during the test (TRUE) or not (False).\r\n"
        "  If specified on command line then 'test_cycle' must also be specified.\r\n"
        "test_cycle - \r\n"
        "  Value of test cycle on which a parameter is to be changed. Not all\r\n"
        "  parameters require 'test_cycle'. Some do.\r\n"
        "disp_params - \r\n"
        "  Boolean flag. Display all control parameters (TRUE) or not (False).\r\n"
        "  Note that all parameters are displayed anyway if any other parameter\r\n"
        "  is specified.\r\n"
        "show_debug - \r\n"
        "  Boolean flag. Display all in-code debug-level messages (TRUE) or not (False).\r\n"
        "  Messages of severity levels higher than 'debug' are also shown..\r\n"
        "  At the end of the test, severity level is returned to its original value.\r\n"
        "expect_enq_deq_dram - \r\n"
        "  Boolean flag. Expect enq/deq to/from DRAM (TRUE) or not (False).\r\n"
        "  If specified on command line then 'test_cycle' must also be specified.\r\n"
        "sat_injection_rate - \r\n"
        "  Uint32. SAT injection rate, in mega bits per sec.\r\n"
        "  If specified on command line then 'test_cycle' must also be specified.\r\n"
        "expected_port_rate - \r\n"
        "  Uint32. expected rx/tx rate, on first snake port, in mega bits per sec.\r\n"
        "  If specified on command line then 'test_cycle' must also be specified.\r\n",
    .synopsis =
        "dram_only = <TRUE | FALE> expect_dram = <TRUE | FALE> disp_params = <TRUE | FALE> expect_enq_deq_dram = <TRUE | FALE>"
        " test_cycle = <Cycle to iupdate, out of total test cycles to be carried out> show_debug = <TRUE | FALE>"
        " sat_injection_rate = <Injection rate, in mega bits per sec., to create using SAT>"
        " expected_port_rate = <RX/TX rate, in mega bits per sec., on first snake port>",
    .examples =
        ""
        "dram_only = FALSE",
};
/**
 * \brief
 *   Options list for 'operation rate' shell command
 * \remark
 *   None
 * \see
 *   ctest_dnx_dram_operation_rate_test_cmd()
 */
static sh_sand_option_t ctest_dnx_dram_operation_rate_test_options[] = {
     /* Name */                   /* Type */             /* Description */                                                       /* Default */   /* 'ENUM' table */
    {CTEST_DNX_DRAM_DRAM_ONLY,   SAL_FIELD_TYPE_BOOL,  "Control on whether to use DRAM only (TRUE) or not (FALSE)",                "TRUE",          (void *)NULL},
    {CTEST_DNX_DRAM_EXPECT_DRAM, SAL_FIELD_TYPE_BOOL,  "Control on whether DRAM is expected to be used (TRUE) or not (FALSE)",     "TRUE",          (void *)NULL},
    {CTEST_DNX_DRAM_EXPECT_ENQ_DEQ_DRAM,
                                 SAL_FIELD_TYPE_BOOL,  "Control on whether enq/deq DRAM is expected (TRUE) or not (FALSE)",        "TRUE",          (void *)NULL},
    {CTEST_DNX_DRAM_SAT_INJECTION_RATE,
                                 SAL_FIELD_TYPE_UINT32,"Specify injection rate in mega bits per sec, using SAT",                   "100000",        (void *)NULL},
    {CTEST_DNX_DRAM_EXPECTED_PORT_RATE,
                                 SAL_FIELD_TYPE_UINT32,"Specify expected rate, on first snake port, in mega bits per sec.",        "100000",        (void *)NULL},
    {CTEST_DNX_DRAM_TEST_CYCLE,  SAL_FIELD_TYPE_UINT32,"Specify the pointed test cycle. Required by some of the CLI parameters",   "0",             (void *)NULL},
    {CTEST_DNX_DRAM_DISP_PARAMS, SAL_FIELD_TYPE_BOOL,  "Indicate whether to display all control parameters (TRUE) or not (FALSE)", "FALSE",         (void *)NULL},
    {CTEST_DNX_DRAM_SHOW_DEBUG,  SAL_FIELD_TYPE_BOOL,  "Indicate whether to display all in-code debug-level messages",             "FALSE",         (void *)NULL},
    {NULL}
};
/**
 * \brief
 *   List of tests for 'operation buffer' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
static sh_sand_invoke_t ctest_dnx_dram_operation_buffer_tests[] = {
    {"dram_buffer_1", "",  CTEST_PRECOMMIT},
    {NULL}
};
/**
 * \brief
 *   List of tests for 'operation rate' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
static sh_sand_invoke_t ctest_dnx_dram_operation_rate_tests[] = {
    {"dram_rate_1",   "",    CTEST_PRECOMMIT},
    {NULL}
};

static sh_sand_man_t diag_dnx_dram_stub_test_man = {
    .brief =
        "Test hbmc temperature monitor using A stub.",
    .full =
        "Driver refer to the stub (array of temperatue values) instead of the real temperature and\r\n"
        " operate accordigly. once all values from stub were read, driver return to normal operation",
    .synopsis =
        NULL,
    .examples =
        "ctest tm dram stub duration=1 mode=0",
};

static sh_sand_option_t diag_dnx_dram_stub_test_options[] = {
    {"duration", SAL_FIELD_TYPE_INT32, "test duration in seconds", "0"},
    {"mode",     SAL_FIELD_TYPE_INT32,
         "temp stub mode: 0-normal, 1-high_temp, 2-low_temp, 3-power_down_on_high_temp, 4-power_down_on_low_temp."
         " Options 2 and 4 are NOT legal if the feature 'low_temperature_dram_protction' is not enabled.", "0"},
    {NULL}
};

static sh_sand_invoke_t diag_dnx_dram_stub_tests[] = {
    {"duration_0", "duration=0 mode=0"},
    {NULL}
};

static sh_sand_man_t diag_dnx_dram_bound_rc_test_man = {
    "Test set (or delete) dram bound rate class for a given port. option to set dram bound\r\n"
    " threshold on specific resource type",
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
    {"port",       SAL_FIELD_TYPE_INT32, "test duration in seconds", "0"},
    {"enable",     SAL_FIELD_TYPE_BOOL,  "TRUE for dram bound rate class, FALSE for default rate class", "0"},
    {"rate_class", SAL_FIELD_TYPE_INT32, "rate class profile id to use", "30"},
    {"resource",   SAL_FIELD_TYPE_ENUM,  "resource_type to set DRAM bound threshold for","bcmResourceOcbPacketDescriptors", (void *) diag_dnx_dram_bound_resource_enum_table},
    {NULL}
};

static sh_sand_invoke_t diag_dnx_dram_bound_rc_tests[] = {
    {"bound_on",  "port=13 enable=1"},
    {"bound_off", "port=13 enable=0"},
    {"bound_on_res_type_bytes",   "port=14 enable=1 rate_class=5 resource=bcmResourceOcbBytes"},
    {"bound_on_res_type_buffers", "port=15 enable=1 rate_class=46 resource=bcmResourceOcbBuffers"},
    {NULL}
};

static sh_sand_man_t ctest_dnx_dram_exercises_test_man = {
    "Carry out various tests on DRAM operation under traffic.",
    NULL,
    NULL,
    NULL,
};



static sh_sand_cmd_t ctest_dnx_dram_exercises_test_operation[] = {
    /*
     * This branch ("operation") is intended for Jericho2, B1 only.
     */
    {"buffer", ctest_dnx_dram_operation_buffer_test_cmd, NULL, NULL, &ctest_dnx_dram_operation_test_buffer_man, NULL, ctest_dnx_dram_operation_buffer_tests},
    {"rate", ctest_dnx_dram_operation_rate_test_cmd, NULL, ctest_dnx_dram_operation_rate_test_options, &ctest_dnx_dram_operation_test_rate_man, NULL, ctest_dnx_dram_operation_rate_tests},
    {NULL}
};

static sh_sand_cmd_t sh_dnx_tm_dram_test_exercises[] = {
    /*
     * This branch ("operation") is intended for Jericho2, B1 only.
     */
    {"operation", NULL, ctest_dnx_dram_exercises_test_operation, NULL,
     &ctest_dnx_dram_operation_test_man, NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, ctest_dnx_dram_is_available},
    {NULL}
};

sh_sand_cmd_t sh_dnx_tm_dram_test_cmds[] = {
    {"exercise",   NULL, sh_dnx_tm_dram_test_exercises, NULL,                                 &ctest_dnx_dram_exercises_test_man,},
    {"stub",       diag_dnx_dram_stub_test_cmd,     NULL, diag_dnx_dram_stub_test_options,    &diag_dnx_dram_stub_test_man, NULL,diag_dnx_dram_stub_tests, SH_CMD_CONDITIONAL, ctest_dnx_dram_hbm_available},
    {"Rate_Class", diag_dnx_dram_bound_rc_test_cmd, NULL, diag_dnx_dram_bound_rc_test_options,&diag_dnx_dram_bound_rc_test_man, NULL,diag_dnx_dram_bound_rc_tests, 0},
    {NULL}
};
/* *INDENT-ON* */

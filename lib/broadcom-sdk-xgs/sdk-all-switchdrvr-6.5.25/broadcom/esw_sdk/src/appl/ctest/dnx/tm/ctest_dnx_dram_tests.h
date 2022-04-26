
/**
 * \file diag_dnx_dram_tests.h
 * Purpose: Extern declarations for command functions and
 *          their associated usage strings.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef CTEST_DNX_DRAM_TESTS_H_INCLUDED
/* { */
#define CTEST_DNX_DRAM_TESTS_H_INCLUDED

/*************
 * INCLUDES  *
 *************/

/*************
 *  MACROES  *
 *************/
/* { */
/*
 * Hard coded source port for core 0 when using SAT.
 */
#define SOURCE_TM_PORT_CORE_0_ON_SAT         218
/*
 * Hard coded source port for core 1 when using SAT.
 */
#define SOURCE_TM_PORT_CORE_1_ON_SAT         219
/*
 * This is the maximal nuumber of bytes in the name of the family of chips for which
 *  ports are set.
 */
#define CTEST_DNX_SIZEOF_COLLECTIVE_NAME     100
/*
 * This is the maximal nuumber of bytes in  the command as put on the BCM prompt line.
 */
#define CTEST_DNX_SIZEOF_BCM_COMMAND_LINE    200
/*
 * Maximal number of cycles allowed for 'operation buffer' test.
 * This is the maximal value for ctest_dnx_dram_operation_buffer_params_t->ctest_dnx_dram_operation_buffer_cycles
 */
#define CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES                     10
/*
 * Number of milliseconds to wait after an 'operation buffer' test cycle has completed
 * before verifying all counters are zero and next cycle can be started.
 */
#define CTEST_DNX_OPERATION_BUFFER_MILLI_SLEEP_AT_STOPPING        30000
/*
 * Number of monitoring loops, to carry out within ctest_dnx_dram_inactivity_verify(),
 * while checking activity counters.
 */
#define CTEST_DNX_DRAM_NUM_LOOPS_FOR_INACTIVITY                   10
/*
 * Maximal number of cycles allowed for 'operation rate' test.
 * This is the maximal value for ctest_dnx_dram_operation_rate_params_t->ctest_dnx_dram_operation_rate_cycles
 */
#define CTEST_DNX_OPERATION_RATE_MAX_CYCLES                       3
/*
 * Command line parameters for "ctest tm dram exercise operation rate ..."
 * See ctest_dnx_dram_operation_rate_test_options[]
 * {
 */
#define CTEST_DNX_DRAM_DRAM_ONLY             "dram_only"
#define CTEST_DNX_DRAM_EXPECT_DRAM           "expect_dram"
#define CTEST_DNX_DRAM_TEST_CYCLE            "test_cycle"
#define CTEST_DNX_DRAM_DISP_PARAMS           "disp_params"
#define CTEST_DNX_DRAM_SHOW_DEBUG            "show_debug"
#define CTEST_DNX_DRAM_EXPECT_ENQ_DEQ_DRAM   "expect_enq_deq_dram"
#define CTEST_DNX_DRAM_SAT_INJECTION_RATE    "sat_injection_rate"
#define CTEST_DNX_DRAM_EXPECTED_PORT_RATE    "expected_port_rate"
/*
 * }
 */
/* } */
/**************
 *  TYPEDEFS  *
 **************/
/* { */
/**
 * Contain the various parameters for DRAM OPERATION BUFFER tests.
 * Definitions used to activate the 'OPERATION BUFFER' tests under
 * variable conditions.
 */
typedef struct
{
    /**
     * Number of cycles to run the 'buffer' test (each with specified parameters).
     * See CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES.
     */
    int ctest_dnx_dram_operation_buffer_cycles;
    /**
     * Number of ports in the 'snake' structure. The number of ports which will
     * actually both transmit and receive will be 'ctest_dnx_dram_num_port_in_snake' - 1
     */
    int ctest_dnx_dram_num_port_in_snake;
    /**
     * Rate of injection of bytes by SAT. Units are internal to SAT but are
     * approximately 'kilo bits per second'.
     */
    uint32 sat_injection_rate_kbps[CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES];
    /**
     * Input burst size for SAT. Units are bytes and are interpreted internally by SAT.
     */
    uint32 sat_burst_size[CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES];
    /**
     * Input granularity size for SAT. Units are clocks per cycle and
     * are interpreted internally by SAT.
     */
    uint32 sat_granularity_size[CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES];
    /**
     * Number of milli seconds to keep the rate specified in 'sat_injection_rate_kbps'
     * before collecting counters' info and, following that, stopping the injection
     * for that cycle. 
     */
    uint32 delay_before_inspect_in_milli[CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES];
    /**
     * Number of seconds to set between the two counter samples which are used
     * to calculate tx and rx rate. 
     */
    uint32 num_sec_to_wait_for_rate[CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES];
    /**
     * Expected rate, both for TX and for RX, in mega bits per second.
     */
    uint32 expected_rate_in_mbps[CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES];
    /**
     * Allowed deviation from expected rate, in percents.
     */
    uint32 expected_rate_deviation_in_percents[CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES];
    /**
     * Flag indication whether to let all counters get back to zero before
     * going on to the next cycle.
     */
    int verify_clear_at_end_of_cycle[CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES];
    /**
     * Flag indication whether to expect DRAM to be in use on this cycle.
     */
    int expect_dram_to_be_used[CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES];
    /**
     * Flag indication whether to expect 'enq/deq' to/from DRAM on this cycle.
     */
    int expect_enq_deq_dram[CTEST_DNX_OPERATION_BUFFER_MAX_CYCLES];
} ctest_dnx_dram_operation_buffer_params_t;
/**
 * Contain the various parameters for DRAM OPERATION RATE tests.
 * Definitions used to activate the 'OPERATION RATE' tests under
 * variable conditions.
 */
typedef struct
{
    /**
     * Number of cycles to run the 'rate' test (each with specified parameters).
     * See CTEST_DNX_OPERATION_RATE_MAX_CYCLES.
     */
    int ctest_dnx_dram_operation_rate_cycles;
    /**
     * Number of ports used, in the 'snake' structure, for combining basic ports into higher
     * rate ports.
     */
    int ctest_dnx_dram_num_port_in_snake;
    /**
     * Number of physical ports, in the 'snake' structure, which are combined to one, higher rate,
     * port.
     */
    int ctest_dnx_num_phys_ports_in_one_combined_port;
    /**
     * Boolean flag. If non-zero then system activates debug HW which is intended to
     * make the chip use dram only.
     */
    int ctest_dnx_use_dram_only[CTEST_DNX_OPERATION_RATE_MAX_CYCLES];
    /**
     * Boolean flag. If non-zero then set 'severity' of 'log' messages to 'debug'.
     * Otherwise, leave it as it was set, by default, at system init.
     */
    int ctest_dnx_apply_debug_severity;
    /*
     * This is the text describing all ports including injected port. Appears on each line.
     * See 'ctest_dnx_num_phys_ports_in_one_combined_port'.
     */
    char *ctest_dnx_text_ports;
    /**
     * Rate of injection of bytes by SAT. Units are internal to SAT but are
     * approximately 'kilo bits per second'.
     */
    uint32 sat_injection_rate_kbps[CTEST_DNX_OPERATION_RATE_MAX_CYCLES];
    /**
     * Input packet size for SAT. Units are bytes.
     */
    uint32 sat_packet_size_bytes[CTEST_DNX_OPERATION_RATE_MAX_CYCLES];
    /**
     * Input burst size for SAT. Units are bytes and are interpreted internally by SAT.
     */
    uint32 sat_burst_size[CTEST_DNX_OPERATION_RATE_MAX_CYCLES];
    /**
     * Input granularity size for SAT. Units are clocks per cycle and
     * are interpreted internally by SAT.
     */
    uint32 sat_granularity_size[CTEST_DNX_OPERATION_RATE_MAX_CYCLES];
    /**
     * Number of milli seconds to keep the rate specified in 'sat_injection_rate_kbps'
     * before collecting counters' info and, following that, stopping the injection
     * for that cycle. 
     */
    uint32 delay_before_inspect_in_milli[CTEST_DNX_OPERATION_RATE_MAX_CYCLES];
    /**
     * Number of seconds to set between the two counter samples which are used
     * to calculate tx and rx rate. 
     */
    uint32 num_sec_to_wait_for_rate[CTEST_DNX_OPERATION_RATE_MAX_CYCLES];
    /**
     * Expected rate, on first snake port, both for TX and for RX, in mega bits per second.
     */
    uint32 expected_rate_in_mbps[CTEST_DNX_OPERATION_RATE_MAX_CYCLES];
    /**
     * Allowed deviation from expected rate, in percents.
     */
    uint32 expected_rate_deviation_in_percents[CTEST_DNX_OPERATION_RATE_MAX_CYCLES];
    /**
     * Flag indication whether to let all counters get back to zero before
     * going on to the next cycle.
     */
    int verify_clear_at_end_of_cycle[CTEST_DNX_OPERATION_RATE_MAX_CYCLES];
    /**
     * Flag indication whether to expect DRAM to be in use on this cycle.
     */
    int expect_dram_to_be_used[CTEST_DNX_OPERATION_RATE_MAX_CYCLES];
    /**
     * Value indicating what rate to expect on DRAM in/out bytes on this
     * cycle, in mega bits per second.
     */
    uint32 expected_dram_rate_in_mbps[CTEST_DNX_OPERATION_RATE_MAX_CYCLES];
    /**
     * Allowed deviation from expected DRAM rate, in percents.
     */
    uint32 expected_dram_rate_deviation_in_percents[CTEST_DNX_OPERATION_RATE_MAX_CYCLES];
    /**
     * Flag indication whether to expect 'enq/deq' to/from DRAM on this cycle.
     */
    int expect_enq_deq_dram[CTEST_DNX_OPERATION_RATE_MAX_CYCLES];
} ctest_dnx_dram_operation_rate_params_t;
/**
 * Describe the various DRAM OPERATION tests.
 * Definitions used to fill enum for type of DRAM test within 'OPERATION' tests
 */
typedef enum
{
    CTEST_DNX_DRAM_OPERATION_TEST_TYPE_FIRST = 0,
    /**
     * Carry out test of DRAM: validate Dram buffer usage
     */
    CTEST_DNX_DRAM_OPERATION_TEST_BUFFER_TYPE = CTEST_DNX_DRAM_OPERATION_TEST_TYPE_FIRST,
    /**
     * Carry out test of DRAM: Usage rate of full scenario
     */
    CTEST_DNX_DRAM_OPERATION_TEST_RATE_TYPE,
    /**
     * Number of types in ctest_dnx_dram_operation_test_type_e
     */
    CTEST_DNX_DRAM_OPERATION_TEST_TYPE_NOF
} ctest_dnx_dram_operation_test_type_e;
/* } */

/*************
 * GLOBALS   *
 *************/

/*************
 * FUNCTIONS *
 *************/

extern sh_sand_cmd_t sh_dnx_tm_dram_test_cmds[];
/* } */
#endif /* CTEST_DNX_DRAM_TESTS_H_INCLUDED */

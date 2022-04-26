/** \file diag_dnx_field_exem_learn_flush.c
 * $Id$
 *
 * 'EXEM' operations (for group add and entry add) procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /*
  * Include files.
  * {
  */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>

#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "ctest_dnx_field_exem_learn_flush.h"
#include "ctest_dnx_field_utils.h"
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_flush.h>
#include <bcm_int/dnx/field/field_context.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/sand/sand_signals.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/*
 * }
 */
/*
 * MACROskey_size
 * {
 */
/**
 * \brief stage=ipmf3 traffic=yes
 *   Keyword for the stage for which groups are to be created.
 */
#define DNX_CTEST_EXEM_LEARN_FLUSH_OPTION_TEST_STAGE        "stage"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define DNX_CTEST_EXEM_LEARN_FLUSH_OPTION_TEST_COUNT        "count"
/**
 * \brief
 *   Keyword for controlling whether to perform traffic test. can be either 0 or 1.
 */
#define DNX_CTEST_EXEM_LEARN_FLUSH_OPTION_TEST_TRAFFIC     "traffic"
/**
 * \brief
 *   Keyword for controlling whether to release resources at end of test.
 *   clean can be either 0 or 1, if the ctest will clean-up after its finished setting-up.
 */
#define DNX_CTEST_EXEM_LEARN_FLUSH_OPTION_TEST_CLEAN       "clean"

/*
 * }
 */
 /*
  * Global and Static
  * {
  */
/* *INDENT-OFF* */

/**
 * \brief
 *   List of tests for 'exem_learn_flush' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 *   We skip the iPMF2 tests, as we can use exem on either iPMF2 or iPMF3 depending on
 *   soc property pmf_sexem3_stage, which by default chooses iPMF3.
 *   Also skip traffic test on LEXEM, as currently the scan machine doesn't run for it.
 */
sh_sand_invoke_t Sh_dnx_field_exem_learn_flush_tests[] = {
    {"DNX_field_exem_learn_flush_1",         "stage=ipmf1 count=6",              CTEST_POSTCOMMIT}
    ,
    {"DNX_field_exem_learn_flush_2",         "stage=ipmf2 count=6",              CTEST_POSTCOMMIT}
    ,
    {"DNX_field_exem_learn_flush_3",         "stage=ipmf3 count=6",              SH_CMD_SKIP_EXEC}
    ,
    {"DNX_field_exem_learn_flush_e",         "stage=epmf count=6",               CTEST_POSTCOMMIT}
    ,
    {"DNX_field_exem_learn_flush_1_traffic", "stage=ipmf1 traffic=yes count=2",  SH_CMD_SKIP_EXEC}
    ,
    {"DNX_field_exem_learn_flush_2_traffic", "stage=ipmf2 traffic=yes count=2",  CTEST_POSTCOMMIT}
    ,
    {"DNX_field_exem_learn_flush_3_traffic", "stage=ipmf3 traffic=yes count=2",  SH_CMD_SKIP_EXEC}
    ,
    {"DNX_field_exem_learn_flush_e_traffic", "stage=epmf traffic=yes count=2",   SH_CMD_SKIP_EXEC}
    ,
    {NULL}
};
/**
 * \brief
 *   Options list for 'exem' shell command
 */
sh_sand_option_t Sh_dnx_field_exem_learn_flush_options[] = {
     /* Name */                              /* Type */              /* Description */                          /* Default */
    {DNX_CTEST_EXEM_LEARN_FLUSH_OPTION_TEST_STAGE,      SAL_FIELD_TYPE_ENUM,   "Stage of test (ipmf1,ipmf2,ipmf3,epmf)",     "ipmf2", (void *)Field_stage_enum_table},
    {DNX_CTEST_EXEM_LEARN_FLUSH_OPTION_TEST_TRAFFIC,    SAL_FIELD_TYPE_BOOL,   "Will the test include traffic test or not",  "No"},
    {DNX_CTEST_EXEM_LEARN_FLUSH_OPTION_TEST_CLEAN,      SAL_FIELD_TYPE_BOOL,   "Will the test perform clean-up or not",      "Yes"},
    {DNX_CTEST_EXEM_LEARN_FLUSH_OPTION_TEST_COUNT,      SAL_FIELD_TYPE_UINT32, "Number of times test will run",              "1"},
    {NULL}      /* End of options list - must be last. */
};
/* *INDENT-ON* */
/*
 * }
 */

/**
 *  The description of the ctest options.
 */
sh_sand_man_t Sh_dnx_field_exem_learn_flush_man = {
    "Tests related to flush entries used in EXEM learn.",
    "Activate exem learn flsuh machine entries related test utilities.\r\n" "creates EXEM groups flush entries.\r\n"
        "Stage performs the test for a specific PMF stage (default is iPMF1).\r\n"
        "Count indicates the number of times the test will run.\r\n",
    "ctest field exem_learn_flush stage=<ipmf1 | ipmf2 | ipmf3 | epmf> traffic=<Yes | No> clean=<Yes | No>",
    "traffic=Yes\n" "stage=ipmf1",
};

/** The number of qualifiers used in the test.*/
#define CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_QUALS        2
/** The number of actions used in the test.*/
#define CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_ACTIONS      2
/** The number of stages used in the test. Used for array size.*/
#define CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_STAGES       4
/** The priority of the actions in the field group.*/
#define CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_ACTION_PRIORITY  (BCM_FIELD_ACTION_PRIORITY(0, 3))

/**
 * Number of retries to apply if, after sending a packet, signals were not yet received.
 * Time delay, in seconds, between retries, is CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_PACKET_WAIT.
 */
#define CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_PACKET_RETRY     0
/**
 * For each retry (as seen above in CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_PACKET_RETRY), how many seconds to wait.
 */
#define CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_PACKET_WAIT      5
/**
 * The presel ID.
 */
#define CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_PRESEL_ID        45

/** The qualifierto be used for each stage.*/
static bcm_field_qualify_t
    Dnx_quals_array[CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_STAGES][CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_QUALS] = {
    {bcmFieldQualifySrcIp, bcmFieldQualifyDstIp},
    {bcmFieldQualifySrcIp, bcmFieldQualifyDstIp},
    {bcmFieldQualifyUDHData0, bcmFieldQualifyUDHData1},
    {bcmFieldQualifySrcIp, bcmFieldQualifyDstIp},
};

/** The qualifier input type to be used for each stage.*/
static bcm_field_input_types_t
    Quals_input_type[CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_STAGES][CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_QUALS] = {
    {bcmFieldInputTypeLayerAbsolute, bcmFieldInputTypeLayerAbsolute},
    {bcmFieldInputTypeLayerAbsolute, bcmFieldInputTypeLayerAbsolute},
    {bcmFieldInputTypeMetaData, bcmFieldInputTypeMetaData},
    {bcmFieldInputTypeLayerAbsolute, bcmFieldInputTypeLayerAbsolute},
};

/** The qualifier input argument to be used for each stage.*/
static int Quals_input_arg[CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_STAGES][CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_QUALS] = {
    {1, 1},
    {1, 1},
    {0, 0},
    {1, 1},
};

/** The qualifier offset to be used for each stage.*/
static int Quals_offset[CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_STAGES][CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_QUALS] = {
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
};

/** The values to write in the qualifiers for the entry. at the moment only iPMF1 checks actual data from the packet.*/
static uint32 Quals_values[CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_STAGES][CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_QUALS] = {
    {1, 2},
    {1, 2},
    {0, 0},
    {1, 2}
};

/** The DNX_ACTION to be used for each stage.*/
static bcm_field_action_t
    Dnx_actions_array[CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_STAGES][CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_ACTIONS] = {
    {bcmFieldActionPrioIntNew, bcmFieldActionDropPrecedenceRaw},
    {bcmFieldActionPrioIntNew, bcmFieldActionDropPrecedenceRaw},
    {bcmFieldActionPrioIntNew, bcmFieldActionDropPrecedenceRaw},
    {bcmFieldActionPrioIntNew, bcmFieldActionDropPrecedenceRaw},

};

/** The values to write in the actions for the entry. */
static uint32 Actions_values[CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_STAGES][CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_QUALS] = {
    {1, 2},
    {1, 2},
    {1, 2},
    {1, 2}
};

/** The values to update in the plus entry. */
static uint32
    Actions_updated_values[CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_STAGES][CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_QUALS] = {
    {2, 1},
    {2, 1},
    {2, 1},
    {2, 1}
};

/*
 * An array, which contains all needed signal information for the traffic testing:
 * core, block, from, to, signal name, size of the buffer and expected value (the last unsused).
 */
static dnx_field_utils_signal_info_t Packet_sig_info[CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_STAGES] = {
/*
 * Fields are: Core, Block, From, to, signal name, size of buffer, expected value.
 * Expected value isn't taken from here, and is merely initialized.
 */
    {1, "IRPP", "IPMF1", "", "tc", 3, 0},
    {1, "IRPP", "IPMF1", "", "tc", 3, 0},
    {1, "IRPP", "IPMF3", "", "tc", 3, 0},
    {0, "ERPP", "EPMF", "", "ftmh_tc", 3, 0}
};

/* The packet to be sent.*/
static dnx_field_utils_packet_info_t Packet_info = {
    {"PTCH_2", "PTCH_2.PP_SSP", "27", "ETH1", "IPv4", "IPv4.SIP", "0.0.0.1", "IPv4.DIP", "0.0.0.2", ""}
};

/**
 * \brief
 *   This function sends a packet and checks the signals.
 * \param [in] unit -
 *    The unit number.
 * \param [in] field_stage -
 *    The PMF stage for which we run the test (IPMF1, IPMF2, IPMF3, EPMF).
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
ctest_dnx_exem_learn_flush_traffic_test(
    int unit,
    dnx_field_stage_e field_stage,
    uint32 expected_value)
{
    int rv;
    char *return_value;
    unsigned int num_retries;
    uint32 expected_value_local;
    bcm_port_mapping_info_t mapping_info_in;
    bcm_port_mapping_info_t mapping_info_verify;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    int port_verify;
    SHR_FUNC_INIT_VARS(unit);

    expected_value_local = expected_value;

    SHR_IF_ERR_EXIT(bcm_port_get(unit, 200, &flags, &interface_info, &mapping_info_in));

    sal_snprintf(Packet_info.header_info[2], DNX_FIELD_UTILS_STR_SIZE, "%d", mapping_info_in.pp_port);

    if (field_stage == DNX_FIELD_STAGE_EPMF)
    {
        port_verify = 201;
    }
    else
    {
        port_verify = 200;
    }
    SHR_IF_ERR_EXIT(bcm_port_get(unit, port_verify, &flags, &interface_info, &mapping_info_verify));
    /*
     * Send traffic.
     */
    SHR_IF_ERR_EXIT(ctest_dnx_field_utils_packet_tx(unit, 200, &Packet_info));

    /*
     * Get verify the output.
     */
    num_retries = 0;
    do
    {
        /*
         * If this is a retry, wait a specified amount of time.
         * Note we do not check the return value of sal_sleep().
         */
        if (num_retries > 0)
        {
            sal_sleep(CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_PACKET_WAIT);
        }
        return_value = NULL;
        rv = sand_signal_verify(unit, mapping_info_verify.core,
                                Packet_sig_info[field_stage].block,
                                Packet_sig_info[field_stage].from,
                                Packet_sig_info[field_stage].to,
                                Packet_sig_info[field_stage].sig_name,
                                &(expected_value_local), Packet_sig_info[field_stage].size, NULL, return_value, 0);
        num_retries++;
    }
    while (num_retries < CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_PACKET_RETRY + 1 && rv == _SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT_WITH_LOG(rv, "Field EXEM traffic test failed. Result of signal verify is %s.%s%s",
                             return_value, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Performs operational testing for the EXEM learn flush entry related code.
 *    Creates an EXEM field group and adds a flush entry.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] field_stage -
 *    For which the EXEM field group is to be created and tested (IPMF1, IPMF2, IPMF3, EPMF).
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \param [in] test_traffic -
 *    Boolean, if true performs traffic test.
 * \param [in] test_clean -
 *    Boolean, if true deletes the configuration performed by the test
 * \remark
 *   * The procedure uses 'Dnx_quals_input_type', 'Dnx_quals_array' and 'Dnx_actions_array' as input for the 
 *     field group.
 *   * The procedure uses 'Packet_sig_info' and 'Packet_info' for packet information for traffic test.
 * \see
 *   * appl_dnx_exem_starter
 */
static shr_error_e
ctest_dnx_exem_learn_flush_tester(
    int unit,
    dnx_field_stage_e field_stage,
    int test_traffic,
    int test_clean)
{
    bcm_field_stage_t bcm_stage;
    bcm_field_group_t fg_id;
    bcm_field_context_t context_id;
    dnx_field_presel_entry_id_t p_id;
    dnx_field_presel_entry_data_t p_data;
    bcm_field_group_info_t fg_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_group_attach_info_t attach_info;
    dbal_physical_tables_e dbal_physical_table;
    int scan_period_index;
    unsigned int stage_ndx;
    unsigned int qual_ndx;
    unsigned int action_ndx;
    bsl_severity_t original_severity_fldprocdnx;
    bsl_severity_t original_severity_dbaldnx;
    bcm_switch_control_key_t control_key;
    bcm_switch_control_info_t control_info;
    bcm_switch_control_info_t scan_control_info_original;
    bcm_field_flush_profile_info_t flush_profile_info;
    uint32 flush_profile_id;
    bcm_field_flush_entry_info_t flush_entry_info;
    bcm_field_flush_entry_info_t flush_entry_info_get;
    bcm_field_flush_entry_info_t flush_entry_info_other;
    dnx_field_stage_e context_field_stage;
    dnx_field_context_mode_t context_mode;
    shr_error_e rv;
    uint32 flush_entry_flags;
    int flush_entry_id = 1;
    int flush_entry_id_other = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the severity level. We silence the error messages for the negative cases.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);

    /*
     * Verify the sizes of the entry structure arrays fit.
     */
    {
        bcm_field_flush_entry_info_t bcm_flush_entry_info;
        dnx_field_flush_entry_t dnx_flush_entry_info;
        bcm_field_entry_info_t entry_add_bcm_entry_info;
        dnx_field_entry_t entry_add_dnx_entry_info;
        if ((sizeof(dnx_flush_entry_info.key_info.qual_info) / sizeof(dnx_flush_entry_info.key_info.qual_info[0])) !=
            (sizeof(entry_add_dnx_entry_info.key_info.qual_info) /
             sizeof(entry_add_dnx_entry_info.key_info.qual_info[0])))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between flush entry key quals and entry add quals.\r\n");
        }
        if ((sizeof(dnx_flush_entry_info.key_info.action_info) /
             sizeof(dnx_flush_entry_info.key_info.action_info[0])) !=
            (sizeof(entry_add_dnx_entry_info.payload_info.action_info) /
             sizeof(entry_add_dnx_entry_info.payload_info.action_info[0])))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between flush entry key actions and entry add actions.\r\n");
        }
        if ((sizeof(dnx_flush_entry_info.key_info.action_info) /
             sizeof(dnx_flush_entry_info.key_info.action_info[0])) !=
            (sizeof(dnx_flush_entry_info.payload_info.action_info) /
             sizeof(dnx_flush_entry_info.payload_info.action_info[0])))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Size mismatch between flush entry key actions and payload_info actions.\r\n");
        }
        if ((sizeof(bcm_flush_entry_info.key.entry_qual) / sizeof(bcm_flush_entry_info.key.entry_qual[0])) !=
            (sizeof(entry_add_bcm_entry_info.entry_qual) / sizeof(entry_add_bcm_entry_info.entry_qual[0])))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between BCM flush entry key quals and entry add quals.\r\n");
        }
        if ((sizeof(bcm_flush_entry_info.key.entry_action) / sizeof(bcm_flush_entry_info.key.entry_action[0])) !=
            (sizeof(entry_add_bcm_entry_info.entry_action) / sizeof(entry_add_bcm_entry_info.entry_action[0])))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Size mismatch between BCM flush entry key actions and entry add actions.\r\n");
        }
        if ((sizeof(bcm_flush_entry_info.key.entry_action) / sizeof(bcm_flush_entry_info.key.entry_action[0])) !=
            (sizeof(bcm_flush_entry_info.payload.entry_action) / sizeof(bcm_flush_entry_info.payload.entry_action[0])))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between BCM flush entry key actions and payload actions.\r\n");
        }
        if (sizeof(bcm_flush_entry_info.key.entry_action[0].mask) !=
            sizeof(dnx_flush_entry_info.key_info.action_info[0].action_mask))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between action mask size BCM and DNX.\r\n");
        }
        if (sizeof(bcm_flush_entry_info.key.entry_action[0].mask) !=
            sizeof(bcm_flush_entry_info.payload.entry_action[0].mask))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between action mask size key and payload.\r\n");
        }
        if (sizeof(bcm_flush_entry_info.key.entry_qual[0].mask) != sizeof(entry_add_bcm_entry_info.entry_qual[0].mask))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between qual mask size BCM flush and entry add.\r\n");
        }
        if (sizeof(bcm_flush_entry_info.key.entry_qual[0].value) !=
            sizeof(entry_add_bcm_entry_info.entry_qual[0].value))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between qual value size BCM flush and entry add.\r\n");
        }
        if (sizeof(bcm_flush_entry_info.key.entry_action[0].value) !=
            sizeof(entry_add_bcm_entry_info.entry_action[0].value))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between action value size BCM flush and entry add.\r\n");
        }
        if (sizeof(bcm_flush_entry_info.key.entry_action[0].value) !=
            sizeof(bcm_flush_entry_info.payload.entry_action[0].value))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between action value size BCM flush key and payload.\r\n");
        }
        if (sizeof(dnx_flush_entry_info.key_info.qual_info[0].qual_mask) !=
            sizeof(entry_add_dnx_entry_info.key_info.qual_info[0].qual_mask))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between qual mask size DNX flush and entry add.\r\n");
        }
        if (sizeof(dnx_flush_entry_info.key_info.qual_info[0].qual_value) !=
            sizeof(entry_add_dnx_entry_info.key_info.qual_info[0].qual_value))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between qual value size DNX flush and entry add.\r\n");
        }
        if (sizeof(dnx_flush_entry_info.key_info.action_info[0].action_value) !=
            sizeof(entry_add_dnx_entry_info.payload_info.action_info[0].action_value))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between action value size DNX flush and entry add.\r\n");
        }
        if (sizeof(dnx_flush_entry_info.key_info.action_info[0].action_value) !=
            sizeof(dnx_flush_entry_info.payload_info.action_info[0].action_value))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between action value size DNX flush key and payload.\r\n");
        }
        if (sizeof(bcm_flush_entry_info.key.entry_action[0].value) !=
            sizeof(dnx_flush_entry_info.key_info.action_info[0].action_value))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between action value size BCM and DNX.\r\n");
        }
    }

    /*
     * Find the index of the stage for selecting qualifiers and actions.
     * Also fill the information regarding whether we expect it to be LEXEM or SEXEM,
     * by filling the 'action_length_type' and 'field_io' that we expect to get from  
     * dnx_field_group_get() and dnx_field_group_context_get().
     */
    switch (field_stage)
    {
#if defined(BCM_DNX2_SUPPORT)
        case DNX_FIELD_STAGE_IPMF1:
        {
            stage_ndx = 0;
            dbal_physical_table = DBAL_PHYSICAL_TABLE_LEXEM;
            scan_period_index = 1;
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            stage_ndx = 1;
            dbal_physical_table = DBAL_PHYSICAL_TABLE_SEXEM_3;
            scan_period_index = 0;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            stage_ndx = 2;
            dbal_physical_table = DBAL_PHYSICAL_TABLE_SEXEM_3;
            scan_period_index = 0;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            stage_ndx = 3;
            dbal_physical_table = DBAL_PHYSICAL_TABLE_LEXEM;
            scan_period_index = 1;
            break;
        }
#endif /* BCM_DNX2_SUPPORT */
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid stage (%d).\r\n", field_stage);
            break;
        }
    }

    /*
     * Create a context.
     */
    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        context_field_stage = DNX_FIELD_STAGE_IPMF1;
    }
    else
    {
        context_field_stage = field_stage;
    }
    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
    sal_strncpy_s(context_mode.name, "EXEM_FLUSH", sizeof(context_mode.name));
    SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, context_field_stage, &context_mode, &context_id));
    dnx_field_presel_entry_id_t_init(unit, &p_id);
    dnx_field_presel_entry_data_t_init(unit, &p_data);
    p_id.presel_id = CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_PRESEL_ID;
    p_id.stage = context_field_stage;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 0;
    SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, 0, &p_id, &p_data));

    /*
     * Create an EXEM field group.
     */
    dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage);
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeExactMatch;
    fg_info.stage = bcm_stage;
    fg_info.nof_quals = CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_QUALS;
    for (qual_ndx = 0; qual_ndx < CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_QUALS; qual_ndx++)
    {
        fg_info.qual_types[qual_ndx] = Dnx_quals_array[stage_ndx][qual_ndx];
    }
    fg_info.nof_actions = CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_ACTIONS;
    for (action_ndx = 0; action_ndx < CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_ACTIONS; action_ndx++)
    {
        fg_info.action_types[action_ndx] = Dnx_actions_array[stage_ndx][action_ndx];
    }
    sal_strncpy((char *) fg_info.name, "EXEM_FLUSH_CTEST", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &fg_id));

    /*
     * Prepare creating the entry.
     */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_QUALS;
    for (qual_ndx = 0; qual_ndx < CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_QUALS; qual_ndx++)
    {
        entry_info.entry_qual[qual_ndx].type = Dnx_quals_array[stage_ndx][qual_ndx];
        entry_info.entry_qual[qual_ndx].value[0] = Quals_values[stage_ndx][qual_ndx];
    }
    entry_info.nof_entry_actions = CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_ACTIONS;
    for (action_ndx = 0; action_ndx < CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_ACTIONS; action_ndx++)
    {
        entry_info.entry_action[action_ndx].type = Dnx_actions_array[stage_ndx][action_ndx];
        entry_info.entry_action[action_ndx].value[0] = Actions_values[stage_ndx][action_ndx];
    }

    /*
     * Attach group to context.
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_QUALS;
    for (qual_ndx = 0; qual_ndx < CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_QUALS; qual_ndx++)
    {
        attach_info.key_info.qual_types[qual_ndx] = Dnx_quals_array[stage_ndx][qual_ndx];
        attach_info.key_info.qual_info[qual_ndx].input_type = Quals_input_type[stage_ndx][qual_ndx];
        attach_info.key_info.qual_info[qual_ndx].input_arg = Quals_input_arg[stage_ndx][qual_ndx];
        attach_info.key_info.qual_info[qual_ndx].offset = Quals_offset[stage_ndx][qual_ndx];
    }
    attach_info.payload_info.nof_actions = CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_ACTIONS;
    for (action_ndx = 0; action_ndx < CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_NUM_ACTIONS; action_ndx++)
    {
        attach_info.payload_info.action_types[action_ndx] = Dnx_actions_array[stage_ndx][action_ndx];
        attach_info.payload_info.action_info[action_ndx].priority = CTEST_DNX_FIELD_EXEM_LEARN_FLUSH_ACTION_PRIORITY;
    }
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    /*
     * Verify that the VMV size of the field group if equal to the VMV size of all EXEM ifeld groups in that stage.
     */
    {
        uint8 fg_vmv_size;
        uint8 fg_vmv_value;
        uint8 stage_vmv_value;
        SHR_IF_ERR_EXIT(dnx_field_group_vmv_get(unit, fg_id, &fg_vmv_size, &fg_vmv_value));
        SHR_IF_ERR_EXIT(dnx_field_group_physical_db_vmv_size_get(unit, dbal_physical_table, &stage_vmv_value));
        if (stage_vmv_value != fg_vmv_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "EXEM field group %d has VMV %d, but the VMV on stage %s is %d.\r\n",
                         fg_id, fg_vmv_size, dnx_field_stage_text(unit, field_stage), stage_vmv_value);
        }
    }

    /*
     * Verify that size of the payload is the same when we get it from DBAL or field_group.
     */
    {
        int dbal_payload_size;
        unsigned int fg_payload_size;
        dbal_tables_e dbal_table_id;
        SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
        SHR_IF_ERR_EXIT(dbal_tables_payload_size_get(unit, dbal_table_id, 0, &dbal_payload_size));
        SHR_IF_ERR_EXIT(dnx_field_group_actions_payload_size_get(unit, fg_id, &fg_payload_size));
        if (dbal_payload_size != fg_payload_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "EXEM field group %d (dbal table ID %d) has payload size %d, but in dbal %d.\r\n", fg_id,
                         dbal_table_id, fg_payload_size, dbal_payload_size);
        }
    }

    /*
     * Only configure the learn mechanism for traffic tests.
     */
    if (test_traffic)
    {
        /*
         * Configure scan period.
         */
        control_key.type = bcmSwitchExemScanPeriod;
        control_key.index = scan_period_index;
        control_info.value = 1000000;
        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_get(unit, control_key, &scan_control_info_original));
        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, control_key, control_info));

        /*
         * Create flush profile.
         */
        bcm_field_flush_profile_info_t_init(&flush_profile_info);
        flush_profile_info.decrement_value = 0;
        flush_profile_info.increment_value = 2;
        flush_profile_info.high_threshold_value = 4;
        flush_profile_info.low_threshold_value = 2;
        flush_profile_info.maximal_value = 15;
        flush_profile_info.out_value = 0;
        flush_profile_info.init_value = 1;
        flush_profile_id = 0;
        SHR_IF_ERR_EXIT(bcm_field_flush_profile_create(unit, 0, &flush_profile_info, &flush_profile_id));

        /*
         * Attach flush profile to field group.
         */
        SHR_IF_ERR_EXIT(bcm_field_flush_profile_attach(unit, 0, fg_id, flush_profile_id));
    }

    /*
     * Get if SEXEM or LEXEM. Check that it matches the stage.
     */
    {
        int is_lexem;
        SHR_IF_ERR_EXIT(dnx_field_group_exem_is_lexem(unit, fg_id, &is_lexem));
        if (is_lexem && (scan_period_index != 1))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "EXEM field group %d in stage \"%s\" (%d) uses LEXEM but scan_period for stage is %d.\r\n",
                         fg_id, dnx_field_stage_text(unit, field_stage), field_stage, scan_period_index);
        }
        if ((is_lexem == FALSE) && (scan_period_index != 0))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "EXEM field group %d in stage \"%s\" (%d) uses SEXEM but scan_period for stage is %d.\r\n",
                         fg_id, dnx_field_stage_text(unit, field_stage), field_stage, scan_period_index);
        }
    }

    /*
     * Flush entry
     */
    bcm_field_flush_entry_info_t_init(&flush_entry_info);
    flush_entry_info.key.nof_entry_quals = 1;
    flush_entry_info.key.entry_qual[0].type = Dnx_quals_array[stage_ndx][0];
    flush_entry_info.key.entry_qual[0].value[0] = Quals_values[stage_ndx][0];
    flush_entry_info.key.entry_qual[0].mask[0] = 3;
    flush_entry_info.key.nof_entry_actions = 1;
    flush_entry_info.key.entry_action[0].type = Dnx_actions_array[stage_ndx][0];
    flush_entry_info.key.entry_action[0].value[0] = Actions_values[stage_ndx][0];
    flush_entry_info.key.entry_action[0].mask[0] = 3;
    flush_entry_info.key.entry_action[0].valid_bit_value = 1;
    flush_entry_info.key.entry_action[0].valid_bit_mask = 1;
    flush_entry_info.key.hit = 0x1;
    flush_entry_info.key.hit_mask = 0x1;
    flush_entry_info.payload.nof_entry_actions = 1;
    flush_entry_info.payload.entry_action[0].type = Dnx_actions_array[stage_ndx][0];
    flush_entry_info.payload.entry_action[0].value[0] = Actions_updated_values[stage_ndx][0];
    flush_entry_info.payload.entry_action[0].mask[0] = 3;
    flush_entry_info.payload.entry_action[0].valid_bit_value = 1;
    flush_entry_info.payload.entry_action[0].valid_bit_mask = 1;
    flush_entry_info.payload.transplant_cmd = TRUE;
    flush_entry_flags = 0;

    /*
     * Adding the flush entry
     */
    SHR_IF_ERR_EXIT(bcm_field_flush_entry_add(unit, flush_entry_flags, fg_id, flush_entry_id, &flush_entry_info));

    /*
     * Adding the EXEM entry
     */
    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info, NULL));

    /*
     * Extra semantic tests, to be performed only for non traffic case
     */
    if (test_traffic == FALSE)
    {
        /*
         * Get entry 
         */
        SHR_IF_ERR_EXIT(bcm_field_flush_entry_get(unit, 0, fg_id, flush_entry_id, &flush_entry_info_get));
        if (sal_memcmp(&flush_entry_info_get, &flush_entry_info, sizeof(flush_entry_info_get)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Get does not equal set.\r\n");
        }
        /*
         * Fail adding an entry in the same place, and then succeed adding in another entry id
         * Remove the new entry afterwards. Fail removing it twice.
         * The other entry is a delete entry place in a lower entry ID (higher priority).
         */
        sal_memcpy(&flush_entry_info_other, &flush_entry_info, sizeof(flush_entry_info_other));
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal);
        rv = bcm_field_flush_entry_add(unit, flush_entry_flags, fg_id, flush_entry_id, &flush_entry_info_other);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
        if (rv != _SHR_E_EXISTS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Added the same entry twice.\r\n");
        }
        SHR_IF_ERR_EXIT(bcm_field_flush_entry_add
                        (unit, flush_entry_flags, fg_id, flush_entry_id_other, &flush_entry_info_other));
        SHR_IF_ERR_EXIT(bcm_field_flush_entry_delete(unit, flush_entry_flags, fg_id, flush_entry_id_other));
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal);
        rv = bcm_field_flush_entry_delete(unit, flush_entry_flags, fg_id, flush_entry_id_other);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Deleted the same entry twice.\r\n");
        }
    }
    /*
     * Perform traffic test if required by 'traffic' option .
     */
    if (test_traffic)
    {
        /*
         * Verify the payload hasn't changed.
         */
        SHR_IF_ERR_EXIT(ctest_dnx_exem_learn_flush_traffic_test(unit, field_stage, Actions_values[stage_ndx][0]));
        sal_sleep(1);
        SHR_IF_ERR_EXIT(ctest_dnx_exem_learn_flush_traffic_test(unit, field_stage, Actions_values[stage_ndx][0]));
        sal_sleep(1);
        SHR_IF_ERR_EXIT(ctest_dnx_exem_learn_flush_traffic_test(unit, field_stage, Actions_values[stage_ndx][0]));
        sal_sleep(1);
        SHR_IF_ERR_EXIT(ctest_dnx_exem_learn_flush_traffic_test
                        (unit, field_stage, Actions_updated_values[stage_ndx][0]));
        sal_sleep(1);
    }

    /*
     * Replace the flush entry with a new one.
     */
    SHR_IF_ERR_EXIT(bcm_field_flush_entry_delete(unit, flush_entry_flags, fg_id, flush_entry_id));
    flush_entry_info.payload.transplant_cmd = TRUE;
    flush_entry_info.payload.delete_cmd = FALSE;
    flush_entry_info.key.entry_action[0].value[0] = Actions_updated_values[stage_ndx][0];
    flush_entry_info.payload.entry_action[0].value[0] = 3;
    SHR_IF_ERR_EXIT(bcm_field_flush_entry_add(unit, flush_entry_flags, fg_id, flush_entry_id, &flush_entry_info));
    SHR_IF_ERR_EXIT(bcm_field_flush_entry_get(unit, 0, fg_id, flush_entry_id, &flush_entry_info_get));
    if (sal_memcmp(&flush_entry_info_get, &flush_entry_info, sizeof(flush_entry_info_get)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Get does not equal set.\r\n");
    }

    /*
     * Perform traffic test if required by 'traffic' option .
     */
    if (test_traffic)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_exem_learn_flush_traffic_test
                        (unit, field_stage, Actions_updated_values[stage_ndx][0]));
        sal_sleep(1);
        SHR_IF_ERR_EXIT(ctest_dnx_exem_learn_flush_traffic_test(unit, field_stage, 3));
        sal_sleep(1);
    }

    /*
     * Perform clean-up, if required by 'clean' option.
     */
    if (test_clean)
    {
        if (test_traffic)
        {
            SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, control_key, scan_control_info_original));
            SHR_IF_ERR_EXIT(bcm_field_flush_profile_destroy(unit, 0, flush_profile_id));
        }
        SHR_IF_ERR_EXIT(bcm_field_flush_entry_delete(unit, flush_entry_flags, fg_id, flush_entry_id));
        SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id, context_id));
        SHR_IF_ERR_EXIT(bcm_field_entry_delete(unit, fg_id, entry_info.entry_qual, 0));
        SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, fg_id));
        p_data.entry_valid = FALSE;
        SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, 0, &p_id, &p_data));
        SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, context_field_stage, context_id));
        sal_sleep(1);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is the basic 'exem_learn_flush' testing
 *   application.
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
ctest_dnx_exem_learn_flush_starter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e field_stage;
    int test_traffic;
    int test_clean;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_ENUM(DNX_CTEST_EXEM_LEARN_FLUSH_OPTION_TEST_STAGE, field_stage);
    SH_SAND_GET_BOOL(DNX_CTEST_EXEM_LEARN_FLUSH_OPTION_TEST_TRAFFIC, test_traffic);
    SH_SAND_GET_BOOL(DNX_CTEST_EXEM_LEARN_FLUSH_OPTION_TEST_CLEAN, test_clean);

    if (dnx_data_field.features.exem_age_flush_scan_get(unit) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "EXEM Age/Flush scan is not supported on this device.\n");
    }

#if defined(ADAPTER_SERVER_MODE)
    if (1)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "EXEM learn flush test not available for adapter.\n");
    }
    else
#endif
    {
        SHR_IF_ERR_EXIT(ctest_dnx_exem_learn_flush_tester(unit, field_stage, test_traffic, test_clean));
    }

exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief - run 'exem_learn_flush' sequence in diag shell
 */
shr_error_e
sh_dnx_field_exem_learn_flush_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 count, count_iter;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SH_SAND_GET_UINT32("count", count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_exem_learn_flush_starter(unit, args, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

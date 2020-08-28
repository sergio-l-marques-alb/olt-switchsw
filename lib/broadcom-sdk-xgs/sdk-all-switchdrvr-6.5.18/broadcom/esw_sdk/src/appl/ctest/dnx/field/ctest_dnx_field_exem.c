/** \file diag_dnx_field_exem.c
 * $Id$
 *
 * 'EXEM' operations (for group add and entry add) procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
#include "ctest_dnx_field_exem.h"
#include "ctest_dnx_field_utils.h"
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_entry.h>
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
 * MACROs
 * {
 */
/**
 * \brief
 *   Keyword for the stage for which groups are to be created.
 */
#define DNX_DIAG_EXEM_OPTION_TEST_STAGE        "stage"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define DNX_DIAG_EXEM_OPTION_TEST_COUNT        "count"
/**
 * \brief
 *   Keyword for controlling whether to perform tracfic test. can be either 0 or 1.
 */
#define DNX_CTEST_EXEM_OPTION_TEST_TRAFFIC     "traffic"
/**
 * \brief
 *   Keyword for controlling whether to release resources at end of test.
 *   clean can be either 0 or 1, if the ctest will clean-up after its finished setting-up.
 */
#define DNX_CTEST_EXEM_OPTION_TEST_CLEAN       "clean"

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
 *   List of tests for 'exem' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 *   We skip the iPMF2 tests, as we can use exem on either iPMF2 or iPMF3 depending on
 *   soc property pmf_sexem3_stage, which by default chooses iPMF3.
 */
sh_sand_invoke_t Sh_dnx_field_exem_tests[] = {
    {"DNX_field_exem_1",         "stage=ipmf1 count=6",              CTEST_POSTCOMMIT}
    ,
    {"DNX_field_exem_2",         "stage=ipmf2 count=6",              SH_CMD_SKIP_EXEC}
    ,
    {"DNX_field_exem_3",         "stage=ipmf3 count=6",              CTEST_POSTCOMMIT}
    ,
    {"DNX_field_exem_e",         "stage=epmf count=6",               CTEST_POSTCOMMIT}
    ,
    {"DNX_field_exem_1_traffic", "stage=ipmf1 traffic=yes count=6",  CTEST_POSTCOMMIT}
    ,
    {"DNX_field_exem_2_traffic", "stage=ipmf2 traffic=yes count=6",  SH_CMD_SKIP_EXEC}
    ,
    {"DNX_field_exem_3_traffic", "stage=ipmf3 traffic=yes count=6",  CTEST_POSTCOMMIT}
    ,
    {"DNX_field_exem_e_traffic", "stage=epmf traffic=yes count=6",   CTEST_POSTCOMMIT}
    ,
    {NULL}
};
/**
 * \brief
 *   Options list for 'exem' shell command
 */
sh_sand_option_t Sh_dnx_field_exem_options[] = {
     /* Name */                              /* Type */              /* Description */                          /* Default */
    {DNX_DIAG_EXEM_OPTION_TEST_STAGE,      SAL_FIELD_TYPE_ENUM,    "Stage of test (ipmf1,ipmf2,ipmf3,epmf)",     "ipmf1", (void *)Field_stage_enum_table},
    {DNX_CTEST_EXEM_OPTION_TEST_TRAFFIC,    SAL_FIELD_TYPE_BOOL,   "Will the test indlude traffic test or not",  "No"},
    {DNX_CTEST_EXEM_OPTION_TEST_CLEAN,      SAL_FIELD_TYPE_BOOL,   "Will the test perform clean-up or not",      "Yes"},
    {DNX_DIAG_EXEM_OPTION_TEST_COUNT,      SAL_FIELD_TYPE_UINT32,  "Number of times test will run",              "1"},
    {NULL}      /* End of options list - must be last. */
};
/* *INDENT-ON* */
/*
 * }
 */

/**
 *  The description of the ctest options.
 */
sh_sand_man_t Sh_dnx_field_exem_man = {
    "'EXEM' related test utilities",
    "Activate 'exem' related test utilities.\r\n" "creates EXEM groups and entries.\r\n"
        "Stage performs the test for a specific PMF stage (default is iPMF1).\r\n"
        "Count indicates the number of times the test will run.\r\n",
    "ctest field exem stage=<ipmf1 | ipmf2 | ipmf3 | epmf> traffic=<Yes | No> clean=<Yes | No>",
    "traffic=Yes\n" "stage=ipmf1",
};

/** The number of qualifiers used in the test.*/
#define CTEST_DNX_FIELD_EXEM_NUM_QUALS        2
/** The number of actions used in the test.*/
#define CTEST_DNX_FIELD_EXEM_NUM_ACTIONS      2
/** The number of stages used in the test. Used for array size.*/
#define CTEST_DNX_FIELD_EXEM_NUM_STAGES       4
/** The priority of the actions in the field group.*/
#define CTEST_DNX_FIELD_EXEM_ACTION_PRIORITY  (BCM_FIELD_ACTION_PRIORITY(0, 3))
/**
 * Number of retries to apply if, after sending a packet, signals were not yet received.
 * Time delay, in seconds, between retries, is CTEST_DNX_FIELD_EXEM_PACKET_WAIT.
 */
#define CTEST_DNX_FIELD_EXEM_PACKET_RETRY     1
/**
 * For each retry (as seen above in CTEST_DNX_FIELD_EXEM_PACKET_RETRY), how many seconds to wait.
 */
#define CTEST_DNX_FIELD_EXEM_PACKET_WAIT      5

/** The DNX_QUAL to be used for aech stage.*/
static dnx_field_qual_t Dnx_quals_array[CTEST_DNX_FIELD_EXEM_NUM_STAGES][CTEST_DNX_FIELD_EXEM_NUM_QUALS] = {
    {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_HEADER_QUAL_IPV4_SRC),
     DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_HEADER_QUAL_IPV4_DST)},
    {DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF1_QUAL_SNOOP_STRENGTH),
     DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_PMF1_EXEM_ACTION)},
    {DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FWD_ACTION_DP),
     DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_WEAK_TM_PROFILE)},
    {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_EPMF, DNX_FIELD_HEADER_QUAL_IPV4_SRC),
     DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_EPMF, DNX_FIELD_HEADER_QUAL_IPV4_DST)},
};

/** The qualifier input type to be used for each stage.*/
static dnx_field_input_type_e Quals_input_type[CTEST_DNX_FIELD_EXEM_NUM_STAGES][CTEST_DNX_FIELD_EXEM_NUM_QUALS] = {
    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE, DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE},
    {DNX_FIELD_INPUT_TYPE_META_DATA, DNX_FIELD_INPUT_TYPE_META_DATA2},
    {DNX_FIELD_INPUT_TYPE_META_DATA, DNX_FIELD_INPUT_TYPE_META_DATA},
    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE, DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE},
};

/** The qualifier input argument to be used for each stage.*/
static dnx_field_input_type_e Quals_input_arg[CTEST_DNX_FIELD_EXEM_NUM_STAGES][CTEST_DNX_FIELD_EXEM_NUM_QUALS] = {
    {1, 1},
    {0, 0},
    {0, 0},
    {1, 1},
};

/** The qualifier offset to be used for each stage.*/
static dnx_field_input_type_e Quals_offset[CTEST_DNX_FIELD_EXEM_NUM_STAGES][CTEST_DNX_FIELD_EXEM_NUM_QUALS] = {
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
};

/** The values to write in the qualifiers for the entry. at the moment only iPMF1 checks actual data from the packet.*/
static uint32 Quals_values[CTEST_DNX_FIELD_EXEM_NUM_STAGES][CTEST_DNX_FIELD_EXEM_NUM_QUALS] = {
    {1, 2},
    {0, 0},
    {0, 0},
    {1, 2}
};

/** The DNX_ACTION to be used for each stage.*/
static dnx_field_action_t Dnx_actions_array[CTEST_DNX_FIELD_EXEM_NUM_STAGES][CTEST_DNX_FIELD_EXEM_NUM_ACTIONS] = {
    {DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1,
                DBAL_ENUM_FVAL_IPMF1_ACTION_TC),
     DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1,
                DBAL_ENUM_FVAL_IPMF1_ACTION_DP)},
    {DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF2,
                DBAL_ENUM_FVAL_IPMF1_ACTION_TC),
     DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF2,
                DBAL_ENUM_FVAL_IPMF1_ACTION_DP)},
    {DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF3,
                DBAL_ENUM_FVAL_IPMF3_ACTION_TC),
     DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF3,
                DBAL_ENUM_FVAL_IPMF3_ACTION_DP)},
    {DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_EPMF,
                DBAL_ENUM_FVAL_EPMF_ACTION_TC),
     DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_EPMF,
                DBAL_ENUM_FVAL_EPMF_ACTION_DP)},

};

/** The values to write in the actions for the entry. */
static uint32 Actions_values[CTEST_DNX_FIELD_EXEM_NUM_STAGES][CTEST_DNX_FIELD_EXEM_NUM_QUALS] = {
    {1, 2},
    {1, 2},
    {1, 2},
    {1, 2}
};

/*
 * An array, which contains all needed signal information for the traffic testing:
 * core, block, from, to, signal name, size of the buffer and expected value (the last unsused).
 */
static dnx_field_utils_signal_info_t Packet_sig_info[CTEST_DNX_FIELD_EXEM_NUM_STAGES] = {
/*
 * Fields are: Core, Block, From, to, signal name, size of buffer, expected value. 
 * Exepected value isn't taken from here, and is merely initialized.
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
 *   This function sends a packet and checkes the signals.
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
ctest_dnx_exem_traffic_test(
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
            sal_sleep(CTEST_DNX_FIELD_EXEM_PACKET_WAIT);
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
    while (num_retries < CTEST_DNX_FIELD_EXEM_PACKET_RETRY + 1 && rv == _SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT_WITH_LOG(rv, "Field EXEM traffic test failed. Result of signal verify is %s.%s%s",
                             return_value, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks that two dnx_field_entry_t structures areidentical.
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
ctest_dnx_exem_entry_cmp(
    int unit,
    dnx_field_entry_t * entry_info_in,
    dnx_field_entry_t * entry_info_out)
{
    unsigned int qual_ndx;
    unsigned int action_ndx;
    unsigned int qual_param_ndx;

    SHR_FUNC_INIT_VARS(unit);

    for (qual_ndx = 0; qual_ndx < CTEST_DNX_FIELD_EXEM_NUM_QUALS; qual_ndx++)
    {
        if (entry_info_in->key_info.qual_info[qual_ndx].dnx_qual !=
            entry_info_out->key_info.qual_info[qual_ndx].dnx_qual)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Qualifier %d returned from entry get differently from what was to be written.\r\n", qual_ndx);
        }
        
        for (qual_param_ndx = 0; qual_param_ndx < 1; qual_param_ndx++)
        {
            if (entry_info_in->key_info.qual_info[qual_ndx].qual_value[qual_param_ndx] !=
                entry_info_out->key_info.qual_info[qual_ndx].qual_value[qual_param_ndx])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Qualifier %d returned from entry get differently from what was to be written.\r\n",
                             qual_ndx);
            }
        }
    }
    for (action_ndx = 0; action_ndx < CTEST_DNX_FIELD_EXEM_NUM_ACTIONS; action_ndx++)
    {
        if (entry_info_in->payload_info.action_info[action_ndx].dnx_action !=
            entry_info_out->payload_info.action_info[action_ndx].dnx_action
            || entry_info_in->payload_info.action_info[action_ndx].action_value[0] !=
            entry_info_out->payload_info.action_info[action_ndx].action_value[0])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Action %d returned from get differently from what was to be written.\r\n", action_ndx);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Performs operational testing for the EXEM related code in the field group and field entry modules.
 *    Creates an EXEM field group and an EXEM entry.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] field_stage -
 *    For which the EXEM field group is to be created and tested (IPMF1, IPMF2, IPMF3, EPMF).
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \param [in] exem_test_traffic -
 *    Boolean, if true performs traffic test.
 * \param [in] exem_test_clean -
 *    Boolean, if true deletes the configuration performed by the test
 * \remark
 *   * The procedure uses 'Dnx_quals_input_type', 'Dnx_quals_array' and 'Dnx_actions_array' as input for the 
 *     field group.
 *   * The procedure uses 'Packet_sig_info' and 'Packet_info' for packet information for traffic test.
 * \see
 *   * appl_dnx_exem_starter
 */
static shr_error_e
appl_dnx_exem_tester(
    int unit,
    dnx_field_stage_e field_stage,
    int exem_test_traffic,
    int exem_test_clean)
{
    dnx_field_group_t fg_id;
    dnx_field_context_t context_id;
    dnx_field_group_add_flags_e fg_flags;
    dnx_field_group_info_t fg_info_in;
    dnx_field_group_full_info_t fg_info_out;
    dnx_field_entry_t entry_info_in;
    dnx_field_entry_t entry_info_out;
    dnx_field_group_attach_info_t attach_info_in;
    dnx_field_group_context_full_info_t attach_info_out;
    dbal_enum_value_field_field_io_e expected_field_io;
    dnx_field_action_length_type_e expected_action_length_type;
    unsigned int stage_ndx;
    unsigned int qual_ndx;
    unsigned int action_ndx;
    dnx_field_group_t fg_id_fec;
    dnx_field_group_info_t fg_info_fec;
    dnx_field_entry_t entry_info_fec;
    dnx_field_group_attach_info_t attach_info_fec;
    bsl_severity_t original_severity_fldprocdnx;
    bsl_severity_t original_severity_dbaldnx;
    uint32 entry_handle_fec;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the severity level. We silence the error messages for the negative cases.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);

    /*
     * Find the index of the stage for selecting qualifiers and actions.
     * Also fill the information regarding whether we expect it to be LEXEM or SEXEM,
     * by filling the 'action_length_type' and 'field_io' that we expect to get from  
     * dnx_field_group_get() and dnx_field_group_context_get().
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            stage_ndx = 0;
            expected_field_io = DBAL_ENUM_FVAL_FIELD_IO_LEXEM;
            expected_action_length_type = DNX_FIELD_ACTION_LENGTH_TYPE_LEXEM;
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            stage_ndx = 1;
            expected_field_io = DBAL_ENUM_FVAL_FIELD_IO_SEXEM;
            expected_action_length_type = DNX_FIELD_ACTION_LENGTH_TYPE_SEXEM;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            stage_ndx = 2;
            expected_field_io = DBAL_ENUM_FVAL_FIELD_IO_SEXEM;
            expected_action_length_type = DNX_FIELD_ACTION_LENGTH_TYPE_SEXEM;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            stage_ndx = 3;
            expected_field_io = DBAL_ENUM_FVAL_FIELD_IO_LEXEM;
            expected_action_length_type = DNX_FIELD_ACTION_LENGTH_TYPE_LEXEM;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid stage (%d).\r\n", field_stage);
            break;
        }
    }

    /*
     * Set the context ID to be the default context ID.
     */
    context_id = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);

    /*
     * Create an EXEM field group.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &fg_info_in));
    fg_info_in.fg_type = DNX_FIELD_GROUP_TYPE_EXEM;
    fg_info_in.field_stage = field_stage;
    for (qual_ndx = 0; qual_ndx < CTEST_DNX_FIELD_EXEM_NUM_QUALS; qual_ndx++)
    {
        fg_info_in.dnx_quals[qual_ndx] = Dnx_quals_array[stage_ndx][qual_ndx];
    }
    for (action_ndx = 0; action_ndx < CTEST_DNX_FIELD_EXEM_NUM_ACTIONS; action_ndx++)
    {
        
        fg_info_in.dnx_actions[action_ndx] = Dnx_actions_array[stage_ndx][action_ndx];
    }
    sal_strncpy(fg_info_in.name, "EXEM_CTEST", sizeof(fg_info_in.name));
    fg_flags = 0;
    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, fg_flags, &fg_info_in, &fg_id));

    /*
     * Create an entry.
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info_in));
    for (qual_ndx = 0; qual_ndx < CTEST_DNX_FIELD_EXEM_NUM_QUALS; qual_ndx++)
    {
        entry_info_in.key_info.qual_info[qual_ndx].dnx_qual = Dnx_quals_array[stage_ndx][qual_ndx];
        entry_info_in.key_info.qual_info[qual_ndx].qual_value[0] = Quals_values[stage_ndx][qual_ndx];
    }
    for (action_ndx = 0; action_ndx < CTEST_DNX_FIELD_EXEM_NUM_ACTIONS; action_ndx++)
    {
        entry_info_in.payload_info.action_info[action_ndx].dnx_action = Dnx_actions_array[stage_ndx][action_ndx];
        entry_info_in.payload_info.action_info[action_ndx].action_value[0] = Actions_values[stage_ndx][action_ndx];
    }
    SHR_IF_ERR_EXIT(dnx_field_entry_exem_add(unit, 0, fg_id, &entry_info_in));

    /*
     * Attach group to context.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info_in));
    for (qual_ndx = 0; qual_ndx < CTEST_DNX_FIELD_EXEM_NUM_QUALS; qual_ndx++)
    {
        attach_info_in.dnx_quals[qual_ndx] = Dnx_quals_array[stage_ndx][qual_ndx];
        attach_info_in.qual_info[qual_ndx].input_type = Quals_input_type[stage_ndx][qual_ndx];
        attach_info_in.qual_info[qual_ndx].input_arg = Quals_input_arg[stage_ndx][qual_ndx];
        attach_info_in.qual_info[qual_ndx].offset = Quals_offset[stage_ndx][qual_ndx];
    }
    for (action_ndx = 0; action_ndx < CTEST_DNX_FIELD_EXEM_NUM_ACTIONS; action_ndx++)
    {
        
        attach_info_in.dnx_actions[action_ndx] = Dnx_actions_array[stage_ndx][action_ndx];
        attach_info_in.action_info[action_ndx].priority = CTEST_DNX_FIELD_EXEM_ACTION_PRIORITY;
    }
    SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info_in));

    /*
     * Get the group and verify that it returns what we wrote.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id, &fg_info_out));
    if (fg_info_out.group_full_info.action_length_type != expected_action_length_type)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Group get returned with action_length_type %d, expected %d.\r\n",
                     fg_info_out.group_full_info.action_length_type, expected_action_length_type);
    }
    if (fg_info_in.field_stage != fg_info_out.group_basic_info.field_stage
        || fg_info_in.fg_type != fg_info_out.group_basic_info.fg_type
        || fg_flags != fg_info_out.group_full_info.flags
        || fg_info_in.field_stage != fg_info_out.group_full_info.field_stage
        || fg_info_in.fg_type != fg_info_out.group_full_info.fg_type)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Returned values from group get different from what was to be written.\r\n");
    }
    for (qual_ndx = 0; qual_ndx < CTEST_DNX_FIELD_EXEM_NUM_QUALS; qual_ndx++)
    {
        if (fg_info_in.dnx_quals[qual_ndx] != fg_info_out.group_basic_info.dnx_quals[qual_ndx]
            || fg_info_in.dnx_quals[qual_ndx] !=
            fg_info_out.group_full_info.key_template.key_qual_map[qual_ndx].qual_type)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Qualifier %d returned from group get differently from what was to be written.\r\n", qual_ndx);
        }
    }
    for (action_ndx = 0; action_ndx < CTEST_DNX_FIELD_EXEM_NUM_ACTIONS; action_ndx++)
    {
        
        if (fg_info_in.dnx_actions[action_ndx] != fg_info_out.group_basic_info.dnx_actions[action_ndx]
            || fg_info_in.dnx_actions[action_ndx] !=
            fg_info_out.group_full_info.actions_payload_info.actions_on_payload_info[action_ndx].dnx_action)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Action %d returned from group get differently from what was to be written.\r\n", action_ndx);
        }
    }

    /*
     * Get the entry and verify that it returns what we wrote.
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_exem_get(unit, fg_id, &(entry_info_in.key_info), &entry_info_out));
    SHR_IF_ERR_EXIT(ctest_dnx_exem_entry_cmp(unit, &entry_info_in, &entry_info_out));

    /*
     * Get the context attach and verify that it returns what we wrote.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id, &attach_info_out));
    if (attach_info_out.field_io != expected_field_io)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Context attach get returned with field_io %d, expected %d.\r\n",
                     attach_info_out.field_io, expected_field_io);
    }
    for (qual_ndx = 0; qual_ndx < CTEST_DNX_FIELD_EXEM_NUM_QUALS; qual_ndx++)
    {
        if (attach_info_in.dnx_quals[qual_ndx] != attach_info_out.attach_basic_info.dnx_quals[qual_ndx])
            
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Qualifier %d returned from context get differently from what was to be written.\r\n",
                         qual_ndx);
        }
    }
    for (action_ndx = 0; action_ndx < CTEST_DNX_FIELD_EXEM_NUM_ACTIONS; action_ndx++)
    {
        
        if (attach_info_in.dnx_actions[action_ndx] != attach_info_out.attach_basic_info.dnx_actions[action_ndx]
            || attach_info_in.action_info[action_ndx].priority !=
            attach_info_out.attach_basic_info.action_info[action_ndx].priority)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Action %d returned from get differently from what was to be written.\r\n", action_ndx);
        }
    }

    /*
     * Delete the entry using delete and verify that it is deleted.
     * Then rewrite the entry and verify that it is rewritten.
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_exem_delete(unit, fg_id, &(entry_info_in.key_info)));
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal);
    rv = dnx_field_entry_exem_get(unit, fg_id, &(entry_info_in.key_info), &entry_info_out);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Entry not deleted by dnx_field_entry_exem_delete.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_entry_exem_add(unit, 0, fg_id, &entry_info_in));
    SHR_IF_ERR_EXIT(dnx_field_entry_exem_get(unit, fg_id, &(entry_info_in.key_info), &entry_info_out));
    SHR_IF_ERR_EXIT(ctest_dnx_exem_entry_cmp(unit, &entry_info_in, &entry_info_out));

    /*
     * Delete the entry using delete_all and verify that it is deleted.
     * Then rewrite the entry and verify that it is rewritten.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_entry_delete_all(unit, fg_id));
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal);
    rv = dnx_field_entry_exem_get(unit, fg_id, &(entry_info_in.key_info), &entry_info_out);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Entry not deleted by dnx_field_group_entry_delete_all.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_entry_exem_add(unit, 0, fg_id, &entry_info_in));
    SHR_IF_ERR_EXIT(dnx_field_entry_exem_get(unit, fg_id, &(entry_info_in.key_info), &entry_info_out));
    SHR_IF_ERR_EXIT(ctest_dnx_exem_entry_cmp(unit, &entry_info_in, &entry_info_out));

    /*
     * Perform traffic test if required by 'traffic' option .
     */
    if (exem_test_traffic)
    {
        /*
         * If we are in ePMF, configure field group in iPMF1 to get the packet passed TM.
         */
        if (field_stage == DNX_FIELD_STAGE_EPMF)
        {
            SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &fg_info_fec));
            fg_info_fec.fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
            fg_info_fec.field_stage = DNX_FIELD_STAGE_IPMF1;
            fg_info_fec.dnx_quals[0] =
                DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_FWD_CONTEXT);
            fg_info_fec.dnx_actions[0] =
                DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1, DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA);
            SHR_IF_ERR_EXIT(dnx_field_group_add(unit, 0, &fg_info_fec, &fg_id_fec));
            SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info_fec));
            attach_info_fec.dnx_quals[0] =
                DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_FWD_CONTEXT);
            attach_info_fec.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
            attach_info_fec.qual_info[0].input_arg = 0;
            attach_info_fec.qual_info[0].offset = 0;
            attach_info_fec.dnx_actions[0] =
                DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1, DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA);
            attach_info_fec.action_info[0].priority = CTEST_DNX_FIELD_EXEM_ACTION_PRIORITY;
            SHR_IF_ERR_EXIT(dnx_field_group_context_attach
                            (unit, 0, fg_id_fec, DNX_FIELD_CONTEXT_ID_DEFAULT(unit), &attach_info_fec));
            SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info_fec));

            entry_info_fec.priority = 0;
            entry_info_fec.key_info.qual_info[0].dnx_qual =
                DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_FWD_CONTEXT);
            entry_info_fec.key_info.qual_info[0].qual_value[0] = 0;
            entry_info_fec.key_info.qual_info[0].qual_mask[0] = 0;
            entry_info_fec.payload_info.action_info[0].dnx_action =
                DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1, DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA);
            entry_info_fec.payload_info.action_info[0].action_value[0] = 0xc00c9;
            SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, fg_id_fec, &entry_info_fec, &entry_handle_fec));
        }
        /*
         * Create the packet and test the resulting signal.
         */
        SHR_IF_ERR_EXIT(ctest_dnx_exem_traffic_test(unit, field_stage, Actions_values[stage_ndx][0]));
    }

    /*
     * Perform clean-up, if required by 'clean' option.
     */
    if (exem_test_clean)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_context_detach(unit, fg_id, context_id));
        SHR_IF_ERR_EXIT(dnx_field_entry_exem_delete(unit, fg_id, &(entry_info_in.key_info)));
        SHR_IF_ERR_EXIT(dnx_field_group_delete(unit, fg_id));
        /*
         * If we created a field group to get to ePMF, delete it.
         */
        if (exem_test_traffic && field_stage == DNX_FIELD_STAGE_EPMF)
        {
            SHR_IF_ERR_EXIT(dnx_field_entry_tcam_delete(unit, fg_id_fec, entry_handle_fec, NULL));
            SHR_IF_ERR_EXIT(dnx_field_group_context_detach(unit, fg_id_fec, DNX_FIELD_CONTEXT_ID_DEFAULT(unit)));
            SHR_IF_ERR_EXIT(dnx_field_group_delete(unit, fg_id_fec));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is the basic 'exem' testing
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
appl_dnx_exem_starter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e field_stage;
    int exem_test_traffic;
    int exem_test_clean;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_ENUM(DNX_DIAG_EXEM_OPTION_TEST_STAGE, field_stage);
    SH_SAND_GET_BOOL(DNX_CTEST_EXEM_OPTION_TEST_TRAFFIC, exem_test_traffic);
    SH_SAND_GET_BOOL(DNX_CTEST_EXEM_OPTION_TEST_CLEAN, exem_test_clean);

    SHR_IF_ERR_EXIT(appl_dnx_exem_tester(unit, field_stage, exem_test_traffic, exem_test_clean));

exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief - run 'exem' sequence in diag shell
 */
shr_error_e
sh_dnx_field_exem_cmd(
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
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));
        SHR_IF_ERR_EXIT(appl_dnx_exem_starter(unit, args, sand_control));
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

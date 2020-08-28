/** \file ctest_dnx_field_ace.c
 * $Id$
 *
 * 'ACE' operations (for format and entry add and for FES configuration) procedures for DNX.
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
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include "ctest_dnx_field_ace.h"
#include "ctest_dnx_field_utils.h"
#include <bcm_int/dnx/field/field_actions.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/sand/sand_signals.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * The various values available for 'general_test_type' input.
 */
#define TEST_TYPE_FORMAT "FoRMat"
#define TEST_TYPE_FES_CFG   "FES_CFG"

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
 *   Keyword for test type on database command (data base testing)
 */
#define DNX_CTEST_ACE_OPTION_TEST_TYPE         "type"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define DNX_CTEST_ACE_OPTION_TEST_COUNT        "count"
/**
 * \brief
 *   Keyword for controlling whether to perform tracfic test. can be either 0 or 1.
 *   Only relevant for test type FORMAT.
 */
#define DNX_CTEST_ACE_OPTION_TEST_TRAFFIC      "traffic"
/**
 * \brief
 *   Keyword for controlling whether to update the entry. can be either 0 or 1.
 *   Only relevant for test type FORMAT.
 */
#define DNX_CTEST_ACE_OPTION_TEST_UPDATE      "update"
/**
 * \brief
 *   Keyword for controlling the context used.
 *   Only relevant for test type FORMAT.
 */
#define DNX_CTEST_ACE_OPTION_TEST_CONTEXT      "context"

/**
 * \brief
 *   Keyword for controlling whether to release resources at end of test.
 *   clean can be either 0 or 1, if the ctest will clean-up after its finished setting-up.
 *   Only relevant for test type FORMAT.
 */
#define DNX_CTEST_ACE_OPTION_TEST_CLEAN        "clean"

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
 *   List of tests for 'ACE' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_ace_tests[] = {
    {"DNX_ace_for_FES_cfg",                  "type=fes_cfg count=6",             CTEST_POSTCOMMIT}
    ,
    {"DNX_ace_for_format_entry_add",         "type=format count=25",              CTEST_POSTCOMMIT}
    ,
    {"DNX_ace_for_format_entry_add_traffic", "type=format traffic=yes count=1",  CTEST_POSTCOMMIT}
    ,
    {NULL}
};
/**
 * \brief
 *   Options list for 'ACE' shell command
 */
sh_sand_option_t Sh_dnx_field_ace_options[] = {
    /* Name */                               /* Type */              /* Description */                      /* Default */
    {DNX_CTEST_ACE_OPTION_TEST_TYPE,       SAL_FIELD_TYPE_STR,     "Type of test (format or fes_cfg)",            NULL},
    {DNX_CTEST_ACE_OPTION_TEST_TRAFFIC,    SAL_FIELD_TYPE_BOOL,    "Will the test include traffic test or not",  "No"},
    {DNX_CTEST_ACE_OPTION_TEST_UPDATE,     SAL_FIELD_TYPE_BOOL,    "Will the test update an entry or not",       "Yes"},
    {DNX_CTEST_ACE_OPTION_TEST_CONTEXT,    SAL_FIELD_TYPE_UINT32,  "The context ID for iPMF1 and ePMF",          "0"},
    {DNX_CTEST_ACE_OPTION_TEST_CLEAN,      SAL_FIELD_TYPE_BOOL,    "Will the test perform clean-up or not",      "Yes"},
    {DNX_CTEST_ACE_OPTION_TEST_COUNT,      SAL_FIELD_TYPE_UINT32,  "Number of times test will run",              "1"},
    {NULL}      /* End of options list - must be last. */
};
/* *INDENT-ON* */
/*
 * }
 */

/**
 *  Details for test options.
 */
sh_sand_man_t Sh_dnx_field_ace_man = {
    "'ACE' related test utilities",
    "Activate 'ACE' related test utilities.\r\n"
        "Type 'FoRMat' created an ace format, entry, and action in ePMF accessing int.\r\n"
        " - 'TRAffic' also performs traffic test.\r\n"
        " - 'CLeaN' also deletes the configuration afterwards.\r\n"
        "Type 'FES_CFG' only deals with FES configuration in the ACE ACR.\r\n"
        " - Parameters 'TRAffic' and 'CLeaN' have no effect on type 'FES_CFG',\r\n"
        "   it always performs clean-up and never sends any traffic.",
    "ctest field ace type=<FORMAT | FES_CFG> traffic=<Yes | No> clean=<Yes | No>",
    "type=format traffic=Yes\n" "TY=FES_CFG",
};

/** The value to write to the actions in the entry.*/
#define CTEST_DNX_FIELD_ACE_ACTION_VALUE             50
/** The value to write to the actions in the entry after update.*/
#define CTEST_DNX_FIELD_ACE_UPDATED_ACTION_VALUE     60
/** 
 * Number of retries to apply if, after sending a packet, signals were not yet received.
 * Time delay, in seconds, between retries, is CTEST_DNX_FIELD_ACE_PACKET_WAIT.
 */
#define CTEST_DNX_FIELD_ACE_PACKET_RETRY     0
/** 
 * For each retry (as seen above in CTEST_DNX_FIELD_ACE_PACKET_RETRY), how many seconds to wait.
 */
#define CTEST_DNX_FIELD_ACE_PACKET_WAIT      5

/*
 * An array, which contains all needed signal information for the traffic testing:
 * core, block, from, to, signal name, size of the buffer and
 * to match on the qualifier. 
 * Expected value isn't taken from here, and is merely initialized.
 */
#ifdef ADAPTER_SERVER_MODE
static dnx_field_utils_signal_info_t Packet_sig_info = { 0, "ETPP", "Term", "", "PPH_TTL", 3, 0 };
#else
static dnx_field_utils_signal_info_t Packet_sig_info = { 0, "ETPP", "PRP2", "Term", "PPH_TTL", 3, 0 };
#endif

/* The packet to be sent. */
static dnx_field_utils_packet_info_t Packet_info = {
    {"PTCH_2", "PTCH_2.PP_SSP", "27", "ETH1", "IPv4", "IPv4.SIP", "0.0.0.2", "IPv4.DIP", "0.0.0.1", ""}
};

/**
 * \brief
 *    Performs operational testing for the code regarding the ACE table in the field group module and entry module.
 *    Tests:
 *    * dnx_field_ace_format_add()
 *    * dnx_field_ace_format_get()
 *    * dnx_field_ace_format_delete()
 *    * dnx_field_ace_entry_add()
 *    * dnx_field_ace_entry_get()
 *    * dnx_field_ace_entry_delete()
 *    * Using ePMF to perform lookup in ACE table.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] ace_test_context -
 *    The context ID to use.
 * \param [in] ace_test_traffic -
 *    Boolean, if true performs traffic test.
  * \param [in] ace_test_traffic -
 *    Boolean, if true updates an entry.
 * \param [in] ace_test_clean -
 *    Boolean, if true deletes the configuration performed by the test
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * ctest_dnx_ace_starter
 */
static shr_error_e
ctest_dnx_ace_tester_format(
    int unit,
    int ace_test_context,
    int ace_test_traffic,
    int ace_test_update,
    int ace_test_clean)
{
    dnx_field_ace_id_t ace_id;
    dnx_field_ace_id_t ace_id_out;
    dnx_field_ace_id_t ace_id_2;
    dnx_field_ace_id_t ace_id_3;
    dnx_field_ace_format_info_t ace_format_info;
    dnx_field_ace_format_info_t ace_format_info_2;
    dnx_field_ace_format_info_t ace_format_info_3;
    dnx_field_ace_format_add_flags_e ace_format_flags;
    uint32 ace_entry_flags;
    dnx_field_entry_payload_t payload_info_in;
    dnx_field_entry_payload_t payload_info_out;
    dnx_field_ace_format_full_info_t ace_id_info;
    dnx_field_ace_key_t ace_entry_key;
    dnx_field_group_info_t fg_info;
    dnx_field_group_t fg_id_egress;
    dnx_field_group_attach_info_t attach_info;
    dnx_field_entry_t tcam_entry_info;
    uint32 tcam_entry_handle_egress;
    uint8 context_existed_ipmf1;
    uint8 context_existed_epmf;
    dnx_field_context_t context_id;
    dnx_field_context_mode_t context_mode;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create the context if it doesn't exist.
     */
    context_id = ace_test_context;
    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
    SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated
                    (unit, DNX_FIELD_STAGE_IPMF1, context_id, &context_existed_ipmf1));
    if (context_existed_ipmf1 == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_create
                        (unit, DNX_FIELD_CONTEXT_FLAG_WITH_ID, DNX_FIELD_STAGE_IPMF1, &context_mode, &context_id));
    }
    SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated
                    (unit, DNX_FIELD_STAGE_EPMF, context_id, &context_existed_epmf));
    if (context_existed_ipmf1 == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_create
                        (unit, DNX_FIELD_CONTEXT_FLAG_WITH_ID, DNX_FIELD_STAGE_EPMF, &context_mode, &context_id));
    }

    /*
     * Create a second ACE format just to test the effect of deleting ACE IDs on other ACE IDs.
     */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_info_t_init(unit, &ace_format_info_2));
    ace_format_info_2.dnx_actions[0] =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_ACE, DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TC);
    ace_format_flags = 0;
    SHR_IF_ERR_EXIT(dnx_field_ace_format_add(unit, ace_format_flags, &ace_format_info_2, &ace_id_2));

    /*
     * Create an ACE Format.
     */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_info_t_init(unit, &ace_format_info));

    /** Test the ACE Format name. */
    sal_strncpy(ace_format_info.name, "ACE_Format", DBAL_MAX_STRING_LENGTH);

    ace_format_info.dnx_actions[0] =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_ACE, DBAL_ENUM_FVAL_ACE_ACTION_PPH_TTL);
    ace_format_info.dnx_actions[1] =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_ACE, DBAL_ENUM_FVAL_ACE_ACTION_PP_DSP);
    ace_format_info.dnx_actions[2] =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_ACE, DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAMP_VALUE);
    ace_format_flags = 0;
    SHR_IF_ERR_EXIT(dnx_field_ace_format_add(unit, ace_format_flags, &ace_format_info, &ace_id));

    /*
     * Create a third ACE format just to test the effect of deleting ACE IDs on other ACE IDs.
     */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_info_t_init(unit, &ace_format_info_3));
    ace_format_info_3.dnx_actions[0] =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_ACE, DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TC);
    ace_format_flags = 0;
    SHR_IF_ERR_EXIT(dnx_field_ace_format_add(unit, ace_format_flags, &ace_format_info_3, &ace_id_3));

    /*
     * Delete the second ACE format.
     */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_delete(unit, ace_id_2));

    /*
     * Create an entry. Fill only part of the actions, leaving a gap in the middle.
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_payload_t_init(unit, &payload_info_in));
    payload_info_in.action_info[0].dnx_action = ace_format_info.dnx_actions[0];
    payload_info_in.action_info[0].action_value[0] = CTEST_DNX_FIELD_ACE_ACTION_VALUE;
    payload_info_in.action_info[1].dnx_action = ace_format_info.dnx_actions[2];
    payload_info_in.action_info[1].action_value[0] = CTEST_DNX_FIELD_ACE_ACTION_VALUE;
    ace_entry_flags = 0;
    SHR_IF_ERR_EXIT(dnx_field_ace_entry_add(unit, ace_entry_flags, ace_id, &payload_info_in, &ace_entry_key));

    /*
     * Get the ACE Format and and verify that it returns what we wrote.
     */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_get(unit, ace_id, &ace_id_info));
    if (ace_id_info.format_full_info.flags != ace_format_flags)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Returned flags from ACE format get different from what was to be written.\r\n");
    }
    if (ace_id_info.format_basic_info.dnx_actions[0] != ace_format_info.dnx_actions[0] ||
        ace_id_info.format_basic_info.dnx_actions[1] != ace_format_info.dnx_actions[1] ||
        ace_id_info.format_basic_info.dnx_actions[2] != ace_format_info.dnx_actions[2] ||
        ace_id_info.format_basic_info.dnx_actions[3] != DNX_FIELD_ACTION_INVALID ||
        ace_id_info.format_full_info.actions_payload_info.actions_on_payload_info[0].dnx_action !=
        ace_format_info.dnx_actions[0]
        || ace_id_info.format_full_info.actions_payload_info.actions_on_payload_info[1].dnx_action !=
        ace_format_info.dnx_actions[1]
        || ace_id_info.format_full_info.actions_payload_info.actions_on_payload_info[2].dnx_action !=
        ace_format_info.dnx_actions[2]
        || ace_id_info.format_full_info.actions_payload_info.actions_on_payload_info[3].dnx_action !=
        DNX_FIELD_ACTION_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Actions returned from ACE group get are different from what was to be written.\r\n");
    }

    /*
     * Get the entry and and verify that it returns what we wrote.
     */
    SHR_IF_ERR_EXIT(dnx_field_ace_entry_get(unit, ace_entry_key, &payload_info_out, &ace_id_out));
    if (payload_info_out.action_info[0].dnx_action != payload_info_in.action_info[0].dnx_action ||
        payload_info_out.action_info[0].action_value[0] != payload_info_in.action_info[0].action_value[0] ||
        payload_info_out.action_info[1].dnx_action != payload_info_in.action_info[1].dnx_action ||
        payload_info_out.action_info[1].action_value[0] != payload_info_in.action_info[1].action_value[0] ||
        payload_info_out.action_info[2].dnx_action != DNX_FIELD_ACTION_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Actions returned from entry get are different from what was to be written.\r\n");
    }
    if (ace_id_out != ace_id)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Entry (%d) uses ACE format Id %d instead of %d.\r\n",
                     ace_entry_key, ace_id_out, ace_id);
    }

    /*
     * Configure an ePMF field group with a catch all entry to use the ACE entry. Attach it to the default context.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &fg_info));
    fg_info.fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    fg_info.field_stage = DNX_FIELD_STAGE_EPMF;
    fg_info.dnx_quals[0] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_CNI);
    fg_info.dnx_actions[0] =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_EPMF,
                   DBAL_ENUM_FVAL_EPMF_ACTION_CUD_OUTLIF_OR_MCDB_PTR);
    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, 0, &fg_info, &fg_id_egress));
    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info));
    attach_info.dnx_quals[0] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_CNI);
    attach_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
    attach_info.qual_info[0].input_arg = 0;
    attach_info.qual_info[0].offset = 0;
    attach_info.dnx_actions[0] =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_EPMF,
                   DBAL_ENUM_FVAL_EPMF_ACTION_CUD_OUTLIF_OR_MCDB_PTR);
    attach_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 3);
    SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, fg_id_egress, context_id, &attach_info));
    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &tcam_entry_info));
    tcam_entry_info.priority = 0;
    tcam_entry_info.key_info.qual_info[0].dnx_qual =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_CNI);
    tcam_entry_info.key_info.qual_info[0].qual_value[0] = 0;
    tcam_entry_info.key_info.qual_info[0].qual_mask[0] = 0;
    tcam_entry_info.payload_info.action_info[0].dnx_action =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_EPMF,
                   DBAL_ENUM_FVAL_EPMF_ACTION_CUD_OUTLIF_OR_MCDB_PTR);
    tcam_entry_info.payload_info.action_info[0].action_value[0] = ace_entry_key;
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, fg_id_egress, &tcam_entry_info, &tcam_entry_handle_egress));

    /*
     * Perform traffic test if required by 'traffic' option .
     */
    if (ace_test_traffic)
    {
        uint32 expected_value = CTEST_DNX_FIELD_ACE_ACTION_VALUE;
        SHR_IF_ERR_EXIT(bcm_port_get(unit, 200, &flags, &interface_info, &mapping_info));
        sal_snprintf(Packet_info.header_info[2], DNX_FIELD_UTILS_STR_SIZE, "%d", mapping_info.pp_port);
        SHR_IF_ERR_EXIT(ctest_dnx_field_utils_packet_tx(unit, 200, &Packet_info));
        SHR_IF_ERR_EXIT(sand_signal_verify(unit, mapping_info.core, Packet_sig_info.block, Packet_sig_info.from,
                                           Packet_sig_info.to, Packet_sig_info.sig_name, &(expected_value),
                                           Packet_sig_info.size, NULL, NULL, 0));
    }

    if (ace_test_update)
    {
        /*
         * Update the entry. Change the action types.
         */
        payload_info_in.action_info[0].dnx_action = ace_format_info.dnx_actions[0];
        payload_info_in.action_info[0].action_value[0] = CTEST_DNX_FIELD_ACE_UPDATED_ACTION_VALUE;
        payload_info_in.action_info[1].dnx_action = ace_format_info.dnx_actions[1];
        payload_info_in.action_info[1].action_value[0] = CTEST_DNX_FIELD_ACE_UPDATED_ACTION_VALUE;
        payload_info_in.action_info[2].dnx_action = DNX_FIELD_ACTION_INVALID;
        ace_entry_flags = DNX_FIELD_ENTRY_ADD_FLAG_UPDATE;
        SHR_IF_ERR_EXIT(dnx_field_ace_entry_add(unit, ace_entry_flags, ace_id, &payload_info_in, &ace_entry_key));

        /*
         * Get the updated entry and verify it.
         */
        SHR_IF_ERR_EXIT(dnx_field_ace_entry_get(unit, ace_entry_key, &payload_info_out, &ace_id_out));
        if (payload_info_out.action_info[0].dnx_action != payload_info_in.action_info[0].dnx_action ||
            payload_info_out.action_info[0].action_value[0] != payload_info_in.action_info[0].action_value[0] ||
            payload_info_out.action_info[1].dnx_action != payload_info_in.action_info[1].dnx_action ||
            payload_info_out.action_info[1].action_value[0] != payload_info_in.action_info[1].action_value[0] ||
            payload_info_out.action_info[2].dnx_action != DNX_FIELD_ACTION_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Actions returned from entry get after update "
                         "are different from what was to be written.\r\n");
        }
        if (ace_id_out != ace_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Entry (%d) uses ACE format Id %d instead of %d.\r\n",
                         ace_entry_key, ace_id_out, ace_id);
        }

        /*
         * Perform traffic test again, if required by 'traffic' option .
         */
        if (ace_test_traffic)
        {
            uint32 expected_value = CTEST_DNX_FIELD_ACE_UPDATED_ACTION_VALUE;
            SHR_IF_ERR_EXIT(bcm_port_get(unit, 200, &flags, &interface_info, &mapping_info));
            sal_snprintf(Packet_info.header_info[2], DNX_FIELD_UTILS_STR_SIZE, "%d", mapping_info.pp_port);
            SHR_IF_ERR_EXIT(ctest_dnx_field_utils_packet_tx(unit, 200, &Packet_info));
            SHR_IF_ERR_EXIT(sand_signal_verify(unit, Packet_sig_info.core, Packet_sig_info.block, Packet_sig_info.from,
                                               Packet_sig_info.to, Packet_sig_info.sig_name, &(expected_value),
                                               Packet_sig_info.size, NULL, NULL, 0));
        }
    }

    /*
     * Delete the third ACE format.
     */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_delete(unit, ace_id_3));

    /*
     * Perform clean-up, if required by 'clean' option.
     */
    if (ace_test_clean)
    {
        /*
         * Delete the egress PMF configuration configuration.
         */
        SHR_IF_ERR_EXIT(dnx_field_entry_tcam_delete(unit, fg_id_egress, tcam_entry_handle_egress, NULL));
        SHR_IF_ERR_EXIT(dnx_field_group_context_detach(unit, fg_id_egress, context_id));
        SHR_IF_ERR_EXIT(dnx_field_group_delete(unit, fg_id_egress));

        /*
         * Delete the ACE entry.
         */
        SHR_IF_ERR_EXIT(dnx_field_ace_entry_delete(unit, ace_entry_key));
        /*
         * Delete the ACE group.
         */
        SHR_IF_ERR_EXIT(dnx_field_ace_format_delete(unit, ace_id));

        /*
         * Delete the context ID.
         */
        if (context_existed_ipmf1 == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, DNX_FIELD_STAGE_IPMF1, context_id));
        }
        if (context_existed_epmf == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, DNX_FIELD_STAGE_EPMF, context_id));
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Performs operational testing for the code regarding ACE table in the field action module.
 *    Tests:
 *    * dnx_field_actions_fes_ace_set()
 *    * dnx_field_actions_ace_id_fes_info_get()
 *    * dnx_field_actions_fes_ace_detach()
 * \param [in] unit -
 *    Identifier of HW platform.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * ctest_dnx_ace_starter
 */
static shr_error_e
ctest_dnx_ace_tester_fes_cfg(
    int unit)
{
    dnx_field_ace_id_t ace_id;
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    dnx_field_action_type_t action_type1, action_type2, action_type3;
    unsigned int valid_bits;
    unsigned int shift;
    unsigned int type;
    unsigned int polarity;
    unsigned int required_mask1, required_mask2, required_mask3;
    dnx_field_actions_fes_ace_get_info_t ace_id_fes_get_info;
    uint8 nof_masks_before[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT];
    uint8 nof_masks_after[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_field_fes_id_t fes_id_mask_1;
    dnx_field_fes_id_t fes_id_mask_2;
    dnx_field_fes_id_t fes_id_no_mask;
    dnx_field_fes_id_t fes_id;
    unsigned int loc_no_mask;
    unsigned int loc_mask_1;
    unsigned int loc_mask_2;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure parameter, including three field groups with the same two actions. 
     */
    action_type1 = DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TC;
    action_type2 = DBAL_ENUM_FVAL_ACE_ACTION_FTMH_DP;
    action_type3 = DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TM_ACTION_TYPE;
    ace_id = 7;
    valid_bits = 1;
    shift = 2;
    type = 0;
    polarity = 1;
    /*
     * Note that since the first action has no mask, it would be allocated last (unless masks appear beforehand).
     */
    required_mask1 = 0;
    required_mask2 = 3;
    required_mask3 = 5;
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_info_t_init(unit, DNX_FIELD_STAGE_ACE, fes_inst_info));
    fes_inst_info[0].common_info.fes2msb_info[0].action_type = action_type1;
    fes_inst_info[0].common_info.fes2msb_info[0].valid_bits = valid_bits;
    fes_inst_info[0].common_info.fes2msb_info[0].shift = shift;
    fes_inst_info[0].common_info.fes2msb_info[0].type = type;
    fes_inst_info[0].common_info.fes2msb_info[0].polarity = polarity;
    fes_inst_info[0].alloc_info.fes2msb_info[0].required_mask = required_mask1;
    fes_inst_info[1].common_info.fes2msb_info[0].action_type = action_type2;
    fes_inst_info[1].common_info.fes2msb_info[0].valid_bits = valid_bits;
    fes_inst_info[1].common_info.fes2msb_info[0].shift = shift;
    fes_inst_info[1].common_info.fes2msb_info[0].type = type;
    fes_inst_info[1].common_info.fes2msb_info[0].polarity = polarity;
    fes_inst_info[1].alloc_info.fes2msb_info[0].required_mask = required_mask2;
    fes_inst_info[2].common_info.fes2msb_info[0].action_type = action_type3;
    fes_inst_info[2].common_info.fes2msb_info[0].valid_bits = valid_bits;
    fes_inst_info[2].common_info.fes2msb_info[0].shift = shift;
    fes_inst_info[2].common_info.fes2msb_info[0].type = type;
    fes_inst_info[2].common_info.fes2msb_info[0].polarity = polarity;
    fes_inst_info[2].alloc_info.fes2msb_info[0].required_mask = required_mask3;

    /*
     * Verify that the FES configuration for the ACE ID is empty
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_ace_id_fes_info_get(unit, ace_id, &ace_id_fes_get_info));
    if (ace_id_fes_get_info.nof_fes_instr != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test assumes that nothing was configured before for ACE ID %d.\r\n",
                     ace_id);
    }

    /*
     * Get the current mask usage.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_ace_id_nof_mask_state_get(unit, nof_masks_before));
    /*
     * Find the expected FES ID allocation.
     */
    for (fes_id_mask_1 = 0; fes_id_mask_1 < dnx_data_field.ace.nof_fes_instruction_per_context_get(unit);
         fes_id_mask_1++)
    {
        if (nof_masks_before[fes_id_mask_1] < (dnx_data_field.ace.nof_masks_per_fes_get(unit) - 1))
        {
            break;
        }
    }
    if (fes_id_mask_1 >= dnx_data_field.ace.nof_fes_instruction_per_context_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No EFES action masks left for allocation.\r\n");
    }
    for (fes_id_mask_2 = fes_id_mask_1 + 1;
         fes_id_mask_2 < dnx_data_field.ace.nof_fes_instruction_per_context_get(unit); fes_id_mask_2++)
    {
        if (nof_masks_before[fes_id_mask_2] < (dnx_data_field.ace.nof_masks_per_fes_get(unit) - 1))
        {
            break;
        }
    }
    if (fes_id_mask_2 >= dnx_data_field.ace.nof_fes_instruction_per_context_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Only one EFES action mask left for allocation.\r\n");
    }
    if (fes_id_mask_1 != 0)
    {
        fes_id_no_mask = 0;
    }
    else if (fes_id_mask_2 != 1)
    {
        fes_id_no_mask = 1;
    }
    else
    {
        fes_id_no_mask = 2;
    }
    /*
     * Find the order by which the actions appear.
     */
    if (fes_id_no_mask < fes_id_mask_1)
    {
        loc_no_mask = 0;
        loc_mask_1 = 1;
        loc_mask_2 = 2;
    }
    else if (fes_id_no_mask < fes_id_mask_1)
    {
        loc_mask_1 = 0;
        loc_no_mask = 1;
        loc_mask_2 = 2;
    }
    else
    {
        loc_mask_1 = 0;
        loc_mask_2 = 1;
        loc_no_mask = 2;
    }

    /*
     * Write to FESes.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_ace_set(unit, ace_id, fes_inst_info));

    /*
     * Get FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_ace_id_fes_info_get(unit, ace_id, &ace_id_fes_get_info));
    /*
     * Verify the FES configuration. 
     * Note that the location of the action in the field group is not reflected in the get function.
     */
    if (ace_id_fes_get_info.nof_fes_instr != 3
        || ace_id_fes_get_info.fes_instr[loc_no_mask].fes_id != fes_id_no_mask
        || ace_id_fes_get_info.fes_instr[loc_mask_1].fes_id != fes_id_mask_1
        || ace_id_fes_get_info.fes_instr[loc_mask_2].fes_id != fes_id_mask_2
        || ace_id_fes_get_info.fes_instr[loc_no_mask].action_type != action_type1
        || ace_id_fes_get_info.fes_instr[loc_mask_1].action_type != action_type2
        || ace_id_fes_get_info.fes_instr[loc_mask_2].action_type != action_type3
        || ace_id_fes_get_info.fes_instr[loc_no_mask].valid_bits != valid_bits
        || ace_id_fes_get_info.fes_instr[loc_mask_1].valid_bits != valid_bits
        || ace_id_fes_get_info.fes_instr[loc_mask_2].valid_bits != valid_bits
        || ace_id_fes_get_info.fes_instr[loc_no_mask].shift != shift
        || ace_id_fes_get_info.fes_instr[loc_mask_1].shift != shift
        || ace_id_fes_get_info.fes_instr[loc_mask_2].shift != shift
        || ace_id_fes_get_info.fes_instr[loc_no_mask].type != type
        || ace_id_fes_get_info.fes_instr[loc_mask_1].type != type
        || ace_id_fes_get_info.fes_instr[loc_mask_2].type != type
        || ace_id_fes_get_info.fes_instr[loc_no_mask].polarity != polarity
        || ace_id_fes_get_info.fes_instr[loc_mask_1].polarity != polarity
        || ace_id_fes_get_info.fes_instr[loc_mask_2].polarity != polarity
        || ace_id_fes_get_info.fes_instr[loc_no_mask].chosen_mask != 0
        || ace_id_fes_get_info.fes_instr[loc_mask_1].chosen_mask != 1
        || ace_id_fes_get_info.fes_instr[loc_mask_2].chosen_mask != 1
        || ace_id_fes_get_info.fes_instr[loc_no_mask].mask != required_mask1
        || ace_id_fes_get_info.fes_instr[loc_mask_1].mask != required_mask2
        || ace_id_fes_get_info.fes_instr[loc_mask_2].mask != required_mask3)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected FES configuration for first run.\r\n");
    }

    /*
     * Check the new number of masks. 
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_ace_id_nof_mask_state_get(unit, nof_masks_after));
    for (fes_id = 0; fes_id < dnx_data_field.ace.nof_fes_instruction_per_context_get(unit); fes_id++)
    {
        if (fes_id == fes_id_mask_1 || fes_id == fes_id_mask_2)
        {
            if (nof_masks_after[fes_id] != nof_masks_before[fes_id] + 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Added mask if FES ID %d not updated.\r\n", fes_id);
            }
        }
        else
        {
            if (nof_masks_after[fes_id] != nof_masks_before[fes_id])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Upexpected number of EFES masks change in FES ID %d.\r\n", fes_id);
            }
        }
    }

    /*
     * Delete FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_ace_detach(unit, ace_id));

    /*
     * Verify the delete.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_ace_id_fes_info_get(unit, ace_id, &ace_id_fes_get_info));
    if (ace_id_fes_get_info.nof_fes_instr != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES configuration was not deleted.\r\n");
    }

    /*
     * Rewrite to FEses after delete.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_ace_set(unit, ace_id, fes_inst_info));

    /*
     * Get FES configuration and check that it is unchanged.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_ace_id_fes_info_get(unit, ace_id, &ace_id_fes_get_info));
    /*
     * Check that the results are the same after rewrite.
     */
    if (ace_id_fes_get_info.nof_fes_instr != 3
        || ace_id_fes_get_info.fes_instr[loc_no_mask].fes_id != fes_id_no_mask
        || ace_id_fes_get_info.fes_instr[loc_mask_1].fes_id != fes_id_mask_1
        || ace_id_fes_get_info.fes_instr[loc_mask_2].fes_id != fes_id_mask_2
        || ace_id_fes_get_info.fes_instr[loc_no_mask].action_type != action_type1
        || ace_id_fes_get_info.fes_instr[loc_mask_1].action_type != action_type2
        || ace_id_fes_get_info.fes_instr[loc_mask_2].action_type != action_type3
        || ace_id_fes_get_info.fes_instr[loc_no_mask].valid_bits != valid_bits
        || ace_id_fes_get_info.fes_instr[loc_mask_1].valid_bits != valid_bits
        || ace_id_fes_get_info.fes_instr[loc_mask_2].valid_bits != valid_bits
        || ace_id_fes_get_info.fes_instr[loc_no_mask].shift != shift
        || ace_id_fes_get_info.fes_instr[loc_mask_1].shift != shift
        || ace_id_fes_get_info.fes_instr[loc_mask_2].shift != shift
        || ace_id_fes_get_info.fes_instr[loc_no_mask].type != type
        || ace_id_fes_get_info.fes_instr[loc_mask_1].type != type
        || ace_id_fes_get_info.fes_instr[loc_mask_2].type != type
        || ace_id_fes_get_info.fes_instr[loc_no_mask].polarity != polarity
        || ace_id_fes_get_info.fes_instr[loc_mask_1].polarity != polarity
        || ace_id_fes_get_info.fes_instr[loc_mask_2].polarity != polarity
        || ace_id_fes_get_info.fes_instr[loc_no_mask].chosen_mask != 0
        || ace_id_fes_get_info.fes_instr[loc_mask_1].chosen_mask != 1
        || ace_id_fes_get_info.fes_instr[loc_mask_2].chosen_mask != 1
        || ace_id_fes_get_info.fes_instr[loc_no_mask].mask != required_mask1
        || ace_id_fes_get_info.fes_instr[loc_mask_1].mask != required_mask2
        || ace_id_fes_get_info.fes_instr[loc_mask_2].mask != required_mask3)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected FES configuration for second run.\r\n");
    }

    /*
     * Check the new number of masks. 
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_ace_id_nof_mask_state_get(unit, nof_masks_after));
    for (fes_id = 0; fes_id < dnx_data_field.ace.nof_fes_instruction_per_context_get(unit); fes_id++)
    {
        if (fes_id == fes_id_mask_1 || fes_id == fes_id_mask_2)
        {
            if (nof_masks_after[fes_id] != nof_masks_before[fes_id] + 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Added mask if FES ID %d not updated.\r\n", fes_id);
            }
        }
        else
        {
            if (nof_masks_after[fes_id] != nof_masks_before[fes_id])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Upexpected number of EFES masks change in FES ID %d.\r\n", fes_id);
            }
        }
    }

    /*
     * Delete FES configuration once more.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_ace_detach(unit, ace_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is the basic 'ace' testing
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
ctest_dnx_ace_starter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *ace_test_type_name;
    int ace_test_traffic;
    int ace_test_update;
    uint32 ace_test_context;
    int ace_test_clean;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_STR(DNX_CTEST_ACE_OPTION_TEST_TYPE, ace_test_type_name);
    SH_SAND_GET_BOOL(DNX_CTEST_ACE_OPTION_TEST_TRAFFIC, ace_test_traffic);
    SH_SAND_GET_BOOL(DNX_CTEST_ACE_OPTION_TEST_UPDATE, ace_test_update);
    SH_SAND_GET_UINT32(DNX_CTEST_ACE_OPTION_TEST_CONTEXT, ace_test_context);
    SH_SAND_GET_BOOL(DNX_CTEST_ACE_OPTION_TEST_CLEAN, ace_test_clean);

    /*
     * For now, a fail to match the input parameter result by just a message.
     * Procedure returns with 'success'.
     */
    if (sal_strcasecmp(ace_test_type_name, TEST_TYPE_FORMAT) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Tesing 'ACE format'. %s%s\r\n", __FUNCTION__, __LINE__, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(ctest_dnx_ace_tester_format
                        (unit, ace_test_context, ace_test_traffic, ace_test_update, ace_test_clean));
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Option %s. Testing 'ACE format' returned 'success' flag.%s\r\n",
                    __FUNCTION__, __LINE__, ace_test_type_name, EMPTY);
    }
    else if (sal_strcasecmp(ace_test_type_name, TEST_TYPE_FES_CFG) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Testing 'ACE fes configuration'.%s%s\r\n", __FUNCTION__, __LINE__, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(ctest_dnx_ace_tester_fes_cfg(unit));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %s. Testing 'ACE fes configuration' returned 'success' flag.%s\r\n",
                    __FUNCTION__, __LINE__, ace_test_type_name, EMPTY);
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Option %s is not implemented. Illegal parameter. %s\r\n",
                    __FUNCTION__, __LINE__, ace_test_type_name, EMPTY);
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - run 'ace' sequence in diag shell
 */
shr_error_e
sh_dnx_field_ace_cmd(
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
        SHR_IF_ERR_EXIT(ctest_dnx_ace_starter(unit, args, sand_control));
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

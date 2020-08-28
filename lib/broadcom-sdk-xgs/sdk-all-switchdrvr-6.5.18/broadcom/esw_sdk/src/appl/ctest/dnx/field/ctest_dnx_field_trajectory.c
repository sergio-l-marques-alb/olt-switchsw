/** \file diag_dnx_field_trajectory.c
 * $Id$
 *
 * Trajectory iPMF use-case for DNX.
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
/** sal */
#include <sal/appl/sal.h>
/** soc */
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>

#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/instru/instru.h>
#include "ctest_dnx_field_utils.h"
#include <bcm_int/dnx/field/field_init.h>

/*
 * }
 */
/*
 * DEFINEs
 * {
 */

/*
 * The various values available for 'type' input.
 */
#define TEST_TYPE_INGRESS       "INGRESS"
#define TEST_TYPE_EGRESS        "EGRESS"

/*
 * }
 */

/*
 * MACROs
 * {
 */
/**
 * \brief
 *   Keyword for type of test on 'group' command (data base testing)
 *   TYPE can be either INGRESS or EGRESS.
 */
#define DNX_DIAG_FIELD_TAJECTORY_OPTION_TEST_TYPE           "type"
/**
 * \brief
 *   Keyword for TCL testing of test on 'group' command (data base testing)
 *   clean can be either 0 or 1, if the ctest will clean-up after its finished setting-up.
 */
#define DNX_DIAG_FIELD_TRAJECTORY_OPTION_CLEAN              "clean"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define DNX_DIAG_ACTION_OPTION_TEST_COUNT                   "count"
/*
 * }
 */

/* *INDENT-OFF* */

/**
 * \brief
 *   Options list for 'trajectory' shell command
 * \remark
 */
sh_sand_option_t dnx_field_trajectory_options[] = {
     /* Name */                                 /* Type */              /* Description */                                     /* Default */
    {DNX_DIAG_FIELD_TAJECTORY_OPTION_TEST_TYPE, SAL_FIELD_TYPE_STR,     "Type of test (ingress or egress)",                   "INGRESS"},
    {DNX_DIAG_FIELD_TRAJECTORY_OPTION_CLEAN,    SAL_FIELD_TYPE_BOOL,    "Will test perform HW and SW-state clean-up or not",  "Yes"},
    {DNX_DIAG_ACTION_OPTION_TEST_COUNT,         SAL_FIELD_TYPE_UINT32,  "Number of times test will run",            "1"},
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'trajectory' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t sh_dnx_field_trajectory_tests[] = {
    {"DNX_trajectory_setup", "type=INGRESS count=6", CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/**
 *  trajectory shell command leaf details
 */
sh_sand_man_t sh_dnx_field_trajectory_man = {
    "Execute trajectory sample with an IPV4 5-tuple.",
    "Ingress type creates ingress PMF configuration to enable trajectory hashing with 5-tuple"
        "(SIP,DIP,protocol,srcport,dstport) for ipv4."
        "Egress type configures the ETPP for an FTMH packet that is expected to leave the ITPP.",
    "ctest field trajectory type=<INGRESS | EGRESS> clean=<YES | NO> count=<Count>",
    "\n" "type=egress clean=NO"
};

/**
 * \brief
 *  Runs the trajectory IPv4/5-tuple sample application for ingress PMF.
 * \param [in] unit - Device ID
 * \param [out] context_id_p - Context ID
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
appl_dnx_trajectory_ingress(
    int unit,
    dnx_field_context_t * context_id_p)
{
    dnx_field_presel_entry_id_t p_id;
    dnx_field_context_flags_e flags;
    dnx_field_context_mode_t context_mode;
    dnx_field_group_info_t fg_info;
    dnx_field_group_attach_info_t a_info;
    dnx_field_group_t fg_id;
    dnx_field_presel_entry_data_t p_data;
    dnx_field_entry_t entry_in_info;
    uint32 entry_handle;
    dnx_field_context_hash_info_t hash_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));

    /**
     * Create and configure the context
     */
    flags = 0;
    context_mode.context_ipmf1_mode.hash_mode = DNX_FIELD_CONTEXT_HASH_MODE_ENABLED;
    SHR_IF_ERR_EXIT(dnx_field_context_create(unit, flags, DNX_FIELD_STAGE_IPMF1, &context_mode, context_id_p));

    /**
     * set the preselection for the context
     */
    p_id.presel_id = 5;
    p_id.stage = DNX_FIELD_STAGE_IPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = *context_id_p;
    p_data.nof_qualifiers = 1;
    p_data.qual_data[0].qual_type_dbal_field = DBAL_FIELD_FWD_LAYER_TYPE_0;
    /*
     * 0x2 is IPv4 
     */
    p_data.qual_data[0].qual_value = 0x2;
    p_data.qual_data[0].qual_mask = 0xFF;
    SHR_IF_ERR_EXIT(dnx_field_presel_set(0, 0, &p_id, &p_data));

    /**
     * Build the hash key with 5-tuple and attach to iPMF-1 context
     */

    SHR_IF_ERR_EXIT(dnx_field_context_hash_info_t_init(unit, &hash_info));

    hash_info.hash_function = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_BISYNC;
    hash_info.order = TRUE;
    hash_info.hash_config.action_key = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ADDITIONAL_LB;
    hash_info.hash_config.hash_action = DNX_FIELD_CONTEXT_HASH_ACTION_REPLACE_CRC16;

    hash_info.key_info.dnx_quals[0] =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_HEADER_QUAL_IPV4_SRC);

    hash_info.key_info.dnx_quals[1] =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_HEADER_QUAL_IPV4_DST);
    hash_info.key_info.dnx_quals[2] =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_HEADER_QUAL_IPV4_PROTOCOL);
    hash_info.key_info.dnx_quals[3] =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_HEADER_QUAL_L4_SRC_PORT);
    hash_info.key_info.dnx_quals[4] =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_HEADER_QUAL_L4_DST_PORT);
    hash_info.key_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    hash_info.key_info.qual_info[0].input_arg = 1;
    hash_info.key_info.qual_info[0].offset = 0;
    hash_info.key_info.qual_info[1].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    hash_info.key_info.qual_info[1].input_arg = 1;
    hash_info.key_info.qual_info[1].offset = 0;
    hash_info.key_info.qual_info[2].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    hash_info.key_info.qual_info[2].input_arg = 1;
    hash_info.key_info.qual_info[2].offset = 0;
    hash_info.key_info.qual_info[3].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    hash_info.key_info.qual_info[3].input_arg = 2;
    hash_info.key_info.qual_info[3].offset = 0;
    hash_info.key_info.qual_info[4].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    hash_info.key_info.qual_info[4].input_arg = 2;
    hash_info.key_info.qual_info[4].offset = 0;

    SHR_IF_ERR_EXIT(dnx_field_context_hash_create
                    (unit, DNX_FIELD_CONTEXT_HASH_FLAGS_NONE, DNX_FIELD_STAGE_IPMF1, *context_id_p, &hash_info));

    /**
     * Add a field group to iPMF2
     */
    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &fg_info));
    fg_info.field_stage = DNX_FIELD_STAGE_IPMF2;
    fg_info.fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    fg_info.dnx_quals[0] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_TRJ_HASH);
    /*
     * Note that 'field_group.dnx_actions' is initialized in 'dnx_field_group_info_t_init' above.
     */
    fg_info.dnx_actions[0] =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_IPMF1_ACTION_MIRROR_DATA);
    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, 0, &fg_info, &fg_id));

    /**
     * Attach the second field group to the context
     */
    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &a_info));
    sal_memcpy(a_info.dnx_quals, fg_info.dnx_quals, sizeof(a_info.dnx_quals));
    a_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;
    sal_memcpy(a_info.dnx_actions, fg_info.dnx_actions, sizeof(a_info.dnx_actions));
    a_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 7);
    SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, fg_id, *context_id_p, &a_info));

    /**
     * Add fake entry to test false positive. 
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_in_info));

    entry_in_info.priority = 16;
    entry_in_info.payload_info.action_info[0].dnx_action =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_IPMF1_ACTION_MIRROR_DATA);
    entry_in_info.payload_info.action_info[0].action_value[0] = 5;
    entry_in_info.key_info.qual_info[0].dnx_qual =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_TRJ_HASH);
    entry_in_info.key_info.qual_info[0].qual_value[0] = 0;
    /*
     * Mask is 16 bits for now.
     */
    entry_in_info.key_info.qual_info[0].qual_mask[0] = 0xffff;
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, fg_id, &entry_in_info, &entry_handle));
    /**
     * Add real entry to set the mirror mrror code to the value mapped to the trajectory trace mirror destination. 
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_in_info));

    entry_in_info.priority = 15;
    entry_in_info.payload_info.action_info[0].dnx_action =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_IPMF1_ACTION_MIRROR_DATA);
    entry_in_info.payload_info.action_info[0].action_value[0] = 1;
    entry_in_info.key_info.qual_info[0].dnx_qual =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_TRJ_HASH);
    /*
     * Must match tcl packet
     */
    entry_in_info.key_info.qual_info[0].qual_value[0] = 0x1b5d;
    /*
     * Mask is 16 bits for now.
     */
    entry_in_info.key_info.qual_info[0].qual_mask[0] = 0xffff;
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, fg_id, &entry_in_info, &entry_handle));
    /**
     * Add catch all entry to test false positive. 
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_in_info));

    entry_in_info.priority = 17;
    entry_in_info.payload_info.action_info[0].dnx_action =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_IPMF1_ACTION_MIRROR_DATA);
    entry_in_info.payload_info.action_info[0].action_value[0] = 6;
    entry_in_info.key_info.qual_info[0].dnx_qual =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_TRJ_HASH);
    entry_in_info.key_info.qual_info[0].qual_value[0] = 0;
    entry_in_info.key_info.qual_info[0].qual_mask[0] = 0;
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, fg_id, &entry_in_info, &entry_handle));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Runs the trajectory ETPP test.
 * \param [in] unit - Device ID
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
appl_dnx_trajectory_egress(
    int unit)
{
    bcm_instru_trajectory_trace_t trajectory_trace_params;
    bcm_gport_t gport;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * EEDB configuration for trajectory trace
     */
    trajectory_trace_params.egress_tunnel_if = 0;
    trajectory_trace_params.flags = 0 | BCM_TUNNEL_WITH_ID;
    trajectory_trace_params.observation_domain = 0x12345678;
    trajectory_trace_params.template_id = 1;
    BCM_GPORT_TUNNEL_ID_SET(gport, 0x1000);
    trajectory_trace_params.trajectory_trace_id = gport;
    SHR_IF_ERR_EXIT(dnx_instru_tragectory_trace_create(unit, &trajectory_trace_params));

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "Config trajectory trace tunnel id =%d\n"),
              trajectory_trace_params.trajectory_trace_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Runs the trajectory tests according to user input.
 * \param [in] unit - Device ID
 * \param [in] args - Extra arguments
  * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
appl_dnx_trajectory_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_type_name;
    int test_clean;
    dnx_field_context_t context_id;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_STR("type", test_type_name);
    SH_SAND_GET_BOOL("clean", test_clean);

    /*
     * For now, a fail to match the input parameter result by just a message.
     * Procedure returns with 'success'.
     */
    if (sal_strcasecmp(test_type_name, TEST_TYPE_INGRESS) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %s. Testing 'ingress'. %s\r\n",
                    __FUNCTION__, __LINE__, test_type_name, EMPTY);
        SHR_IF_ERR_EXIT(appl_dnx_trajectory_ingress(unit, &context_id));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %s. Testing 'ingress' returned 'success' flag. %s\r\n",
                    __FUNCTION__, __LINE__, test_type_name, EMPTY);
        if (test_clean == TRUE)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Option %s. Performing clean up. %s\r\n",
                        __FUNCTION__, __LINE__, test_type_name, EMPTY);
            CTEST_DNX_FIELD_UTIL_ERR(dnx_field_context_hash_destroy
                                     (unit, DNX_FIELD_CONTEXT_HASH_FLAGS_NONE, DNX_FIELD_STAGE_IPMF1, context_id));
            CTEST_DNX_FIELD_UTIL_ERR(ctest_dnx_field_util_clean(unit));

        }
    }
    else if (sal_strcasecmp(test_type_name, TEST_TYPE_EGRESS) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %s. Testing 'egress'. %s\r\n",
                    __FUNCTION__, __LINE__, test_type_name, EMPTY);
        SHR_IF_ERR_EXIT(appl_dnx_trajectory_egress(unit));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %s. Testing 'egress' returned 'success' flag. %s\r\n",
                    __FUNCTION__, __LINE__, test_type_name, EMPTY);
        if (test_clean == TRUE)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Option %s. Performing clean up. %s\r\n",
                        __FUNCTION__, __LINE__, test_type_name, EMPTY);
            
            SHR_IF_ERR_EXIT(ctest_dnx_field_util_clean(unit));
        }
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Option %s is not implemented. Illegal parameter. %s\r\n",
                    __FUNCTION__, __LINE__, test_type_name, EMPTY);
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - run trajectory init sequence in diag shell
 *
 * \param [in] unit - the unit number in system
 * \param [in,out] args - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "dnx trajectory_start"
 */
shr_error_e
sh_dnx_field_trajectory_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SH_SAND_GET_UINT32("count", count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_trajectory_run(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}

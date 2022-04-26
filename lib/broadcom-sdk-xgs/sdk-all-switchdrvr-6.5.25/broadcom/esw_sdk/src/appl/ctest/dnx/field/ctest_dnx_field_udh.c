/** \file ctest_dnx_field_udh.c
 *
 * UDH (User-Defined Headers)feature testing.
 * UDH is an extra header that gets attached to the packet while it passes through all the blocks
 * inside the device, it allows us to cascaded data from one block to another (mainly PMF).
 *
 * UDH is composed of:
 * 1) UDH base is 8bit Header that each 2bit defines the type of each User-Defined Header
 * 2) 4 User-Defined Headers that their size is according to their type.
 * 3) UDHs are appended to the header when they have size != 0
 *
 * UDH Types:
 * 1) There are 4 types that are pre-configured to certain sizes at init time (see field_init.c)
 * 2) Each type can have size that can vary between 0-4 bytes.
 * 3) For example types can be pre-configured to:
 *              * Type0 - 0B
 *              * Type1 - 1B
 *              * Type2 - 2B
 *              * Type3 - 4B
 *
 * Examples (based on the example types mentioned above):
 *     Example 1:
 *         UDH Base: 0x00
 *         *** No UDHs since all have size 0
 *
 *     Example 2:
 *         UDH Base: 0x03
 *         UDH1 : 0xA00000ABC  -- Example data
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /**
  * Include files.
  * {
  */
#include <soc/dnxc/swstate/sw_state_defs.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_init.h>
#include "ctest_dnx_field_udh.h"
#include "ctest_dnx_field_utils.h"
#include <appl/diag/dnx/diag_dnx_field.h>

/*
 * }
 */

/* *INDENT-OFF* */

/**
 * \brief
 *   Keyword for test type:
 *   Type can be either BCM or DNX
 */
#define CTEST_DNX_FIELD_UDH_OPTION_TYPE        "type"

/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define CTEST_DNX_FIELD_UDH_OPTION_COUNT              "count"

/**
 * \brief
 *   Keyword for controlling whether to release resources at end of test.
 *   clean can be either 0 or 1, if the ctest will clean-up after its finished setting-up.
 */
#define CTEST_DNX_FIELD_UDH_OPTION_CLEAN              "clean"

/**
 * \brief
 *   Options list for 'UDH' shell command
 * \remark
 */
sh_sand_option_t Sh_dnx_field_udh_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */
    {CTEST_DNX_FIELD_UDH_OPTION_TYPE,        SAL_FIELD_TYPE_ENUM,    "Type (level) of test (dnx or bcm)",        "DNX",     (void *)Field_level_enum_table},
    {CTEST_DNX_FIELD_UDH_OPTION_COUNT,       SAL_FIELD_TYPE_UINT32,  "Number of times test will run",            "1"},
    {CTEST_DNX_FIELD_UDH_OPTION_CLEAN,      SAL_FIELD_TYPE_BOOL,    "Will test perform HW and SW-state clean-up or not",     "Yes"},
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'UDH' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_udh_tests[] = {

    {"DNX_fld_udh", "type=dnx clean=yes count=6", CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/**
 *  context shell command leaf details
 */
sh_sand_man_t Sh_dnx_field_udh_man = {
    "Field UDH feature tests",
    "Creating a field group with single qualifier and action\n"
        "Creating a new context with one of the UDH modes\n"
        "Creating a new FG in IPMF2 to read the UDH result\n"
        "Cleaning up all resources used after the test finishesr\n"
        "The 'count' variable defines how many times the test will run",
    "ctest field udh type=<DNX | BCM> count=1"
};

/**
 * \brief
 *  UDH sample scenario, creates a field group that sets UDH1 type to 2 and UDH1 data to 0xA4A
 *
 * \param [in] unit - Device ID
 * \param [in] args - Extra arguments
 * \param [in] sand_control - Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
appl_dnx_field_udh_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_group_info_t fg_info_in;
    dnx_field_group_attach_info_t attach_info;
    dnx_field_group_t grp_id;
    dnx_field_entry_t entry_info;
    uint32 entry_handle1;
    unsigned int udh_test_clean;
    unsigned int test_type;
    dnx_field_context_t dnx_context_id;

    SHR_FUNC_INIT_VARS(unit);

    udh_test_clean = 0;
    test_type = 0;
    SH_SAND_GET_BOOL(CTEST_DNX_FIELD_UDH_OPTION_CLEAN, udh_test_clean);
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_UDH_OPTION_TYPE, test_type);

    if (test_type == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM Mode not supported for this test yet");
    }
    else if (test_type == 1)
    {
        bcm_field_context_param_info_t context_params;
        dnx_field_context_mode_t context_mode;

        /*
         * Create context
         */
        dnx_field_context_mode_t_init(unit, &context_mode);
        SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, DNX_FIELD_STAGE_IPMF1, &context_mode, &dnx_context_id));

        /** Set system header profile to a one that uses UDH */
        context_params.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
        context_params.param_val = bcmFieldSystemHeaderProfileFtmhTshPphUdh;
        SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1,
                                                    dnx_context_id, &context_params));

        /*
         * Add FG to set UDH1's Type/Data 
         */
        SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &fg_info_in));
        fg_info_in.fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
        fg_info_in.field_stage = DNX_FIELD_STAGE_IPMF1;

        /*
         * UDH1 Type/Data actions 
         */
        fg_info_in.dnx_actions[0] =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_USER_HEADER_1_TYPE);
        fg_info_in.dnx_actions[1] =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_USER_HEADER_1);

        /*
         * Add dummy qual/ we will later add a *catch-all* entry 
         */
        fg_info_in.dnx_quals[0] =
            DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_HEADER_QUAL_IPV4_SRC);

        SHR_IF_ERR_EXIT(dnx_field_group_add(unit, 0, &fg_info_in, &grp_id));

        /*
         * Attach FG to default context 
         */
        SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info));
        attach_info.dnx_actions[0] = fg_info_in.dnx_actions[0];
        attach_info.dnx_actions[1] = fg_info_in.dnx_actions[1];

        attach_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 10);
        attach_info.action_info[1].priority = BCM_FIELD_ACTION_PRIORITY(0, 10);

        attach_info.dnx_quals[0] = fg_info_in.dnx_quals[0];
        attach_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
        attach_info.qual_info[0].input_arg = 1;
        attach_info.qual_info[0].offset = 0;

        SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, grp_id, dnx_context_id, &attach_info));

        /*
         * Add *catch-all* entry 
         */
        SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));

        entry_info.key_info.qual_info[0].dnx_qual = fg_info_in.dnx_quals[0];

        /*
         * Mask 0 is catch-all 
         */
        entry_info.key_info.qual_info[0].qual_value[0] = 0x0;
        entry_info.key_info.qual_info[0].qual_mask[0] = 0x0;

        /*
         * Set Actions to change UDH1 Type to 2, and Data to 0xA4A 
         */
        entry_info.payload_info.action_info[0].dnx_action = fg_info_in.dnx_actions[0];
        entry_info.payload_info.action_info[0].action_value[0] = 2;

        entry_info.payload_info.action_info[1].dnx_action = fg_info_in.dnx_actions[1];
        entry_info.payload_info.action_info[1].action_value[0] = 0xA4A;

        entry_info.priority = 1;

        SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, grp_id, &entry_info, &entry_handle1));
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %d (for 'test_type') is not implemented. Illegal parameter. %s\r\n",
                    __func__, __LINE__, test_type, EMPTY);
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
exit:
    if ((test_type == 1) && udh_test_clean)
    {
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_context_detach(unit, grp_id, dnx_context_id));
        /** Delete the FG and deallocate the FG_ID */
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_delete(unit, grp_id));
        /** Delete the FG and deallocate the FG_ID */
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_context_destroy(unit, DNX_FIELD_STAGE_IPMF1, dnx_context_id));
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - runs the UDH ctest
 *
 * \param [in] unit - the unit number in system
 * \param [in] args - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "ctest field udh"
 */
shr_error_e
sh_dnx_field_udh_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(CTEST_DNX_FIELD_UDH_OPTION_COUNT, count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_udh_run(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}

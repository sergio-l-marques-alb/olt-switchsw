/** \file ctest_dnx_field_kbp.c
 * $Id$
 *
 * KBP iPMF use-case for DNX.
 *
 */

/*

ctest fld kbp type=simple clean=no count=1
dbal table info table=DYNAMIC_870

 * */
/*
 * $Copyright: (c) 2018 Broadcom.
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
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_init.h>
#include <bcm_int/dnx_dispatch.h>
#include "ctest_dnx_field_utils.h"
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
/*
 * }
 */

/* *INDENT-OFF* */
 /**
 * \brief
 *   Keyword for TCL testing, clean can be either 0 or 1,
 *   if the ctest will clean-up after its finished setting-up.
 */
#define DNX_DIAG_FIELD_KBP_OPTION_CLEAN             "clean"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define DNX_DIAG_FIELD_KBP_OPTION_TEST_COUNT        "count"
/**
 * \brief
 *   Keyword for test type
 */
#define DNX_DIAG_FIELD_KBP_OPTION_TEST_TYPE         "type"

#define TEST_TYPE_KBP_SIMPLE                        "simple"


#define KBP_FG_ID                                   5
#define KBP_CTX_ID                                  6

/**
 * \brief
 *   Options list for 'kbp' shell command
 * \remark
 */
sh_sand_option_t dnx_field_kbp_options[] = {
     /* Name */                                 /* Type */              /* Description */                       /* Default */
    {DNX_DIAG_FIELD_KBP_OPTION_TEST_TYPE,       SAL_FIELD_TYPE_STR,     "Type of test",                             TEST_TYPE_KBP_SIMPLE },
    {DNX_DIAG_FIELD_KBP_OPTION_CLEAN,           SAL_FIELD_TYPE_BOOL,    "Will test perform cleanup or not",         "Yes"},
    {DNX_DIAG_FIELD_KBP_OPTION_TEST_COUNT,      SAL_FIELD_TYPE_UINT32,  "Number of times test will run",            "1"},
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'kbp' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t sh_dnx_field_kbp_tests[] = {
    {"kbp", "type=simple clean=yes count=1", CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/**
 *  kbp shell command leaf details
 */
sh_sand_man_t sh_dnx_field_kbp_man = {
    "Create kbp configuration between IFWD-2 and IPMF1",
    "Using action of container to pass information",
    "ctest kbp type=simple clean=yes count=1",
};

static shr_error_e
appl_dnx_kbp_fg_add(
    int unit,
    dnx_field_group_t * fg_id_p)
{
    bcm_field_action_t bcm_action;
    dnx_field_action_t dnx_action;
    dnx_field_action_in_info_t action_info;
    dnx_field_group_info_t fg_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &fg_info));
    fg_info.field_stage = DNX_FIELD_STAGE_EXTERNAL;
    fg_info.fg_type = DNX_FIELD_GROUP_TYPE_KBP;
    sal_strncpy_s(fg_info.name, "KBP_FG_1", sizeof(fg_info.name));

    fg_info.dnx_quals[0] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_PP_PORT);
    fg_info.dnx_quals[1] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_DP);

    fg_info.dnx_quals[2] =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_HEADER_QUAL_MAC_SRC);

    fg_info.dnx_quals[3] =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_HEADER_QUAL_MAC_DST);
    fg_info.dnx_quals[4] =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_HEADER_QUAL_IPV6_SIP);

    /*
     *  Create a void user defined function
     */
    SHR_IF_ERR_EXIT(dnx_field_action_in_info_t_init(unit, &action_info));
    action_info.stage = DNX_FIELD_STAGE_EXTERNAL;
    action_info.bcm_action = bcmFieldActionVoid;
    action_info.size = 32;
    action_info.prefix_size = 0;        /* size + prefix_size summary should be 32 */
    sal_strncpy_s(action_info.name, "KBP_USER_ACTION", sizeof(action_info.name));

    SHR_IF_ERR_EXIT(dnx_field_action_create(unit, 0, &action_info, &bcm_action, &dnx_action));

    fg_info.dnx_actions[0][0] = dnx_action;
    fg_info.use_valid_bit[0][0] = FALSE;

    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, DNX_FIELD_GROUP_ADD_FLAG_WITH_ID, &fg_info, fg_id_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_kbp_fg_attach(
    int unit,
    dnx_field_group_t kbp_fg_id,
    dnx_field_context_t context_id)
{
    uint32 qual_ndx, action_ndx;
    dnx_field_group_attach_info_t group_attach_info;
    dnx_field_key_template_t key_template;
    dnx_field_actions_fg_payload_sw_info_t actions_payload_info;

    SHR_FUNC_INIT_VARS(unit);

    dnx_field_group_attach_info_t_init(unit, &group_attach_info);
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.key_template.get(unit, kbp_fg_id, &(key_template)));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.actions_payload_info.get(unit, kbp_fg_id, &(actions_payload_info)));

    /*
     * Build the attach information 
     */
    /*
     * Get the current result size and set the offset to it
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_total_result_size_get(unit, context_id, &group_attach_info.payload_offset));
    /*
     * This is the lookup id 
     */
    group_attach_info.payload_id = 1;

    for (qual_ndx = 0; qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_ndx++)
    {
        group_attach_info.dnx_quals[qual_ndx] = key_template.key_qual_map[qual_ndx].qual_type;
    }

    for (action_ndx = 0; action_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG; action_ndx++)
    {
        group_attach_info.dnx_actions[0][action_ndx] =
            actions_payload_info.actions_on_payload_info[0][action_ndx].dnx_action;
    }
    /*
     * Build the qualifier attach information 
     */
    group_attach_info.qual_info[0].input_arg = 0;
    group_attach_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
    group_attach_info.qual_info[0].offset = 0;

    group_attach_info.qual_info[1].input_arg = 0;
    group_attach_info.qual_info[1].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
    group_attach_info.qual_info[1].offset = 0;

    group_attach_info.qual_info[2].input_arg = 2;
    group_attach_info.qual_info[2].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    group_attach_info.qual_info[2].offset = 0xf;

    group_attach_info.qual_info[3].input_arg = 2;
    group_attach_info.qual_info[3].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    group_attach_info.qual_info[3].offset = 0xf;

    group_attach_info.qual_info[4].input_arg = 2;
    group_attach_info.qual_info[4].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    group_attach_info.qual_info[4].offset = 0xf;

    /*
     * Call the attach API
     */
    SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, kbp_fg_id, context_id, &group_attach_info));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_kbp_entry_add(
    int unit,
    dnx_field_group_t kbp_fg_id,
    uint32 *entry_handle)
{
    dnx_field_entry_t entry_info;

    uint32 qual_ndx, action_ndx, value_indx = 0;
    uint32 qual_size;
    dnx_field_key_template_t key_template;
    dnx_field_actions_fg_payload_sw_info_t actions_payload_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.key_template.get(unit, kbp_fg_id, &(key_template)));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.actions_payload_info.get(unit, kbp_fg_id, &(actions_payload_info)));

    dnx_field_entry_t_init(unit, &entry_info);
    /*
     * Build the entry information
     */

    for (qual_ndx = 0; (qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (key_template.key_qual_map[qual_ndx].qual_type != DNX_FIELD_QUAL_TYPE_INVALID); qual_ndx++)
    {
        entry_info.key_info.qual_info[qual_ndx].dnx_qual = key_template.key_qual_map[qual_ndx].qual_type;
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                        (unit, DNX_FIELD_STAGE_EXTERNAL, entry_info.key_info.qual_info[qual_ndx].dnx_qual, &qual_size));
        /*
         * Fill the value (might be bigger then 32b' ) 
         */
        for (value_indx = 0; value_indx < BCM_FIELD_QUAL_WIDTH_IN_WORDS; value_indx++)
        {
            entry_info.key_info.qual_info[qual_ndx].qual_value[value_indx] = qual_ndx;
            entry_info.key_info.qual_info[qual_ndx].qual_mask[value_indx] = 0xFFFFFFFF;
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "qual_size %d qual %s, qual_ndx %d, value_indx %d\n",
                         qual_size, dnx_field_dnx_qual_text(unit, entry_info.key_info.qual_info[qual_ndx].dnx_qual),
                         qual_ndx, value_indx);
        }

    }

    for (action_ndx = 0; (action_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) &&
         (actions_payload_info.actions_on_payload_info[0][action_ndx].dnx_action != DNX_FIELD_ACTION_INVALID);
         action_ndx++)
    {
        entry_info.payload_info.action_info[action_ndx].dnx_action =
            actions_payload_info.actions_on_payload_info[0][action_ndx].dnx_action;
        entry_info.payload_info.action_info[action_ndx].action_value[0] = 0xAB;
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "context %d action %s, action_ndx %d, value_indx %d\n",
                     KBP_CTX_ID, dnx_field_dnx_action_type_text(unit,
                                                                entry_info.payload_info.
                                                                action_info[action_ndx].dnx_action), action_ndx,
                     value_indx);
    }

    entry_info.priority = 1;

    SHR_IF_ERR_EXIT(dnx_field_entry_kbp_add(unit, kbp_fg_id, &entry_info, entry_handle));

exit:
    SHR_FUNC_EXIT;
    return 0;
}

/**
 * \brief
 * \param [in] unit - Device ID
 * \param [in] args - Extra arguments
 * \param [in] sand_control - Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
appl_dnx_kbp_clean(
    int unit,
    uint32 entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_entry_kbp_delete(unit, entry_handle));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * \param [in] unit - Device ID
 * \param [in] args - Extra arguments
 * \param [in] sand_control - Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
appl_dnx_kbp_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_group_t kbp_fg_id = KBP_FG_ID;

    int field_kbp_test_clean = 0;
    dnx_field_context_t context = KBP_CTX_ID;
    uint32 entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL(DNX_DIAG_FIELD_KBP_OPTION_CLEAN, field_kbp_test_clean);

    SHR_IF_ERR_EXIT(appl_dnx_kbp_fg_add(unit, &kbp_fg_id));

    SHR_IF_ERR_EXIT(appl_dnx_kbp_fg_attach(unit, kbp_fg_id, context));

    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));

    SHR_IF_ERR_EXIT(appl_dnx_kbp_entry_add(unit, kbp_fg_id, &entry_handle));

    LOG_ERROR_EX(BSL_LOG_MODULE, "Config KBP FG on IFWD2, fg_id=%d, context %d entry %u %s\n ", kbp_fg_id, context,
                 entry_handle, EMPTY);
exit:
    if (field_kbp_test_clean)
    {
        CTEST_DNX_FIELD_UTIL_ERR(appl_dnx_kbp_clean(unit, entry_handle));
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - run kbp init sequence in diag shell
 *
 * \param [in] unit - the unit number in system
 * \param [in,out] args - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "dnx kbp_start"
 */
shr_error_e
sh_dnx_field_kbp_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(DNX_DIAG_FIELD_KBP_OPTION_TEST_COUNT, count);

    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_kbp_run(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}

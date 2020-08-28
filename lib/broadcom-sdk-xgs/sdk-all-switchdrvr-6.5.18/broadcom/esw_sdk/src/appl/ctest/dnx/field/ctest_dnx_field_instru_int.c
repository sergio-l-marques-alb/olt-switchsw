/** \file diag_dnx_field_instru_int.c
 * $Id$
 *
 * Instrumantation of INT PMF configuration on DNX level.
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
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
/** bcm */
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_init.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "ctest_dnx_field_utils.h"

/*
 * }
 */

/* *INDENT-OFF* */
 /**
 * \brief
 *   Keyword , clean can be either 0 or 1,
 *   if the ctest will clean-up after its finished setting-up.
 */
#define DNX_DIAG_FIELD_INSTRU_INT_OPTION_CLEAN        "clean"
#define DNX_DIAG_FIELD_INSTRU_INT_OPTION_TEST_TYPE    "type"

#define TEST_TYPE_INT_TAIL                   "tail"
#define TEST_TYPE_INT_FIRST                  "first"
#define TEST_TYPE_INT_INTERMEDIATE           "intermediate"
#define TEST_TYPE_INT_LAST                   "last"
#define TEST_TYPE_INT_NONE                   "none" /* used only to clean */

/**
 * \brief
 *   Options list for 'cascade' shell command
 * \remark
 */
sh_sand_option_t dnx_field_instru_int_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */
    {DNX_DIAG_FIELD_INSTRU_INT_OPTION_CLEAN,       SAL_FIELD_TYPE_BOOL,    "Will test perform cleanup or not",          "Yes"},
    {DNX_DIAG_FIELD_INSTRU_INT_OPTION_TEST_TYPE,   SAL_FIELD_TYPE_STR,     "Type of test (first, intermediate, last)",  "intermediate" },
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'Instru INT' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t sh_dnx_field_instru_int_tests[] = {
    {"DNX_Istru_INT", "clean=yes", CTEST_POSTCOMMIT},
    {NULL}
};

/**
 * \brief
 *   List of global parameters for 'Instru INT' test.
 * \remark
 *   This parameters are set and used in the main function of the test - ctest_dnx_field_instru_int_run().
 *   They are kept as global since INT TCL tests need to configure PMF without clearing, then perform the test and only
 *   at the end of the test clear the PMF confiugrations. So these global parameters are used to store the allocated
 *   attributes in order for clear to work.
 *   This means that any test that uses this ctest can't support WB.
 */
dnx_field_context_t context_ipmf1;
dnx_field_context_t context_ipmf3;
dnx_field_presel_t presel_id_ipmf1 = 2;
dnx_field_group_t fg_id_ipmf1;
dnx_field_group_t fg_id_ipmf3;
bcm_field_group_t fg_flow_id;
uint32 entry_handle_ipmf1;
uint32 entry_handle_ipmf3;
bcm_field_action_t flow_action = 0;

/* *INDENT-ON* */
/**
 *  InstruINT shell command leaf details
 */
sh_sand_man_t sh_dnx_field_instru_int_man = {
    "Configure iPMF1 and iPMF3 for instrumentation application INT",
    "",
    "ctest field InstruINT",
    "clean=YES type=first"
};

/** Number of qualifier and action configured for iPMF1*/
#define CTEST_INSTRU_INT_NOF_QUAL_IPMF1     (1)
#define CTEST_INSTRU_INT_NOF_ACTION_IPMF1   (1)

/** Number of qualifier and action qualifier configured for iPMF3*/
#define CTEST_INSTRU_INT_NOF_QUAL_IPMF3     (1)
#define CTEST_INSTRU_INT_NOF_ACTION_IPMF3   (1)

/**  Qualifier and action type configured for iPMF1*/
#define CTEST_INSTRU_INT_IPMF1_QUAL_LR_QUALIFIER        (DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE))
#define CTEST_INSTRU_INT_IPMF1_ACTION_TAIL_EDIT_PROFILE   (DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1, DBAL_ENUM_FVAL_IPMF1_ACTION_END_OF_PACKET_EDITING))

/**
 * Value is set in the following way:
 * first_additional_header_exist = TRUE
 * first_additional_header = 4
 * second_additional_header_exists = TRUE
 * Tunnel_Type = 9
 * */

#define CTEST_INSTRU_INT_IPMF1_QUAL_LR_QUALIFIER_VALUE    (DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLAN_GPEOUDP)     /* GRE 
                                                                                                                         */

/** Qualifier and action type configured for iPMF3*/
#define CTEST_INSTRU_INT_IPMF3_QUAL_TAIL_EDIT_PROFILE        (DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_END_OF_PACKET_EDITING))
#define CTEST_INSTRU_INT_IPMF3_ACTION_INT_COMMAND        (DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF3, DBAL_ENUM_FVAL_IPMF3_ACTION_INT_DATA))

static shr_error_e
ctest_dnx_field_instru_int_ipmf3_fg_add(
    int unit,
    dnx_field_group_t * fg_id_p,
    dnx_field_group_info_t * fg_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, fg_info_p));
    fg_info_p->field_stage = DNX_FIELD_STAGE_IPMF3;
    fg_info_p->fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    fg_info_p->dnx_quals[0] = CTEST_INSTRU_INT_IPMF3_QUAL_TAIL_EDIT_PROFILE;
    /*
     * Set first action on first '2msb' combination' to be DP. All
     * other entries have been filled in in 'dnx_field_group_info_t_init' above.
     */
    fg_info_p->dnx_actions[0] = CTEST_INSTRU_INT_IPMF3_ACTION_INT_COMMAND;

    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, 0, fg_info_p, fg_id_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_field_instru_int_ipmf3_fg_attach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_info_t * fg_info_p,
    dnx_field_context_t context)
{
    dnx_field_group_attach_info_t a_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &a_info));

    sal_memcpy(a_info.dnx_quals, fg_info_p->dnx_quals, CTEST_INSTRU_INT_NOF_QUAL_IPMF3 * sizeof(dnx_field_qual_t));
    a_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;

    sal_memcpy(a_info.dnx_actions, fg_info_p->dnx_actions,
               CTEST_INSTRU_INT_NOF_QUAL_IPMF3 * sizeof(dnx_field_action_t));
    

    SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, fg_id, context, &a_info));

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
ctest_dnx_field_instru_int_ipmf3_entry_add(
    int unit,
    int qualifier_value,
    int action_value,
    dnx_field_group_t fg_id,
    uint32 *entry_handle_p)
{
    dnx_field_entry_t entry_in_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_in_info));

    entry_in_info.key_info.qual_info[0].dnx_qual = CTEST_INSTRU_INT_IPMF3_QUAL_TAIL_EDIT_PROFILE;
    entry_in_info.key_info.qual_info[0].qual_mask[0] = 0xF;
    entry_in_info.key_info.qual_info[0].qual_value[0] = qualifier_value;
    entry_in_info.payload_info.action_info[0].dnx_action = CTEST_INSTRU_INT_IPMF3_ACTION_INT_COMMAND;
    entry_in_info.payload_info.action_info[0].action_value[0] = action_value;
    entry_in_info.priority = 2;

    /** For more testing we can add entries with higher priorities that should not hit*/

    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, fg_id, &entry_in_info, entry_handle_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_field_instru_int_ipmf1_presel_config(
    int unit,
    dnx_field_context_t context,
    uint8 entry_valid,
    dnx_field_presel_t * presel_id_p)
{
    dnx_field_presel_entry_id_t p_id;
    dnx_field_presel_entry_data_t p_data;
    SHR_FUNC_INIT_VARS(unit);

    p_id.presel_id = *presel_id_p;
    p_id.stage = DNX_FIELD_STAGE_IPMF1;
    p_data.entry_valid = entry_valid;
    p_data.context_id = context;
    p_data.nof_qualifiers = 1;
    /**
     * Set the first layer to be Ethernet code 1
     * Set the second layer to be IPv4 code 2
     * */
    p_data.qual_data[0].qual_type_dbal_field = DBAL_FIELD_FWD_LAYER_TYPE_0;
    p_data.qual_data[0].qual_value = DBAL_ENUM_FVAL_LAYER_TYPES_IPV4;
    p_data.qual_data[0].qual_mask = 0x3F;

    SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, 0, &p_id, &p_data));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_field_instru_int_ipmf3_presel_config(
    int unit,
    dnx_field_context_t context,
    uint8 entry_valid,
    dnx_field_presel_t * presel_id_p)
{
    dnx_field_presel_entry_id_t p_id;
    dnx_field_presel_entry_data_t p_data;
    SHR_FUNC_INIT_VARS(unit);

    p_id.presel_id = *presel_id_p;
    p_id.stage = DNX_FIELD_STAGE_IPMF3;
    p_data.entry_valid = entry_valid;
    p_data.context_id = context;
    p_data.nof_qualifiers = 1;
    /**
     * */
    p_data.qual_data[0].qual_type_dbal_field = DBAL_FIELD_ACL_CONTEXT_PROFILE;
    p_data.qual_data[0].qual_value = context_ipmf1;
    p_data.qual_data[0].qual_mask = 0x3F;

    SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, 0, &p_id, &p_data));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_field_instru_int_ipmf1_fg_add(
    int unit,
    dnx_field_group_t * fg_id_p,
    dnx_field_group_info_t * fg_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, fg_info_p));
    fg_info_p->field_stage = DNX_FIELD_STAGE_IPMF1;
    fg_info_p->fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    fg_info_p->dnx_quals[0] = CTEST_INSTRU_INT_IPMF1_QUAL_LR_QUALIFIER;
    /*
     * Note that 'fg_info_p->dnx_actions' has been initiated in 'dnx_field_group_info_t_init' above.
     */
    fg_info_p->dnx_actions[0] = CTEST_INSTRU_INT_IPMF1_ACTION_TAIL_EDIT_PROFILE;

    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, 0, fg_info_p, fg_id_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_field_instru_int_ipmf1_fg_attach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_info_t * fg_info_p,
    dnx_field_context_t context)
{
    dnx_field_group_attach_info_t a_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &a_info));

    sal_memcpy(a_info.dnx_quals, fg_info_p->dnx_quals, CTEST_INSTRU_INT_NOF_QUAL_IPMF1 * sizeof(dnx_field_qual_t));
    a_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD;
    a_info.qual_info[0].input_arg = 0;

    sal_memcpy(a_info.dnx_actions, fg_info_p->dnx_actions,
               CTEST_INSTRU_INT_NOF_QUAL_IPMF1 * sizeof(dnx_field_action_t));
    

    SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, fg_id, context, &a_info));

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
ctest_dnx_field_instru_int_ipmf1_entry_add(
    int unit,
    int action_value,
    dnx_field_group_t fg_id,
    uint32 *entry_handle_p)
{
    dnx_field_entry_t entry_in_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_in_info));

    entry_in_info.key_info.qual_info[0].dnx_qual = CTEST_INSTRU_INT_IPMF1_QUAL_LR_QUALIFIER;
    entry_in_info.key_info.qual_info[0].qual_value[0] = CTEST_INSTRU_INT_IPMF1_QUAL_LR_QUALIFIER_VALUE;
    /**Value is 13 bit length LR qualfifier for IPv4*/
    entry_in_info.key_info.qual_info[0].qual_mask[0] = 0xf;
    entry_in_info.payload_info.action_info[0].dnx_action = CTEST_INSTRU_INT_IPMF1_ACTION_TAIL_EDIT_PROFILE;
    entry_in_info.payload_info.action_info[0].action_value[0] = action_value;
    entry_in_info.priority = 2;

    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, fg_id, &entry_in_info, entry_handle_p));

exit:
    SHR_FUNC_EXIT;
}

/*This is WA to create FLow Id*/
static shr_error_e
ctest_dnx_field_flow_id_ipmf1_WA_init(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_predefined;
    void *dest_char;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_field_action_info_get
                    (unit, bcmFieldActionLatencyFlowId, bcmFieldStageIngressPMF1, &action_info_predefined));

       /** Create destination action to ignore destination qualifier which is 0*/
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionLatencyFlowId;
    action_info.prefix_size = action_info_predefined.size;
       /**Valid bit is the lsb*/
    action_info.prefix_value = 1;
    action_info.size = 0;
    action_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "flow_valid_bit", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &flow_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = bcmFieldStageIngressPMF1;

    fg_info.nof_actions = 1;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[0] = flow_action;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "wa_flow_id", sizeof(fg_info.name));

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_flow_id));

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
       /**Make the action to be lowest priority*/
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_POSITION(0, 0);

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_flow_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Simple Case
 *  In iPMF1:
 *     if in DP =0 then CONTAINER = 3
 *  In iPMF3:
 *     if CONTAINER = 3 then DP = 2
 * \param [in] unit - Device ID
 * \param [in] args - Extra arguments
 * \param [in] sand_control - Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_instru_int_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_context_mode_t context_mode;
    dnx_field_group_info_t fg_info_ipmf1;

    dnx_field_group_info_t fg_info_ipmf3;
    int field_instru_int_test_clean;
    char *int_test_type = NULL;

    SHR_FUNC_INIT_VARS(unit);

    field_instru_int_test_clean = 0;
    SH_SAND_GET_BOOL("clean", field_instru_int_test_clean);

    SH_SAND_GET_STR(DNX_DIAG_FIELD_INSTRU_INT_OPTION_TEST_TYPE, int_test_type);
    if (sal_strcasecmp(int_test_type, TEST_TYPE_INT_NONE) == 0)
    {
        /** used just for cleaning */
        goto exit;
    }

    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
    SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, DNX_FIELD_STAGE_IPMF1, &context_mode, &context_ipmf1));
    /**Call WA to enable Flow id*/
    SHR_IF_ERR_EXIT(ctest_dnx_field_flow_id_ipmf1_WA_init(unit, context_ipmf1));
    SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_ipmf1_presel_config(unit, context_ipmf1, TRUE, &presel_id_ipmf1));
    SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, DNX_FIELD_STAGE_IPMF3, &context_mode, &context_ipmf3));
    SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_ipmf3_presel_config(unit, context_ipmf3, TRUE, &presel_id_ipmf1));
    SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_ipmf1_fg_add(unit, &fg_id_ipmf1, &fg_info_ipmf1));

    SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_ipmf1_fg_attach(unit, fg_id_ipmf1, &fg_info_ipmf1, context_ipmf1));

    if (sal_strcasecmp(int_test_type, TEST_TYPE_INT_INTERMEDIATE) == 0)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_ipmf1_entry_add(unit, 2, fg_id_ipmf1, &entry_handle_ipmf1));
    }
    else if (sal_strcasecmp(int_test_type, TEST_TYPE_INT_FIRST) == 0)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_ipmf1_entry_add(unit, 4, fg_id_ipmf1, &entry_handle_ipmf1));
    }
    else if (sal_strcasecmp(int_test_type, TEST_TYPE_INT_LAST) == 0)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_ipmf1_entry_add(unit, 6, fg_id_ipmf1, &entry_handle_ipmf1));
    }
    else                                                                                                                                         /** TEST_TYPE_INT_TAIL */
    {
        SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_ipmf1_entry_add(unit, 1, fg_id_ipmf1, &entry_handle_ipmf1));
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Config iPMF1, context=%d, presel=%d, fg_id=%d,entry_handle=%d  \n ", context_ipmf1,
                 presel_id_ipmf1, fg_id_ipmf1, entry_handle_ipmf1);

    SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_ipmf3_fg_add(unit, &fg_id_ipmf3, &fg_info_ipmf3));

    SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_ipmf3_fg_attach(unit, fg_id_ipmf3, &fg_info_ipmf3, context_ipmf3));

    if (sal_strcasecmp(int_test_type, TEST_TYPE_INT_INTERMEDIATE) == 0)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_ipmf3_entry_add(unit, 2, 1, fg_id_ipmf3, &entry_handle_ipmf3));
    }
    else if (sal_strcasecmp(int_test_type, TEST_TYPE_INT_FIRST) == 0)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_ipmf3_entry_add(unit, 4, 1, fg_id_ipmf3, &entry_handle_ipmf3));
    }
    else if (sal_strcasecmp(int_test_type, TEST_TYPE_INT_LAST) == 0)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_ipmf3_entry_add(unit, 6, 0, fg_id_ipmf3, &entry_handle_ipmf3));
    }
    else                                                                                                                                         /** TEST_TYPE_INT_TAIL */
    {
        SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_ipmf3_entry_add(unit, 1, 1, fg_id_ipmf3, &entry_handle_ipmf3));
    }

    LOG_INFO_EX(BSL_LOG_MODULE,
                "Config iPMF3, context=%d, fg_id=%d,entry_handle=%d %s \n ", context_ipmf3, fg_id_ipmf3,
                entry_handle_ipmf3, EMPTY);

exit:
    if (field_instru_int_test_clean)
    {
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_entry_tcam_delete(unit, fg_id_ipmf1, entry_handle_ipmf1, NULL));
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_entry_tcam_delete(unit, fg_id_ipmf3, entry_handle_ipmf3, NULL));
        CTEST_DNX_FIELD_UTIL_ERR(ctest_dnx_field_instru_int_ipmf1_presel_config
                                 (unit, context_ipmf1, FALSE, &presel_id_ipmf1));
        CTEST_DNX_FIELD_UTIL_ERR(ctest_dnx_field_instru_int_ipmf3_presel_config
                                 (unit, context_ipmf3, FALSE, &presel_id_ipmf1));
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_context_detach(unit, fg_id_ipmf1, context_ipmf1));
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_context_detach(unit, fg_flow_id, context_ipmf1));
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_context_detach(unit, fg_id_ipmf3, context_ipmf3));
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_context_destroy(unit, DNX_FIELD_STAGE_IPMF1, context_ipmf1));
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_context_destroy(unit, DNX_FIELD_STAGE_IPMF3, context_ipmf3));
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_delete(unit, fg_id_ipmf1));
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_delete(unit, fg_flow_id));
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_delete(unit, fg_id_ipmf3));

        SHR_IF_ERR_EXIT(bcm_field_action_destroy(unit, flow_action));
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - run INT instrumentations init sequence in diag shell
 *
 * \param [in] unit - the unit number in system
 * \param [in,out] args - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "ct fld IINT"
 */
shr_error_e
sh_dnx_field_instru_int_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(ctest_dnx_field_instru_int_run(unit, args, sand_control));
exit:
    SHR_FUNC_EXIT;
}

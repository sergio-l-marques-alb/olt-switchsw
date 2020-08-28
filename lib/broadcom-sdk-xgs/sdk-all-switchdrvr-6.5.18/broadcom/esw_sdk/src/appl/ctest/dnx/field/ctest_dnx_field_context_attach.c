
/** \file ctest_dnx_field_context_attach.c
 *
 * Field Context attach/detach usage testing and example.
 * Creating a Field group with a single qualifier and single action,
 * which is attached to a context that we create.
 *
 * Since we only use one action and one qualifier, we use index zero(0) hard-coded,
 * for access and allocation.
 *
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

 /**
  * Include files.
  * {
  */
#include <soc/dnxc/swstate/sw_state_defs.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_init.h>
#include "ctest_dnx_field_context_attach.h"
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
#define CTEST_DNX_FIELD_CONTEXT_ATTACH_OPTION_TYPE        "type"
/**
 * \brief
 *   Keyword for stage of test on context command (data base testing)
 *   stage can be either IPMF1, IPMF2 ,IPMF3, EPMF .
 */
#define CTEST_DNX_FIELD_CONTEXT_ATTACH_OPTION_TEST_STAGE         "stage"
 /**
 * \brief
 *   Clean can be either 0 or 1,
 *   if the ctest will clean-up after its finished setting-up.
 */
#define CTEST_DNX_FIELD_CONTEXT_ATTACH_OPTION_CLEAN              "clean"

/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define CTEST_DNX_FIELD_CONTEXT_ATTACH_OPTION_COUNT              "count"

/**
 * \brief
 *   Options list for 'context_attach' shell command
 * \remark
 */
sh_sand_option_t Sh_dnx_field_context_attach_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */
    {CTEST_DNX_FIELD_CONTEXT_ATTACH_OPTION_TYPE,        SAL_FIELD_TYPE_ENUM,    "Type (level) of test (dnx or bcm)",        "DNX",     (void *)Field_level_enum_table},
    {CTEST_DNX_FIELD_CONTEXT_ATTACH_OPTION_TEST_STAGE,  SAL_FIELD_TYPE_ENUM,    "Stage of test (ipmf1,ipmf2,ipmf3,epmf)",   "ipmf1", (void *)Field_stage_enum_table},
    {CTEST_DNX_FIELD_CONTEXT_ATTACH_OPTION_CLEAN,       SAL_FIELD_TYPE_BOOL,    "Will test perform cleanup or not",         "Yes"},
    {CTEST_DNX_FIELD_CONTEXT_ATTACH_OPTION_COUNT,       SAL_FIELD_TYPE_UINT32,  "Number of times test will run",            "1"},
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'context_attach' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_context_attach_tests[] = {

    {"DNX_fld_context_attach_1", "type=dnx clean=yes stage=ipmf1 count=6", CTEST_POSTCOMMIT},
    {"DNX_fld_context_attach_2", "type=dnx clean=yes stage=ipmf2 count=6", CTEST_POSTCOMMIT},
    {"DNX_fld_context_attach_3", "type=dnx clean=yes stage=ipmf3 count=6", CTEST_POSTCOMMIT},
    {"DNX_fld_context_attach_e", "type=dnx clean=yes stage=epmf  count=6", CTEST_POSTCOMMIT},
    {"BCM_fld_context_attach_1", "type=bcm clean=yes stage=ipmf1 count=6", CTEST_POSTCOMMIT},
    {"BCM_fld_context_attach_2", "type=bcm clean=yes stage=ipmf2 count=6", CTEST_POSTCOMMIT},
    {"BCM_fld_context_attach_3", "type=bcm clean=yes stage=ipmf3 count=6", CTEST_POSTCOMMIT},
    {"BCM_fld_context_attach_e", "type=bcm clean=yes stage=epmf  count=6", CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/**
 *  context shell command leaf details
 */
sh_sand_man_t Sh_dnx_field_context_attach_man = {
    "Field context attach/detach related test utilities",
    "Creating a field group with single qualifier and action \r\n"
        "Attaching the created field group to a context with valid attach info \r\n"
        "Performing a 'dnx_field_group_context_get' and compare \r\n"
        "Cleaning up all resources used after the test finishes. \r\n"
        "The 'count' variable defines how many times the test will run. \r\n",
    "ctest field context_attach type=<DNX | BCM> stage=<IPMF1 | IPMF2 | IPMF3 | EPMF> clean=<YES | NO> count=1"
};
/* *INDENT-OFF* */
/**
 * DNX Qualifiers , actions and qualifier classes per stage.
 */
static ctest_dnx_field_context_attach_info_per_stage_t Qual_action_info[CTEST_DNX_FIELD_CONTEXT_ATTACH_NOF_PMF_STAGES] =
{
                                                        /** IPMF1 Data */
        {DBAL_ENUM_FVAL_IPMF1_ACTION_SNOOP_DATA , DNX_FIELD_HEADER_QUAL_VLAN_ID, DNX_FIELD_QUAL_CLASS_HEADER},
                                                        /** IPMF2 Data */
        {DBAL_ENUM_FVAL_IPMF1_ACTION_IN_PP_PORT,  DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_0, DNX_FIELD_QUAL_CLASS_META2},
                                                        /** IPMF3 Data */
        {DBAL_ENUM_FVAL_IPMF3_ACTION_SNOOP_DATA, DNX_FIELD_IPMF3_QUAL_EEI, DNX_FIELD_QUAL_CLASS_META},
                                                        /** EPMF Data */
        {DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_STRENGTH,DNX_FIELD_HEADER_QUAL_MPLS_LABEL, DNX_FIELD_QUAL_CLASS_HEADER}
};

/**
 * BCM Qualifiers , actions and qualifier info per stage.
 */
static ctest_dnx_field_bcm_context_attach_info_per_stage_t Qual_bcm_action_info[CTEST_DNX_FIELD_CONTEXT_ATTACH_NOF_PMF_STAGES] =
{
                           /** IPMF1 Data */
    { bcmFieldActionDropPrecedenceRaw, bcmFieldQualifyVlanId, {bcmFieldInputTypeLayerAbsolute, CTEST_DNX_FIELD_CONTEXT_ATTACH_QUAL_INPUT_ARG_AND_OFFSET, CTEST_DNX_FIELD_CONTEXT_ATTACH_QUAL_INPUT_ARG_AND_OFFSET} },
                           /** IPMF2 Data */
    { bcmFieldActionAdmitProfile, bcmFieldQualifyHashValue, {bcmFieldInputTypeMetaData, BCM_FIELD_INVALID, BCM_FIELD_INVALID} },
                           /** IPMF3 Data */
    { bcmFieldActionSnoopRaw, bcmFieldQualifyVlanAction, {bcmFieldInputTypeMetaData, BCM_FIELD_INVALID, BCM_FIELD_INVALID} },
                           /** EPMF Data */
    { bcmFieldActionMirrorEgressRaw, bcmFieldQualifyMplsLabel, {bcmFieldInputTypeLayerAbsolute, CTEST_DNX_FIELD_CONTEXT_ATTACH_QUAL_INPUT_ARG_AND_OFFSET, CTEST_DNX_FIELD_CONTEXT_ATTACH_QUAL_INPUT_ARG_AND_OFFSET} }
};

/* *INDENT-ON* */
/**
 * \brief
 *  Function to perform the filling of the  DNX API structure 'fg_info_p' and
 *  calling dnx_field_group_add API with single qualifier and action.
 *  This is a TCAM only Field Group.
 * \param [in] unit             - Device ID
 * \param [in] field_stage      - Field stage for FG creation
 * \param [in] Qual_action_info - Qualifier and Action info per stage
 * \param [out] fg_info_p       - Field Group info to be filled here.
 * \param [out] fg_id_p         - Field Group ID.
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_context_attach_fg_add_dnx(
    int unit,
    dnx_field_stage_e field_stage,
    ctest_dnx_field_context_attach_info_per_stage_t * qual_action_info,
    dnx_field_group_info_t * fg_info_p,
    dnx_field_group_t * fg_id_p)
{
    dnx_field_group_add_flags_e flags;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qual_action_info, _SHR_E_PARAM, "qual_action_info");
    SHR_NULL_CHECK(fg_info_p, _SHR_E_PARAM, "fg_info_p");
    SHR_NULL_CHECK(fg_id_p, _SHR_E_PARAM, "fg_id_p");
    flags = 0;

    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, fg_info_p));

    fg_info_p->fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    fg_info_p->field_stage = field_stage;
    /**
     * Set only 1 action and 1 qualifier  -
     * IPMF1 and EPMF  - HEADER qualifier
     * IPMF2 and IPMF3 - METADATA qualifier
     */
    fg_info_p->dnx_quals[0] = DNX_QUAL(qual_action_info->qual_class, field_stage, qual_action_info->dnx_qualifier);
    fg_info_p->dnx_actions[0] = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, qual_action_info->dnx_action);

    LOG_INFO_EX(BSL_LOG_MODULE, "%s();, Line %d, Stage: %s .Performing Field Group Add %s \r\n",
                __FUNCTION__, __LINE__, dnx_field_stage_text(unit, field_stage), EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, flags, fg_info_p, fg_id_p));

    LOG_INFO_EX(BSL_LOG_MODULE, "%s();, Line %d, Stage: %s  Field Group: %d Added  \r\n",
                __FUNCTION__, __LINE__, dnx_field_stage_text(unit, field_stage), *fg_id_p);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Function to perform the filling of the  DNX API structure 'fg_info_p' and
 *  calling dnx_field_group_add API with single qualifier and action.
 *  This is a TCAM only Field Group.
 * \param [in] unit             - Device ID
 * \param [in] field_stage      - Field stage for FG creation
 * \param [in] qual_action_info_p - Qualifier and Action info per stage
 * \param [out] fg_info_p       - Field Group info to be filled here.
 * \param [out] fg_id_p         - Field Group ID.
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_context_attach_fg_add_bcm(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_bcm_context_attach_info_per_stage_t * qual_action_info_p,
    bcm_field_group_info_t * fg_info_p,
    bcm_field_group_t * fg_id_p)
{
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qual_action_info_p, _SHR_E_PARAM, "qual_action_info_p");
    SHR_NULL_CHECK(fg_info_p, _SHR_E_PARAM, "fg_info_p");
    SHR_NULL_CHECK(fg_id_p, _SHR_E_PARAM, "fg_id_p");
    flags = BCM_FIELD_FLAG_MSB_RESULT_ALIGN;

    bcm_field_group_info_t_init(fg_info_p);

    fg_info_p->fg_type = bcmFieldGroupTypeTcam;
    fg_info_p->stage = field_stage;
    /**
     * Set only 1 action and 1 qualifier -
     * IPMF1 and EPMF  - HEADER qualifier
     * IPMF2 and IPMF3 - METADATA qualifier
     */
    fg_info_p->nof_actions = CTEST_DNX_FIELD_CONTEXT_ATTACH_NOF_QUALS_ACTIONS;
    fg_info_p->nof_quals = CTEST_DNX_FIELD_CONTEXT_ATTACH_NOF_QUALS_ACTIONS;
    fg_info_p->action_types[0] = qual_action_info_p->bcm_action;
    fg_info_p->qual_types[0] = qual_action_info_p->bcm_qualifier;

    LOG_INFO_EX(BSL_LOG_MODULE, "%s();, Line %d, Stage: %s .Performing Field Group Add %s \r\n",
                __FUNCTION__, __LINE__, dnx_field_bcm_stage_text(field_stage), EMPTY);

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, flags, fg_info_p, fg_id_p));

    LOG_INFO_EX(BSL_LOG_MODULE, "%s();, Line %d, Stage: %s  Field Group: %d Added  \r\n",
                __FUNCTION__, __LINE__, dnx_field_bcm_stage_text(field_stage), *fg_id_p);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Function to perform the filling of the  DNX API structure 'attach_info_p' and
 *  calling 'dnx_field_group_context_attach' API with valid attach info for the set
 *  qualifier and action.
 * \param [in] unit             - Device ID
 * \param [in] context_id       - Context ID for attach
 * \param [in] fg_id            - Field Group ID.
 * \param [in] fg_info_p        - Field Group info to be used for attach filling attach info.
 * \param [out] attach_info_p   - Context Attach info to be filled and attached.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_context_attach_ca_dnx(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_group_info_t * fg_info_p,
    dnx_field_group_attach_info_t * attach_info_p)
{
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    flags = 0;

    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, attach_info_p));

    /** Copying the FG info qualifiers and actions to the attach info */
    sal_memcpy(attach_info_p->dnx_quals, fg_info_p->dnx_quals, sizeof(attach_info_p->dnx_quals));
    sal_memcpy(attach_info_p->dnx_actions, fg_info_p->dnx_actions, sizeof(attach_info_p->dnx_actions));

    /** Giving random mid-range priority, the value can be changed, it will not influence the test */
    attach_info_p->action_info[0].priority = CTEST_DNX_FIELD_CONTEXT_ATTACH_ACTION_PRIORITY;

    /** Checking the Qualifier class of the set qualifier and add the qualifier info accordingly */
    if (DNX_QUAL_CLASS(attach_info_p->dnx_quals[0]) == DNX_FIELD_QUAL_CLASS_HEADER
        || DNX_QUAL_CLASS(attach_info_p->dnx_quals[0]) == DNX_FIELD_QUAL_CLASS_LAYER_RECORD)
    {
        /** We set header qualifiers to use ABSOLUTE layer base - Legal hard coded values for Semantic testing */
        attach_info_p->qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
        attach_info_p->qual_info[0].input_arg = CTEST_DNX_FIELD_CONTEXT_ATTACH_QUAL_INPUT_ARG_AND_OFFSET;
        attach_info_p->qual_info[0].offset = CTEST_DNX_FIELD_CONTEXT_ATTACH_QUAL_INPUT_ARG_AND_OFFSET;
    }
    else if (DNX_QUAL_CLASS(attach_info_p->dnx_quals[0]) == DNX_FIELD_QUAL_CLASS_META)
    {
        attach_info_p->qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
    }
    else if (DNX_QUAL_CLASS(attach_info_p->dnx_quals[0]) == DNX_FIELD_QUAL_CLASS_META2)
    {
        attach_info_p->qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;
    }
    LOG_INFO_EX(BSL_LOG_MODULE, "Line %d, Stage: %s .Performing Context Attach on Context: %d and Field Group: %d \r\n",
                __LINE__, dnx_field_stage_text(unit, fg_info_p->field_stage), context_id, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, flags, fg_id, context_id, attach_info_p));

    LOG_INFO_EX(BSL_LOG_MODULE, "Line %d, Stage: %s .Field Group: %d has been attached  to Context: %d \r\n",
                __LINE__, dnx_field_stage_text(unit, fg_info_p->field_stage), fg_id, context_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Function to perform the filling of the  DNX API structure 'attach_info_p' and
 *  calling 'dnx_field_group_context_attach' API with valid attach info for the set
 *  qualifier and action.
 * \param [in] unit             - Device ID
 * \param [in] context_id       - Context ID for attach
 * \param [in] fg_id            - Field Group ID.
 * \param [in] qual_action_info_p - Qualifier and Action info per stage
 * \param [in] fg_info_p        - Field Group info to be used for attach filling attach info.
 * \param [out] attach_info_p   - Context Attach info to be filled and attached.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_context_attach_ca_bcm(
    int unit,
    bcm_field_context_t context_id,
    bcm_field_group_t fg_id,
    ctest_dnx_field_bcm_context_attach_info_per_stage_t * qual_action_info_p,
    bcm_field_group_info_t * fg_info_p,
    bcm_field_group_attach_info_t * attach_info_p)
{
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    flags = 0;

    bcm_field_group_attach_info_t_init(attach_info_p);

    /** Set the FG info qualifiers and actions to the attach info */
    attach_info_p->payload_info.nof_actions = fg_info_p->nof_actions;
    attach_info_p->key_info.nof_quals = fg_info_p->nof_quals;
    attach_info_p->key_info.qual_types[0] = fg_info_p->qual_types[0];
    attach_info_p->payload_info.action_types[0] = fg_info_p->action_types[0];
    attach_info_p->payload_info.action_info[0].priority = CTEST_DNX_FIELD_CONTEXT_ATTACH_ACTION_PRIORITY;

    /** Add the qualifier info according to the given stage information from qual_action_info_p */
    attach_info_p->key_info.qual_info[0].input_type = qual_action_info_p->qualifier_attach_info.input_type;
    attach_info_p->key_info.qual_info[0].input_arg = qual_action_info_p->qualifier_attach_info.input_arg;
    attach_info_p->key_info.qual_info[0].offset = qual_action_info_p->qualifier_attach_info.offset;

    LOG_INFO_EX(BSL_LOG_MODULE, "Line %d, Stage: %s .Performing Context Attach on Context: %d and Field Group: %d \r\n",
                __LINE__, dnx_field_bcm_stage_text(fg_info_p->stage), context_id, fg_id);

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, flags, fg_id, context_id, attach_info_p));

    LOG_INFO_EX(BSL_LOG_MODULE, "Line %d, Stage: %s .Field Group: %d has been attached  to Context: %d \r\n",
                __LINE__, dnx_field_bcm_stage_text(fg_info_p->stage), fg_id, context_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Function to  call 'dnx_field_group_context_get' using the supplied 'fg_id' and 'context_id'
 *  Which were used in the corresponding 'set' operation (dnx_field_group_context_attach())
 *  We compare it to the 'set_attach_info_p' supplied to the function.
 *  We have a single qualifier and single action set in the attach info, hence why
 *  we only compare the first element of the corresponding arrays.
 * \param [in] unit             - Device ID
 * \param [in] context_id       - Context ID for attach
 * \param [in] fg_id            - Field Group ID.
 * \param [out] set_attach_info_p   - Context Attach info to be compared against.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_context_attach_compare_dnx(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_group_attach_info_t * set_attach_info_p)
{
    dnx_field_group_context_full_info_t get_attach_info;

    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO_EX(BSL_LOG_MODULE, "%s();, Line %d, .Performing group context get to field group: %d and context %d\r\n",
                __FUNCTION__, __LINE__, fg_id, context_id);

    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id, &get_attach_info));

    LOG_INFO_EX(BSL_LOG_MODULE,
                "%s(), Line %d .Performing compare of all dnx_field_group_attach_info_t fields %s%s\r\n", __FUNCTION__,
                __LINE__, EMPTY, EMPTY);

    /** Comparing Action Priority */

    if (set_attach_info_p->action_info[0].priority != get_attach_info.attach_basic_info.action_info[0].priority)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Set_attach_info action priority (%d) is not the same as Get_attach_info action priority(%d)\r\n",
                     set_attach_info_p->action_info[0].priority,
                     get_attach_info.attach_basic_info.action_info[0].priority);
    }

    /** Comparing the single DNX Action */
    if (set_attach_info_p->dnx_actions[0] != get_attach_info.attach_basic_info.dnx_actions[0])
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Set_attach_info dnx_action (%s) is not the same as Get_attach_info dnx_action (%s)\r\n",
                     dnx_field_dnx_action_text(unit, set_attach_info_p->dnx_actions[0]),
                     dnx_field_dnx_action_text(unit, get_attach_info.attach_basic_info.dnx_actions[0]));
    }

    /** Comparing the single DNX Qualifier */
    if (set_attach_info_p->dnx_quals[0] != get_attach_info.attach_basic_info.dnx_quals[0])
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Set_attach_info dnx_qual (%s) is not the same as Get_attach_info dnx_qual (%s)\r\n",
                     dnx_field_dnx_qual_text(unit, set_attach_info_p->dnx_quals[0]),
                     dnx_field_dnx_qual_text(unit, get_attach_info.attach_basic_info.dnx_quals[0]));
    }

    /** Comparing the single DNX Qualifier Info - input_type name */
    if (sal_strncmp(dnx_field_input_type_text(set_attach_info_p->qual_info[0].input_type),
                    dnx_field_input_type_text(get_attach_info.attach_basic_info.qual_info[0].input_type),
                    DBAL_MAX_STRING_LENGTH))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Set_attach_info qual input_type name %s is not the same as Get_attach_info qual input_type name %s\r\n",
                     dnx_field_input_type_text(set_attach_info_p->qual_info[0].input_type),
                     dnx_field_input_type_text(get_attach_info.attach_basic_info.qual_info[0].input_type));
    }

    /** Comparing the single DNX Qualifier Info - input_type */
    if (set_attach_info_p->qual_info[0].input_type != get_attach_info.attach_basic_info.qual_info[0].input_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Set_attach_info qual input_type (0x%08X) is not the same as Get_attach_info qual input_type (0x%08X)\r\n",
                     set_attach_info_p->qual_info[0].input_type,
                     get_attach_info.attach_basic_info.qual_info[0].input_type);
    }

    /** Comparing the single DNX Qualifier Info - input_arg */
    if (set_attach_info_p->qual_info[0].input_arg != get_attach_info.attach_basic_info.qual_info[0].input_arg)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Set_attach_info qual input_arg (0x%08X) is not the same as Get_attach_info qual input_arg (0x%08X)\r\n",
                     set_attach_info_p->qual_info[0].input_arg,
                     get_attach_info.attach_basic_info.qual_info[0].input_arg);
    }

    /** Comparing the single DNX Qualifier Info - offset */
    if (set_attach_info_p->qual_info[0].offset != get_attach_info.attach_basic_info.qual_info[0].offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Set_attach_info qual offset (0x%08X) is not the same as Get_attach_info qual offset (0x%08X)\r\n",
                     set_attach_info_p->qual_info[0].offset, get_attach_info.attach_basic_info.qual_info[0].offset);
    }

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), Line %d, .Finished comparing all dnx_field_group_attach_info_t fields %s%s\r\n",
                __FUNCTION__, __LINE__, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Function to  call 'bcm_field_group_context_get' using the supplied 'fg_id' and 'context_id'
 *  Which were used in the corresponding 'set' operation (bcm_field_group_context_attach())
 *  We compare it to the 'set_attach_info_p' supplied to the function.
 *  We have a single qualifier and single action set in the attach info, hence why
 *  we only compare the first element of the corresponding arrays.
 * \param [in] unit             - Device ID
 * \param [in] context_id       - Context ID for attach
 * \param [in] fg_id            - Field Group ID.
 * \param [out] set_attach_info_p   - Context Attach info to be compared against.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_context_attach_compare_bcm(
    int unit,
    bcm_field_context_t context_id,
    bcm_field_group_t fg_id,
    bcm_field_group_attach_info_t * set_attach_info_p)
{
    bcm_field_group_attach_info_t get_attach_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_attach_info_t_init(&get_attach_info);

    LOG_INFO_EX(BSL_LOG_MODULE, "%s();, Line %d, .Performing group context get to field group: %d and context %d\r\n",
                __FUNCTION__, __LINE__, fg_id, context_id);

    SHR_IF_ERR_EXIT(bcm_field_group_context_info_get(unit, fg_id, context_id, &get_attach_info));

    LOG_INFO_EX(BSL_LOG_MODULE,
                "%s(), Line %d .Performing compare of all bcm_field_group_attach_info_t fields %s%s\r\n", __FUNCTION__,
                __LINE__, EMPTY, EMPTY);

    /** Comparing Action Priority */
    if (set_attach_info_p->payload_info.action_info[0].priority != get_attach_info.payload_info.action_info[0].priority)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Set_attach_info action priority (%d) is not the same as Get_attach_info action priority(%d)\r\n",
                     set_attach_info_p->payload_info.action_info[0].priority,
                     get_attach_info.payload_info.action_info[0].priority);
    }

    /** Comparing the single BCM Action */
    if (set_attach_info_p->payload_info.action_types[0] != get_attach_info.payload_info.action_types[0])
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Set_attach_info bcm_action (%s) is not the same as Get_attach_info bcm_action (%s)\r\n",
                     dnx_field_bcm_action_text(unit, set_attach_info_p->payload_info.action_types[0]),
                     dnx_field_bcm_action_text(unit, get_attach_info.payload_info.action_types[0]));
    }

    /** Comparing the single BCM Qualifier */
    if (set_attach_info_p->key_info.qual_types[0] != get_attach_info.key_info.qual_types[0])
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Set_attach_info bcm_qual (%s) is not the same as Get_attach_info bcm_qual (%s)\r\n",
                     dnx_field_bcm_qual_text(unit, set_attach_info_p->key_info.qual_types[0]),
                     dnx_field_bcm_qual_text(unit, get_attach_info.key_info.qual_types[0]));
    }

/*
 * {
 */
#if 0
    /** Comparing the single BCM Qualifier Info - input_type */
    if (set_attach_info_p->key_info.qual_info[0].input_type != get_attach_info.key_info.qual_info[0].input_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Set_attach_info qual input_type %d is not the same as Get_attach_info qual input_type %d\r\n",
                     set_attach_info_p->key_info.qual_info[0].input_type,
                     get_attach_info.key_info.qual_info[0].input_type);
    }

    /** Comparing the single BCM Qualifier Info - input_arg */
    if (set_attach_info_p->key_info.qual_info[0].input_arg != get_attach_info.key_info.qual_info[0].input_arg)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Set_attach_info qual input_arg %d is not the same as Get_attach_info qual input_arg %d\r\n",
                     set_attach_info_p->key_info.qual_info[0].input_arg,
                     get_attach_info.key_info.qual_info[0].input_arg);
    }

    /** Comparing the single BCM Qualifier Info - offset */
    if (set_attach_info_p->key_info.qual_info[0].offset != get_attach_info.key_info.qual_info[0].offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Set_attach_info qual offset %d is not the same as Get_attach_info qual offset %d\r\n",
                     set_attach_info_p->key_info.qual_info[0].offset, get_attach_info.key_info.qual_info[0].offset);
    }

#endif
/*
 * }
 */

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), Line %d, .Finished comparing all bcm_field_group_attach_info_t fields %s%s\r\n",
                __FUNCTION__, __LINE__, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Function that attempts to create an invalid configuration with Qualifier params.
 *  It tries to set mismatching input_type and qualifier(class).
 *
 * \param [in] unit             - Device ID
 * \param [in] stage            - Field stage for FG creation and Context attach.
 * \param [in] qual_action_info - Full qualifier and action info needed.
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_context_attach_mismatching_params(
    int unit,
    dnx_field_stage_e stage,
    ctest_dnx_field_context_attach_info_per_stage_t * qual_action_info)
{
    int rv;
    dnx_field_group_t dnx_fg_id;
    dnx_field_group_attach_info_t dnx_attach_info;
    dnx_field_group_info_t dnx_fg_info;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    /** Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    flags = 0;

    /** Creating a Field Group to be attached to the context */
    SHR_IF_ERR_EXIT(ctest_dnx_field_context_attach_fg_add_dnx(unit, stage, qual_action_info, &dnx_fg_info, &dnx_fg_id));

    /** Copying the FG info qualifiers and actions to the attach info */
    sal_memcpy(dnx_attach_info.dnx_quals, dnx_fg_info.dnx_quals, sizeof(dnx_attach_info.dnx_quals));
    sal_memcpy(dnx_attach_info.dnx_actions, dnx_fg_info.dnx_actions, sizeof(dnx_attach_info.dnx_actions));

   /** Giving random mid-range priority, the value can be changed, it will not influence the test */
    dnx_attach_info.action_info[0].priority = CTEST_DNX_FIELD_CONTEXT_ATTACH_ACTION_PRIORITY;

   /** Checking the Qualifier class of the set qualifier and add the wrong qualifier info */
    if (DNX_QUAL_CLASS(dnx_attach_info.dnx_quals[0]) == DNX_FIELD_QUAL_CLASS_HEADER
        || DNX_QUAL_CLASS(dnx_attach_info.dnx_quals[0]) == DNX_FIELD_QUAL_CLASS_LAYER_RECORD)
    {
        dnx_attach_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
        dnx_attach_info.qual_info[0].input_arg = DNX_FIELD_ATTACH_INFO_FIELD_INPUT_ARG_DEFAULT;
        dnx_attach_info.qual_info[0].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;
    }
    else if (DNX_QUAL_CLASS(dnx_attach_info.dnx_quals[0]) == DNX_FIELD_QUAL_CLASS_META
             || DNX_QUAL_CLASS(dnx_attach_info.dnx_quals[0]) == DNX_FIELD_QUAL_CLASS_META2)
    {
        dnx_attach_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_LAYER_FWD;
        dnx_attach_info.qual_info[0].input_arg = CTEST_DNX_FIELD_CONTEXT_ATTACH_QUAL_INPUT_ARG_AND_OFFSET;
        dnx_attach_info.qual_info[0].offset = CTEST_DNX_FIELD_CONTEXT_ATTACH_QUAL_INPUT_ARG_AND_OFFSET;
    }
    LOG_INFO_EX(BSL_LOG_MODULE, "Line %d, Stage: %s .Performing Context Attach on Context: %d and Field Group: %d \r\n",
                __LINE__, dnx_field_stage_text(unit, stage), DNX_FIELD_CONTEXT_ID_DEFAULT(unit), dnx_fg_id);

    rv = dnx_field_group_context_attach(unit, flags, dnx_fg_id, DNX_FIELD_CONTEXT_ID_DEFAULT(unit), &dnx_attach_info);
    if (rv == _SHR_E_PARAM)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "SUCCESS ON NEGATIVE TEST: Line %d, Stage: %s ."
                    "                    Context was not attached with mismatching QUAL_CLASS(%s) and INPUT_TYPE(%s) \r\n",
                    __LINE__, dnx_field_stage_text(unit, stage),
                    dnx_field_qual_class_text(DNX_QUAL_CLASS(dnx_attach_info.dnx_quals[0])),
                    dnx_field_input_type_text(dnx_attach_info.qual_info[0].input_type));
    }
    else
    {
        SHR_SET_CURRENT_ERR(rv);
    }
exit:
    SHR_SET_CURRENT_ERR(bcm_field_group_delete(unit, dnx_fg_id));
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Function that tries to attach a field group with a zero key to epmf.
 *
 * \param [in] unit             - Device ID
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_context_attach_zero_key_epmf(
    int unit)
{
    int rv;
    dnx_field_group_t dnx_fg_id = DNX_FIELD_GROUP_INVALID;
    dnx_field_group_attach_info_t dnx_attach_info;
    dnx_field_group_info_t dnx_fg_info;
    uint32 attach_flags = 0;
    bcm_field_qualify_t bcm_qual = bcmFieldQualifyCount;
    dnx_field_qualifier_in_info_t qual_info;
    dnx_field_qual_t dnx_qual = DNX_FIELD_QUAL_TYPE_INVALID;
    dnx_field_stage_e stage = DNX_FIELD_STAGE_EPMF;

    SHR_FUNC_INIT_VARS(unit);

    /** Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

    /*
     * Create user defined qualifier.
     */
    dnx_field_qual_in_info_init(unit, &qual_info);
    sal_strncpy_s(qual_info.name, "zero_qual_neg", sizeof(qual_info.name));
    qual_info.size = 5;
    SHR_IF_ERR_EXIT(dnx_field_qual_create(unit, 0, &qual_info, &bcm_qual));
    SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, stage, bcm_qual, &dnx_qual));

    /** Creating a Field Group to be attached to the context */
    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &dnx_fg_info));
    dnx_fg_info.fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    dnx_fg_info.field_stage = stage;
    dnx_fg_info.dnx_quals[0] = dnx_qual;
    dnx_fg_info.dnx_actions[0] = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, stage, DBAL_ENUM_FVAL_EPMF_ACTION_TC);
    sal_strncpy(dnx_fg_info.name, "zero_key_neg", sizeof(dnx_fg_info.name));
    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, 0, &dnx_fg_info, &dnx_fg_id));

    /** Copying the FG info qualifiers and actions to the attach info */
    sal_memcpy(dnx_attach_info.dnx_quals, dnx_fg_info.dnx_quals, sizeof(dnx_attach_info.dnx_quals));
    sal_memcpy(dnx_attach_info.dnx_actions, dnx_fg_info.dnx_actions, sizeof(dnx_attach_info.dnx_actions));

   /** Giving random mid-range priority, the value can be changed, it will not influence the test */
    dnx_attach_info.action_info[0].priority = CTEST_DNX_FIELD_CONTEXT_ATTACH_ACTION_PRIORITY;

   /** Attaching const zero to qualifier. */
    dnx_attach_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_CONST;
    dnx_attach_info.qual_info[0].input_arg = 0;
    LOG_INFO_EX(BSL_LOG_MODULE, "Line %d, Stage: %s .Performing Context Attach on Context: %d and Field Group: %d \r\n",
                __LINE__, dnx_field_stage_text(unit, stage), DNX_FIELD_CONTEXT_ID_DEFAULT(unit), dnx_fg_id);

    rv = dnx_field_group_context_attach(unit, attach_flags, dnx_fg_id, DNX_FIELD_CONTEXT_ID_DEFAULT(unit),
                                        &dnx_attach_info);
    if (rv == _SHR_E_PARAM)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "SUCCESS ON NEGATIVE TEST: Line %d, Stage: %s ."
                    "                    Context was not attached with mismatching QUAL_CLASS(%s) and INPUT_TYPE(%s) \r\n",
                    __LINE__, dnx_field_stage_text(unit, stage),
                    dnx_field_qual_class_text(DNX_QUAL_CLASS(dnx_attach_info.dnx_quals[0])),
                    dnx_field_input_type_text(dnx_attach_info.qual_info[0].input_type));
    }
    else
    {
        SHR_SET_CURRENT_ERR(rv);
    }
exit:
    SHR_SET_CURRENT_ERR(bcm_field_group_delete(unit, dnx_fg_id));
    SHR_SET_CURRENT_ERR(dnx_field_qual_destroy(unit, bcm_qual));
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Simple Case of context attach/detach - we create a Field group and a context.
 *  The field group is attached to the context with valid attach information.
 *  Then we call a compare function, which calls a GET and compares the SET and GET values of the attach info.
 *
 * \param [in] unit -
 *  Device ID
 * \param [in] args - Extra arguments
 * \param [in] sand_control - Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
appl_dnx_field_context_attach_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_field_context_t bcm_context_id;
    uint32 bcm_context_flags;
    bcm_field_stage_t bcm_field_stage;
    bcm_field_group_attach_info_t bcm_attach_info;
    bcm_field_context_info_t bcm_context_info;
    bcm_field_group_info_t bcm_fg_info;
    bcm_field_group_t bcm_fg_id;

    dnx_field_context_mode_t dnx_context_mode;
    dnx_field_context_flags_e dnx_context_flags;
    dnx_field_context_t dnx_context_id;
    dnx_field_group_t dnx_fg_id;
    dnx_field_stage_e dnx_field_stage;
    dnx_field_group_attach_info_t dnx_attach_info;
    dnx_field_group_info_t dnx_fg_info;
    bsl_severity_t original_severity_fldprocdnx;
    bsl_severity_t original_severity_swstate;

    int field_context_test_clean;
    unsigned int test_type;

    SHR_FUNC_INIT_VARS(unit);

    test_type = 0;
    field_context_test_clean = 0;

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);

    SH_SAND_GET_BOOL(CTEST_DNX_FIELD_CONTEXT_ATTACH_OPTION_CLEAN, field_context_test_clean);
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_CONTEXT_ATTACH_OPTION_TEST_STAGE, dnx_field_stage);
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_CONTEXT_ATTACH_OPTION_TYPE, test_type);

    bcm_context_id = dnx_context_id = 0;
    bcm_context_flags = 0;
    dnx_context_flags = 0;

    if (test_type == 0)
    {
        bcm_field_context_info_t_init(&bcm_context_info);

       /** Convert DNX to BCM Field Stage */
        SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, dnx_field_stage, &bcm_field_stage));
        if (dnx_field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            bcm_context_info.cascaded_from = BCM_FIELD_CONTEXT_ID_DEFAULT;
        }
        SHR_IF_ERR_EXIT(bcm_field_context_create
                        (unit, bcm_context_flags, bcm_field_stage, &bcm_context_info, &bcm_context_id));

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Context=%d  created \n %s%s%s", bcm_context_id, EMPTY, EMPTY, EMPTY);

       /** Creating a Field Group to be attached to the context */
        SHR_IF_ERR_EXIT(ctest_dnx_field_context_attach_fg_add_bcm
                        (unit, bcm_field_stage, &Qual_bcm_action_info[dnx_field_stage], &bcm_fg_info, &bcm_fg_id));

       /** Attaching the created Field Group to the context */
        SHR_IF_ERR_EXIT(ctest_dnx_field_context_attach_ca_bcm
                        (unit, bcm_context_id, bcm_fg_id, &Qual_bcm_action_info[dnx_field_stage], &bcm_fg_info,
                         &bcm_attach_info));

       /**
        * Sending the attach_info with which we have attach the field group to the context
        * to be compared with the values from the GET function inside ctest_dnx_context_attach_compare_bcm.
        */
        SHR_IF_ERR_EXIT(ctest_dnx_context_attach_compare_bcm(unit, bcm_context_id, bcm_fg_id, &bcm_attach_info));

    }
    else if (test_type == 1)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &dnx_context_mode));

        if (dnx_field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            dnx_context_mode.context_ipmf2_mode.cascaded_from = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);
        }
        SHR_IF_ERR_EXIT(dnx_field_context_create
                        (unit, dnx_context_flags, dnx_field_stage, &dnx_context_mode, &dnx_context_id));

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Context=%d  created \n %s%s%s", dnx_context_id, EMPTY, EMPTY, EMPTY);

        /** Creating a Field Group to be attached to the context */
        SHR_IF_ERR_EXIT(ctest_dnx_field_context_attach_fg_add_dnx
                        (unit, dnx_field_stage, &Qual_action_info[dnx_field_stage], &dnx_fg_info, &dnx_fg_id));

        /** Attaching the created Field Group to the context */
        SHR_IF_ERR_EXIT(ctest_dnx_field_context_attach_ca_dnx
                        (unit, dnx_context_id, dnx_fg_id, &dnx_fg_info, &dnx_attach_info));

        /**
         * Sending the attach_info with which we have attach the field group to the context
         * to be compared with the values from the GET function inside ctest_dnx_context_attach_compare.
         */
        SHR_IF_ERR_EXIT(ctest_dnx_context_attach_compare_dnx(unit, dnx_context_id, dnx_fg_id, &dnx_attach_info));

/*
 * ---------------------------------------------------------- NEGATIVE TESTING ------------------------------------------------------
 */

        /*
         * Case: Mismatching qualifier class and input type in attach info.
         */
        SHR_IF_ERR_EXIT(ctest_dnx_context_attach_mismatching_params
                        (unit, dnx_field_stage, &Qual_action_info[dnx_field_stage]));

        /*
         * Case: Attaching zero key in ePMF
         */
        if (dnx_field_stage == DNX_FIELD_STAGE_EPMF)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_context_attach_zero_key_epmf(unit));
        }



        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Line %d, Stage: %s .Performing context detach  to field group: %d and context %d\r\n", __LINE__,
                    dnx_field_stage_text(unit, dnx_field_stage), dnx_fg_id, dnx_context_id);

        SHR_IF_ERR_EXIT(dnx_field_group_context_detach(unit, dnx_fg_id, dnx_context_id));

        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Line %d, Stage: %s .Successfully performed context detach  to field group: %d and context %d\r\n",
                    __LINE__, dnx_field_stage_text(unit, dnx_field_stage), dnx_fg_id, dnx_context_id);
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %d (for 'test_type') is not implemented. Illegal parameter. %s\r\n",
                    __FUNCTION__, __LINE__, test_type, EMPTY);
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    if (test_type == 0 && field_context_test_clean == 1)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Line %d, Stage: %s .Performing context detach  to field group: %d and context %d\r\n", __LINE__,
                    dnx_field_bcm_stage_text(bcm_field_stage), bcm_fg_id, bcm_context_id);
        CTEST_DNX_FIELD_UTIL_ERR(bcm_field_group_context_detach(unit, bcm_fg_id, bcm_context_id));
        CTEST_DNX_FIELD_UTIL_ERR(bcm_field_group_delete(unit, bcm_fg_id));
        CTEST_DNX_FIELD_UTIL_ERR(bcm_field_context_destroy(unit, bcm_field_stage, bcm_context_id));

        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Line %d, Stage: %s .Successfully performed context detach  to field group: %d and context %d\r\n",
                    __LINE__, dnx_field_bcm_stage_text(bcm_field_stage), bcm_fg_id, bcm_context_id);
    }
    else if (test_type == 1 && field_context_test_clean == 1)
    {
        CTEST_DNX_FIELD_UTIL_ERR(ctest_dnx_field_util_clean(unit));
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - run context init sequence in diag shell
 *
 * \param [in] unit - the unit number in system
 * \param [in] args - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "dnx context_start"
 */
shr_error_e
sh_dnx_field_context_attach_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(CTEST_DNX_FIELD_CONTEXT_ATTACH_OPTION_COUNT, count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_context_attach_run(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}

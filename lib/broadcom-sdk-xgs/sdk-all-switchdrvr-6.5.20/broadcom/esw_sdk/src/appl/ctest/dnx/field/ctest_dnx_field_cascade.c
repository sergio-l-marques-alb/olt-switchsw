/** \file diag_dnx_field_cascade.c
 * $Id$
 *
 * Trajectory iPMF use-case for DNX.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
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
#include <sal/appl/sal.h>
/** soc */
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>
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
 *   Keyword for TCL testing, clean can be either 0 or 1,
 *   if the ctest will clean-up after its finished setting-up.
 */
#define DNX_DIAG_FIELD_CASCADE_OPTION_CLEAN        "clean"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define DNX_DIAG_ACTION_OPTION_TEST_COUNT        "count"
/**
 * \brief
 *   Keyword for test type
 */
#define DNX_DIAG_CASCADING_OPTION_TEST_TYPE         "type"

#define TEST_TYPE_CASC_IPMF1_IPMF2                  "IPMF1_IPMF2"
#define TEST_TYPE_CASC_IPMF1_IPMF2_SHARE            "IPMF1_IPMF2_SHARE"
#define TEST_TYPE_CASC_IPMF1_IPMF3                  "IPMF1_IPMF3"

/**
 * \brief
 *   Options list for 'cascade' shell command
 * \remark
 */
sh_sand_option_t dnx_field_cascade_options[] = {
     /* Name */                                 /* Type */              /* Description */                       /* Default */
    {DNX_DIAG_CASCADING_OPTION_TEST_TYPE,       SAL_FIELD_TYPE_STR,     "Type of test (ipmf1-2 or ipmf1-3)",    "IPMF1_IPMF3" },
    {DNX_DIAG_FIELD_CASCADE_OPTION_CLEAN,       SAL_FIELD_TYPE_BOOL,    "Will test perform cleanup or not",         "Yes"},
    {DNX_DIAG_ACTION_OPTION_TEST_COUNT,         SAL_FIELD_TYPE_UINT32,  "Number of times test will run",            "1"},
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'cascade' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t sh_dnx_field_cascade_tests[] = {
    {"DNX_cascade_iPMF1_to_iPMF2",       "type=IPMF1_IPMF2 clean=yes count=6",       CTEST_POSTCOMMIT},
    {"DNX_cascade_iPMF1_to_iPMF2_SHARE", "type=IPMF1_IPMF2_SHARE clean=yes count=6", CTEST_POSTCOMMIT},
    {"DNX_cascade_iPMF1_to_iPMF3",       "type=IPMF1_IPMF3 clean=yes count=6",       CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/**
 *  cascade shell command leaf details
 */
sh_sand_man_t sh_dnx_field_cascade_man = {
    "Create cascading configuration between iPMF1 to iPMF2/iPMF3",
    "Using action of container to pass information.\n"
        "type IPMF1_IPMF2 performs cascading from iPMF1 to iPMF2.\n"
        "type IPMF1_IPMF2 performs cascading from iPMF1 to iPMF3.\n"
        "type IPMF1_IPMF2_SHARE performs cascading from iPMF1 to iPMF2 and assumes all actions should be shared.\n",
    "ctest cascade",
    "clean=YES"
};

/**Container Value is Action of iPMF1 and Qualifier of iPMF3*/
#define DIAG_CASCADE_CONTAINER_VALUE    (3)

/**The output to check at iPMF2/iPMF3*/
#define DIAG_CASCADE_IPMF_DP_OUTPUT    (1)

/** Number of qualifier and action configured for iPMF1*/
#define DIAG_CASCADE_NOF_QUAL_IPMF1     (1)
#define DIAG_CASCADE_NOF_ACTION_IPMF1   (1)

/** Number of qualifier and action qualifier configured for iPMF3*/
#define DIAG_CASCADE_NOF_QUAL_IPMF2     (3)
#define DIAG_CASCADE_NOF_ACTION_IPMF2   (1)

/** Number of qualifier and action qualifier configured for iPMF3*/
#define DIAG_CASCADE_NOF_QUAL_IPMF3     (1)
#define DIAG_CASCADE_NOF_ACTION_IPMF3   (1)

/** Qualifier and action type configured for iPMF1*/
#define DIAG_CASCADE_IPMF1_QUAL_DP            (DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_DP))
#define DIAG_CASCADE_IPMF1_ACTION_CONTAINER   (DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1, DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA3))

/** Qualifier and action type configured for iPMF2*/
#define DIAG_CASCADE_IPMF2_QUAL_CONTAINER_1   (DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_KEY_F_INITIAL))
#define DIAG_CASCADE_IPMF2_QUAL_CONTAINER_2   (DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_HEADER_QUAL_MAC_DST))
#define DIAG_CASCADE_IPMF2_ACTION_DP          (DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA))

/** Qualifier and action type configured for iPMF3*/
#define DIAG_CASCADE_IPMF3_QUAL_CONTAINER     (DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_GENERAL_DATA_PMF_CONTAINER))
#define DIAG_CASCADE_IPMF3_ACTION_DP          (DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF3, DBAL_ENUM_FVAL_IPMF3_ACTION_DP))

static shr_error_e
appl_dnx_cascade_ipmf_presel_config(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context,
    uint8 entry_valid,
    dnx_field_presel_t * presel_id_p)
{
    dnx_field_presel_entry_id_t p_id;
    dnx_field_presel_entry_data_t p_data;
    SHR_FUNC_INIT_VARS(unit);

    p_id.presel_id = *presel_id_p;
    p_id.stage = stage;
    p_data.entry_valid = entry_valid;
    p_data.context_id = context;
    p_data.nof_qualifiers = 1;
    p_data.qual_data[0].qual_type_dbal_field =
        (stage == DNX_FIELD_STAGE_IPMF2) ? DBAL_FIELD_STATE_TABLE_DATA_READ : DBAL_FIELD_ACL_CONTEXT_PROFILE;
    p_data.qual_data[0].qual_value = 0x0;
    p_data.qual_data[0].qual_mask = 0xFF;
    /**Set CS for iPMF1*/
    SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, 0, &p_id, &p_data));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_cascade_ipmf2_fg_add(
    int unit,
    dnx_field_group_t * fg_id_p,
    dnx_field_group_info_t * fg_info_p,
    bcm_field_qualify_t * bcm_qual_p)
{
    dnx_field_qualifier_in_info_t qual_info;
    dnx_field_qual_t dnx_qual;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, fg_info_p));
    fg_info_p->field_stage = DNX_FIELD_STAGE_IPMF2;
    fg_info_p->fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    fg_info_p->dnx_quals[0] = DIAG_CASCADE_IPMF2_QUAL_CONTAINER_1;
    fg_info_p->dnx_quals[1] = DIAG_CASCADE_IPMF2_QUAL_CONTAINER_2;

    /*
     * Create an user-qualifier in ipmf2 that will parse the action buffer received from ipmf1 (cascaded)
     */
    SHR_IF_ERR_EXIT(dnx_field_qual_in_info_init(unit, &qual_info));
    qual_info.size = 64;
    sal_strncpy_s(qual_info.name, "CASCADED_ACTION_QUAL", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(dnx_field_qual_create(unit, 0, &qual_info, bcm_qual_p));
    SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, DNX_FIELD_STAGE_IPMF2, *bcm_qual_p, &dnx_qual));

    fg_info_p->dnx_quals[2] = dnx_qual;

    /*
     * Set first action on first '2msb' combination' to be DP. All
     * other entries have been filled in in 'dnx_field_group_info_t_init' above.
     */
    fg_info_p->dnx_actions[0] = DIAG_CASCADE_IPMF2_ACTION_DP;

    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, DNX_FIELD_GROUP_ADD_FLAG_WITH_ID, fg_info_p, fg_id_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_cascade_ipmf3_fg_add(
    int unit,
    dnx_field_group_t * fg_id_p,
    dnx_field_group_info_t * fg_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, fg_info_p));
    fg_info_p->field_stage = DNX_FIELD_STAGE_IPMF3;
    fg_info_p->fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    fg_info_p->dnx_quals[0] = DIAG_CASCADE_IPMF3_QUAL_CONTAINER;
    /*
     * Set first action on first '2msb' combination' to be DP. All
     * other entries have been filled in in 'dnx_field_group_info_t_init' above.
     */
    fg_info_p->dnx_actions[0] = DIAG_CASCADE_IPMF3_ACTION_DP;

    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, DNX_FIELD_GROUP_ADD_FLAG_WITH_ID, fg_info_p, fg_id_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_cascade_ipmf_fg_attach(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_group_t fg_id,
    dnx_field_group_t fg_id_ipmf1,
    dnx_field_group_info_t * fg_info_p,
    dnx_field_context_t context)
{
    dnx_field_group_attach_info_t a_info;
    uint32 nof_qual_ipmf, nof_actions_ipmf;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &a_info));

    nof_qual_ipmf =
        (stage ==
         DNX_FIELD_STAGE_IPMF1) ? DIAG_CASCADE_NOF_QUAL_IPMF1 : ((stage ==
                                                                  DNX_FIELD_STAGE_IPMF2) ? DIAG_CASCADE_NOF_QUAL_IPMF2 :
                                                                 DIAG_CASCADE_NOF_QUAL_IPMF3);
    nof_actions_ipmf =
        (stage ==
         DNX_FIELD_STAGE_IPMF1) ? DIAG_CASCADE_NOF_ACTION_IPMF1 : ((stage ==
                                                                    DNX_FIELD_STAGE_IPMF2) ?
                                                                   DIAG_CASCADE_NOF_ACTION_IPMF2 :
                                                                   DIAG_CASCADE_NOF_ACTION_IPMF3);

    sal_memcpy(a_info.dnx_quals, fg_info_p->dnx_quals, nof_qual_ipmf * sizeof(dnx_field_qual_t));
    a_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;

    if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        a_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;
        a_info.qual_info[1].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
        a_info.qual_info[1].input_arg = 1;
        a_info.qual_info[1].offset = 20;

        /*
         * Cascaded input type for parsing the action buffer 
         */
        a_info.qual_info[2].input_type = DNX_FIELD_INPUT_TYPE_CASCADED;
        /*
         * fg_id_ipmf1 is the Field group in ipmf1 to use the result of 
         */
        a_info.qual_info[2].input_arg = fg_id_ipmf1;
        /*
         * User may supply the offset inside the action buffer. the size to take from the action is given when the
         * user-qualifier was created.
         * We now place 0 in the offset and 64 in the qualifier size to take the entire payload.
         */
        a_info.qual_info[2].offset = 0;
    }

    sal_memcpy(a_info.dnx_actions, fg_info_p->dnx_actions, nof_actions_ipmf * sizeof(dnx_field_action_t));
    
    a_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 7);

    SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, fg_id, context, &a_info));

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
appl_dnx_cascade_ipmf2_entry_add(
    int unit,
    dnx_field_group_t fg_id,
    uint32 *entry_handle_p)
{
    unsigned int nof_quals;
    uint32 i;
    dnx_field_entry_t entry_in_info;
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_dnx_qual_arr_get(unit, fg_id, dnx_quals, &nof_quals));
    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_in_info));

    for (i = 0; i < DIAG_CASCADE_NOF_QUAL_IPMF2; i++)
    {
        entry_in_info.key_info.qual_info[i].dnx_qual = dnx_quals[i];
        entry_in_info.key_info.qual_info[i].qual_mask[0] = 0xFF;
        entry_in_info.key_info.qual_info[i].qual_mask[1] = 0xFF;
        entry_in_info.key_info.qual_info[i].qual_mask[2] = 0xFF;
        entry_in_info.key_info.qual_info[i].qual_mask[3] = 0xFF;
        entry_in_info.key_info.qual_info[i].qual_value[0] = DIAG_CASCADE_CONTAINER_VALUE;
        entry_in_info.key_info.qual_info[i].qual_value[1] = DIAG_CASCADE_CONTAINER_VALUE;
        entry_in_info.key_info.qual_info[i].qual_value[2] = DIAG_CASCADE_CONTAINER_VALUE;
        entry_in_info.key_info.qual_info[i].qual_value[3] = DIAG_CASCADE_CONTAINER_VALUE;
    }

    entry_in_info.payload_info.action_info[0].dnx_action = DIAG_CASCADE_IPMF2_ACTION_DP;
    entry_in_info.payload_info.action_info[0].action_value[0] = DIAG_CASCADE_IPMF_DP_OUTPUT;
    entry_in_info.priority = 2;

    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, fg_id, &entry_in_info, entry_handle_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_cascade_ipmf3_entry_add(
    int unit,
    dnx_field_group_t fg_id,
    uint32 *entry_handle_p)
{
    dnx_field_entry_t entry_in_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_in_info));
    entry_in_info.key_info.qual_info[0].dnx_qual = DIAG_CASCADE_IPMF3_QUAL_CONTAINER;
    entry_in_info.key_info.qual_info[0].qual_mask[0] = 0xF;
    entry_in_info.key_info.qual_info[0].qual_value[0] = DIAG_CASCADE_CONTAINER_VALUE;
    entry_in_info.payload_info.action_info[0].dnx_action = DIAG_CASCADE_IPMF3_ACTION_DP;
    entry_in_info.payload_info.action_info[0].action_value[0] = DIAG_CASCADE_IPMF_DP_OUTPUT;
    entry_in_info.priority = 2;

    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, fg_id, &entry_in_info, entry_handle_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_cascade_ipmf1_presel_config(
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
    p_data.qual_data[0].qual_type_dbal_field = DBAL_FIELD_ACL_CONTEXT_PROFILE;
    p_data.qual_data[0].qual_value = 0x0;
    p_data.qual_data[0].qual_mask = 0xFF;

    SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, 0, &p_id, &p_data));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_cascade_ipmf1_fg_add(
    int unit,
    dnx_field_group_t * fg_id_p,
    dnx_field_group_info_t * fg_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, fg_info_p));
    fg_info_p->field_stage = DNX_FIELD_STAGE_IPMF1;
    fg_info_p->fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    fg_info_p->dnx_quals[0] = DIAG_CASCADE_IPMF1_QUAL_DP;
    /*
     * Note that 'fg_info_p->dnx_actions' has been initiated in 'dnx_field_group_info_t_init' above.
     * Note that, in this test, we refer to the first '2msb' combination only.
     */
    fg_info_p->dnx_actions[0] = DIAG_CASCADE_IPMF1_ACTION_CONTAINER;

    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, DNX_FIELD_GROUP_ADD_FLAG_WITH_ID, fg_info_p, fg_id_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_cascade_ipmf1_fg_attach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_info_t * fg_info_p,
    dnx_field_context_t context)
{
    dnx_field_group_attach_info_t a_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &a_info));

    sal_memcpy(a_info.dnx_quals, fg_info_p->dnx_quals, DIAG_CASCADE_NOF_QUAL_IPMF1 * sizeof(dnx_field_qual_t));
    a_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;

    sal_memcpy(a_info.dnx_actions, fg_info_p->dnx_actions, DIAG_CASCADE_NOF_QUAL_IPMF1 * sizeof(dnx_field_action_t));
    
    a_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 7);

    SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, fg_id, context, &a_info));

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
appl_dnx_cascade_ipmf1_entry_add(
    int unit,
    dnx_field_group_t fg_id,
    uint32 *entry_handle_p)
{
    dnx_field_entry_t entry_in_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_in_info));

    entry_in_info.key_info.qual_info[0].dnx_qual = DIAG_CASCADE_IPMF1_QUAL_DP;
    entry_in_info.key_info.qual_info[0].qual_mask[0] = 0xF;
    entry_in_info.key_info.qual_info[0].qual_value[0] = 0;
    entry_in_info.payload_info.action_info[0].dnx_action = DIAG_CASCADE_IPMF1_ACTION_CONTAINER;
    entry_in_info.payload_info.action_info[0].action_value[0] = DIAG_CASCADE_CONTAINER_VALUE;
    entry_in_info.priority = 2;

    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, fg_id, &entry_in_info, entry_handle_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_cascade_ipmf1_ipmf2_fes_compare(
    int unit,
    dnx_field_context_t context_ipmf1,
    dnx_field_context_t context_ipmf2,
    int test_sharing)
{
    dnx_field_actions_fes_context_get_info_t fes_info_ipmf1;
    dnx_field_actions_fes_context_get_info_t fes_info_ipmf2;
    unsigned int fes_quartet_ndx;
    unsigned int fes_2msb_comb;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get
                    (unit, DNX_FIELD_STAGE_IPMF1, context_ipmf1, &fes_info_ipmf1));
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_ipmf1, &fes_info_ipmf2));

    if (fes_info_ipmf1.nof_fes_quartets != fes_info_ipmf2.nof_fes_quartets)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of FES quartets in cascaded context immediately after creation isn't "
                     "the same as the context we cascade from (iPMF1 %d iPMF2 %d)\r\n",
                     fes_info_ipmf1.nof_fes_quartets, fes_info_ipmf2.nof_fes_quartets);
    }
    /*
     * Verify that all actions are shared (Note that that is not necessarily the case).
     */
    if (test_sharing)
    {
        for (fes_quartet_ndx = 0; fes_quartet_ndx < fes_info_ipmf1.nof_fes_quartets; fes_quartet_ndx++)
        {
            if (fes_info_ipmf1.fes_quartets[fes_quartet_ndx].fes_id !=
                fes_info_ipmf2.fes_quartets[fes_quartet_ndx].fes_id
                || fes_info_ipmf1.fes_quartets[fes_quartet_ndx].fes_pgm_id !=
                fes_info_ipmf2.fes_quartets[fes_quartet_ndx].fes_pgm_id
                || fes_info_ipmf1.fes_quartets[fes_quartet_ndx].priority !=
                fes_info_ipmf2.fes_quartets[fes_quartet_ndx].priority
                || fes_info_ipmf1.fes_quartets[fes_quartet_ndx].fg_id !=
                fes_info_ipmf2.fes_quartets[fes_quartet_ndx].fg_id
                || fes_info_ipmf1.fes_quartets[fes_quartet_ndx].place_in_fg !=
                fes_info_ipmf2.fes_quartets[fes_quartet_ndx].place_in_fg
                || fes_info_ipmf1.fes_quartets[fes_quartet_ndx].key_select !=
                fes_info_ipmf2.fes_quartets[fes_quartet_ndx].key_select)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cascaded context FES configuration not identical to "
                             "cascading context. Perhaps sharing is not as naively expected.\r\n");
            }
            for (fes_2msb_comb = 0; fes_2msb_comb < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes_2msb_comb++)
            {
                if (fes_info_ipmf1.fes_quartets[fes_quartet_ndx].fes2msb_info[fes_2msb_comb].action_type !=
                    fes_info_ipmf2.fes_quartets[fes_quartet_ndx].fes2msb_info[fes_2msb_comb].action_type
                    || fes_info_ipmf1.fes_quartets[fes_quartet_ndx].fes2msb_info[fes_2msb_comb].valid_bits !=
                    fes_info_ipmf2.fes_quartets[fes_quartet_ndx].fes2msb_info[fes_2msb_comb].valid_bits
                    || fes_info_ipmf1.fes_quartets[fes_quartet_ndx].fes2msb_info[fes_2msb_comb].shift !=
                    fes_info_ipmf2.fes_quartets[fes_quartet_ndx].fes2msb_info[fes_2msb_comb].shift
                    || fes_info_ipmf1.fes_quartets[fes_quartet_ndx].fes2msb_info[fes_2msb_comb].type !=
                    fes_info_ipmf2.fes_quartets[fes_quartet_ndx].fes2msb_info[fes_2msb_comb].type
                    || fes_info_ipmf1.fes_quartets[fes_quartet_ndx].fes2msb_info[fes_2msb_comb].polarity !=
                    fes_info_ipmf2.fes_quartets[fes_quartet_ndx].fes2msb_info[fes_2msb_comb].polarity
                    || fes_info_ipmf1.fes_quartets[fes_quartet_ndx].fes2msb_info[fes_2msb_comb].chosen_mask !=
                    fes_info_ipmf2.fes_quartets[fes_quartet_ndx].fes2msb_info[fes_2msb_comb].chosen_mask
                    || fes_info_ipmf1.fes_quartets[fes_quartet_ndx].fes2msb_info[fes_2msb_comb].mask !=
                    fes_info_ipmf2.fes_quartets[fes_quartet_ndx].fes2msb_info[fes_2msb_comb].mask)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cascaded context FES configuration not identical to "
                                 "cascading context. Perhaps sharing is not as naively expected.\r\n");

                }
            }
        }
    }

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
appl_dnx_cascade_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_presel_t presel_id_ipmf1 = 22, presel_id_ipmf3 = 22;
    dnx_field_presel_t presel_id_ipmf2 = 21;
    dnx_field_context_mode_t context_mode;
    dnx_field_context_t context_ipmf1;
    dnx_field_group_info_t fg_info_ipmf1;
    dnx_field_group_t fg_id_ipmf1 = 39;
    dnx_field_context_t context_ipmf2;
    dnx_field_context_t context_ipmf3;
    dnx_field_group_info_t fg_info_ipmf2;
    dnx_field_group_info_t fg_info_ipmf3;
    dnx_field_group_t fg_id_ipmf2 = 40;
    dnx_field_group_t fg_id_ipmf3 = 41;
    uint32 entry_handle_ipmf1 = 0;
    uint32 entry_handle_ipmf2 = 0;
    uint32 entry_handle_ipmf3 = 0;
    int field_cascade_test_clean;
    char *cascade_test_type_name = NULL;
    bcm_field_qualify_t udq_bcm_id;

    SHR_FUNC_INIT_VARS(unit);
    field_cascade_test_clean = 0;

    SH_SAND_GET_BOOL("clean", field_cascade_test_clean);

    SH_SAND_GET_STR(DNX_DIAG_CASCADING_OPTION_TEST_TYPE, cascade_test_type_name);

    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
    SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, DNX_FIELD_STAGE_IPMF1, &context_mode, &context_ipmf1));
    SHR_IF_ERR_EXIT(appl_dnx_cascade_ipmf1_presel_config(unit, context_ipmf1, TRUE, &presel_id_ipmf1));
    SHR_IF_ERR_EXIT(appl_dnx_cascade_ipmf1_fg_add(unit, &fg_id_ipmf1, &fg_info_ipmf1));

    SHR_IF_ERR_EXIT(appl_dnx_cascade_ipmf1_fg_attach(unit, fg_id_ipmf1, &fg_info_ipmf1, context_ipmf1));

    SHR_IF_ERR_EXIT(appl_dnx_cascade_ipmf1_entry_add(unit, fg_id_ipmf1, &entry_handle_ipmf1));

    LOG_INFO_EX(BSL_LOG_MODULE,
                "Config iPMF1, context=%d, presel=%d, fg_id=%d,entry_handle=%d  \n ", context_ipmf1,
                presel_id_ipmf1, fg_id_ipmf1, entry_handle_ipmf1);

    if ((sal_strcasecmp(cascade_test_type_name, TEST_TYPE_CASC_IPMF1_IPMF2) == 0)
        || (sal_strcasecmp(cascade_test_type_name, TEST_TYPE_CASC_IPMF1_IPMF2_SHARE) == 0))
    {
        context_mode.context_ipmf2_mode.cascaded_from = context_ipmf1;

        SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, DNX_FIELD_STAGE_IPMF2, &context_mode, &context_ipmf2));

        SHR_IF_ERR_EXIT(appl_dnx_cascade_ipmf1_ipmf2_fes_compare
                        (unit, context_ipmf1, context_ipmf2,
                         (sal_strcasecmp(cascade_test_type_name, TEST_TYPE_CASC_IPMF1_IPMF2_SHARE) == 0)));

        SHR_IF_ERR_EXIT(appl_dnx_cascade_ipmf_presel_config
                        (unit, DNX_FIELD_STAGE_IPMF2, context_ipmf2, TRUE, &presel_id_ipmf2));

        SHR_IF_ERR_EXIT(appl_dnx_cascade_ipmf2_fg_add(unit, &fg_id_ipmf2, &fg_info_ipmf2, &udq_bcm_id));

        SHR_IF_ERR_EXIT(appl_dnx_cascade_ipmf_fg_attach
                        (unit, DNX_FIELD_STAGE_IPMF2, fg_id_ipmf2, fg_id_ipmf1, &fg_info_ipmf2, context_ipmf2));

        SHR_IF_ERR_EXIT(appl_dnx_cascade_ipmf2_entry_add(unit, fg_id_ipmf2, &entry_handle_ipmf2));

        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Config iPMF2, context=%d, presel=%d, fg_id=%d,entry_handle=%d  \n ", context_ipmf2,
                    presel_id_ipmf2, fg_id_ipmf2, entry_handle_ipmf2);
        if (field_cascade_test_clean)
        {
            /*
             * Start cleaning by detaching iPMF2 FG, because it's cascading from (hence, locking) the iPMF1 FG. Which
             * will result in a error being returned when trying to clean iPMF1 FG
             */
            SHR_IF_ERR_EXIT(dnx_field_entry_tcam_delete(unit, fg_id_ipmf2, entry_handle_ipmf2, NULL));
            SHR_IF_ERR_EXIT(dnx_field_group_context_detach(unit, fg_id_ipmf2, context_ipmf2));
            SHR_IF_ERR_EXIT(dnx_field_group_delete(unit, fg_id_ipmf2));
            SHR_IF_ERR_EXIT(dnx_field_qual_destroy(unit, udq_bcm_id));
        }
    }
    else if (sal_strcasecmp(cascade_test_type_name, TEST_TYPE_CASC_IPMF1_IPMF3) == 0)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, DNX_FIELD_STAGE_IPMF3, &context_mode, &context_ipmf3));

        SHR_IF_ERR_EXIT(appl_dnx_cascade_ipmf_presel_config
                        (unit, DNX_FIELD_STAGE_IPMF3, context_ipmf3, TRUE, &presel_id_ipmf3));

        SHR_IF_ERR_EXIT(appl_dnx_cascade_ipmf3_fg_add(unit, &fg_id_ipmf3, &fg_info_ipmf3));

        SHR_IF_ERR_EXIT(appl_dnx_cascade_ipmf_fg_attach
                        (unit, DNX_FIELD_STAGE_IPMF3, fg_id_ipmf3, fg_id_ipmf1, &fg_info_ipmf3, context_ipmf3));

        SHR_IF_ERR_EXIT(appl_dnx_cascade_ipmf3_entry_add(unit, fg_id_ipmf3, &entry_handle_ipmf3));

        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Config iPMF3, context=%d, presel=%d, fg_id=%d,entry_handle=%d  \n ", context_ipmf3,
                    presel_id_ipmf3, fg_id_ipmf3, entry_handle_ipmf3);
        if (field_cascade_test_clean)
        {
            SHR_IF_ERR_EXIT(dnx_field_entry_tcam_delete(unit, fg_id_ipmf3, entry_handle_ipmf3, NULL));
            SHR_IF_ERR_EXIT(dnx_field_group_context_detach(unit, fg_id_ipmf3, context_ipmf3));
            SHR_IF_ERR_EXIT(dnx_field_group_delete(unit, fg_id_ipmf3));
        }
    }

    if (field_cascade_test_clean)
    {
        SHR_IF_ERR_EXIT(dnx_field_entry_tcam_delete(unit, fg_id_ipmf1, entry_handle_ipmf1, NULL));
        SHR_IF_ERR_EXIT(dnx_field_group_context_detach(unit, fg_id_ipmf1, context_ipmf1));
        SHR_IF_ERR_EXIT(dnx_field_group_delete(unit, fg_id_ipmf1));
    }
exit:
    if (field_cascade_test_clean)
    {
        CTEST_DNX_FIELD_UTIL_ERR(ctest_dnx_field_util_clean(unit));
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - run cascade init sequence in diag shell
 *
 * \param [in] unit - the unit number in system
 * \param [in,out] args - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "dnx cascade_start"
 */
shr_error_e
sh_dnx_field_cascade_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32("count", count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_cascade_run(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}

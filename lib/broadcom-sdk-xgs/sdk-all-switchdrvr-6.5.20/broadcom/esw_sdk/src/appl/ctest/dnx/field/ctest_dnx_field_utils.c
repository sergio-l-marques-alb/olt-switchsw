
/** \file ctest_dnx_field_utils.c
 * $Id$
 *
 * DNX level C test Utility functions stored in this file.
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

#define NUM_STAGES_WITH_CONTEXT                 4

 /*
  * Include files.
  * {
  */

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include "ctest_dnx_field_utils.h"
#include <bcm_int/dnx/field/field_actions.h>

/*
 * Defines
 */
/** MAX Number of PMF contexts as define ,device independent */
#define CTEST_DNX_UTIL_MAX_NUM_CONTEXT         dnx_data_field.common_max_val.nof_contexts_get(unit)
/** MAX Number of Context Selection lines as define ,device independent */
#define CTEST_DNX_UTIL_MAX_NUM_CS_LINES         dnx_data_field.common_max_val.nof_cs_lines_get(unit)
/** MAX Number of User defined actions as define ,device independent */
#define CTEST_DNX_UTIL_MAX_NUM_USER_ACTIONS     dnx_data_field.action.user_nof_get(unit)
/** MAX Number of User defined qualifiers as define ,device independent */
#define CTEST_DNX_UTIL_MAX_NUM_USER_QUALS       dnx_data_field.qual.user_nof_get(unit)
/** MAX Number of User defined actions per field group as define ,device independent */
#define CTEST_DNX_UTIL_MAX_NUM_ACTIONS_PER_FG   dnx_data_field.group.nof_action_per_fg_get(unit)
/** MAX Number of User defined qualifiers per field group as define ,device independent */
#define CTEST_DNX_UTIL_MAX_NUM_QUALS_PER_FG     dnx_data_field.group.nof_quals_per_fg_get(unit)

/** Valid BCM qualifiers for "ctest_dnx_field_util_basic_tcam_fg_t_init" */
static bcm_field_qualify_t ctest_bcm_qualifiers_for_init[CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS] =
    { bcmFieldQualifyColor, bcmFieldQualifyCount, bcmFieldQualifyCount };

/** Valid BCM qualifier values for "ctest_dnx_field_util_basic_tcam_fg_t_init" */
static uint32 ctest_bcm_qualifier_values_for_init[CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS] = { 0x1, 0x1, 0x1 };

/** Valid BCM actions for "ctest_dnx_field_util_basic_tcam_fg_t_init" */
static bcm_field_action_t ctest_bcm_actions_for_init[CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS] =
    { bcmFieldActionDropPrecedence, bcmFieldActionCount, bcmFieldActionCount };

/** Valid BCM action values for "ctest_dnx_field_util_basic_tcam_fg_t_init" */
static uint32 ctest_bcm_action_values_for_init[CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS] = { 0x1, 0x1, 0x1 };

static int action_count = 0;
static int qual_count = 0;
/**
 * \brief
 *  This function clears the DBAL table of Context Selection entries,
 *  for all PMF stages.
 *
 * \param [in] unit                                 - Device ID
 * \param [in] context_id_bitmap_for_destroy        - Contexts for destroy bitmap
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ctest_dnx_field_util_presel_clean(
    int unit)
{
    int stage;
    uint32 nof_presels;
    dnx_field_presel_t presel_id;
    dnx_field_presel_entry_id_t entry_id;
    dnx_field_presel_entry_data_t entry_data;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Clear the relevant tables for each PMF stage
     */
    DNX_FIELD_STAGE_CS_QUAL_ITERATOR(stage)
    {
        /*
         * Get the maximum number of presels per stage.
         */
        nof_presels = dnx_data_field.stage.stage_info_get(unit, stage)->nof_cs_lines;
        if (nof_presels <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Stage \"%s\" (%d) has context selection but not supported by cleanup function.\r\n",
                         dnx_field_stage_text(unit, stage), stage);
        }
        /*
         * Delete all presels except the last one (used for default context)
         * and the one used for ITMH_PPH application.
         */
        for (presel_id = 0; presel_id < nof_presels; presel_id++)
        {
            if (presel_id == dnx_data_field.preselector.default_itmh_pph_presel_id_ipmf1_get(unit)
                || presel_id == dnx_data_field.preselector.default_itmh_presel_id_ipmf1_get(unit)
                || presel_id == dnx_data_field.preselector.default_itmh_pph_presel_id_ipmf1_get(unit)
                || presel_id == dnx_data_field.preselector.default_j1_itmh_presel_id_ipmf1_get(unit)
                || presel_id == dnx_data_field.preselector.default_j1_itmh_pph_presel_id_ipmf1_get(unit)
                || presel_id == dnx_data_field.preselector.default_j1_itmh_pph_oamp_presel_id_ipmf1_get(unit)
                || presel_id == dnx_data_field.preselector.default_stacking_presel_id_ipmf1_get(unit)
                || presel_id == dnx_data_field.preselector.default_j1_learn_presel_id_1st_pass_ipmf1_get(unit)
                || presel_id == dnx_data_field.preselector.default_j1_learn_presel_id_2nd_pass_ipmf1_get(unit)
                || presel_id == dnx_data_field.preselector.default_itmh_pph_presel_id_ipmf3_get(unit)
                || presel_id == dnx_data_field.preselector.default_j1_itmh_pph_presel_id_ipmf3_get(unit)
                || presel_id == dnx_data_field.preselector.default_oam_roo_ipv4_presel_id_ipmf3_get(unit)
                || presel_id == dnx_data_field.preselector.default_oam_roo_ipv6_presel_id_ipmf3_get(unit)
                || presel_id == dnx_data_field.preselector.default_oam_w_o_roo_no_rif_ipv4_presel_id_ipmf3_get(unit)
                || presel_id == dnx_data_field.preselector.default_oam_w_o_roo_no_rif_ipv6_presel_id_ipmf3_get(unit)
                || presel_id == dnx_data_field.preselector.default_oam_w_o_roo_with_rif_ipv4_presel_id_ipmf3_get(unit)
                || presel_id == dnx_data_field.preselector.default_oam_w_o_roo_with_rif_ipv6_presel_id_ipmf3_get(unit)
                || presel_id == dnx_data_field.preselector.default_j1_ipv4_mc_in_lif_presel_id_ipmf3_get(unit)
                || presel_id == dnx_data_field.preselector.default_j1_ipv6_mc_in_lif_presel_id_ipmf3_get(unit)
                || presel_id == dnx_data_field.preselector.default_rch_remove_presel_id_ipmf3_get(unit)
                || presel_id == dnx_data_field.preselector.default_j1_same_port_presel_id_epmf_get(unit)
                || presel_id == dnx_data_field.preselector.default_learn_limit_presel_id_epmf_get(unit)
                || presel_id + 1 == nof_presels)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_field_presel_entry_id_t_init(unit, &entry_id));
            SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, &entry_data));
            entry_id.presel_id = presel_id;
            entry_id.stage = stage;
            /*
             * We use the default context to get past verification.
             */
            entry_data.context_id = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);
            entry_data.entry_valid = FALSE;
            SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, 0, &entry_id, &entry_data));
        }

    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function destroys all non default context IDs.
 * \param [in] unit - Device ID
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ctest_dnx_field_util_context_clean(
    int unit)
{

    unsigned int context_id_ndx;
    int stage;
    uint8 is_alloc;
    dnx_field_context_t cascaded_from;
    unsigned int nof_contexts[NUM_STAGES_WITH_CONTEXT] = {
        [DNX_FIELD_STAGE_IPMF1] = 0,
        [DNX_FIELD_STAGE_IPMF2] = 0,
        [DNX_FIELD_STAGE_IPMF3] = 0,
        [DNX_FIELD_STAGE_EPMF] = 0
    };

    SHR_FUNC_INIT_VARS(unit);

    
    nof_contexts[DNX_FIELD_STAGE_IPMF1] =
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
    nof_contexts[DNX_FIELD_STAGE_IPMF2] =
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_contexts;
    nof_contexts[DNX_FIELD_STAGE_IPMF3] =
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF3)->nof_contexts;
    nof_contexts[DNX_FIELD_STAGE_EPMF] = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EPMF)->nof_contexts;

    /*
     * Clean-up the contexts on iPMF1 and iPMF2, first those on iPMF2 that aren't the default cascading for an
     * iPMF1 context, and then those on iPMF1 (which by then won't have any non default cascaded contexts on iPMF2).
     */
    for (context_id_ndx = 0; context_id_ndx < nof_contexts[DNX_FIELD_STAGE_IPMF2]; context_id_ndx++)
    {
        /*
         * Checking if the context ID is allocated to iPMF2 but not to iPMF1,
         * and it is not the default context.
         */
        if (context_id_ndx != DNX_FIELD_CONTEXT_ID_DEFAULT(unit))
        {
            SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, DNX_FIELD_STAGE_IPMF2, context_id_ndx, &is_alloc));
            if (is_alloc)
            {
                SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                                (unit, DNX_FIELD_STAGE_IPMF2, context_id_ndx, &cascaded_from));
                if (context_id_ndx != cascaded_from)
                {
                    SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, DNX_FIELD_STAGE_IPMF2, context_id_ndx));
                }
            }
        }
    }
    for (context_id_ndx = 0; context_id_ndx < nof_contexts[DNX_FIELD_STAGE_IPMF1]; context_id_ndx++)
    {
        /*
         * NOTE: remove the context_id_index != 1, once we store the dnx_data for OAM WAs.
         * Checking if the context ID is allocated to iPMF1 and it is not the default context or the ITMH_PPH context.
         */
        if (context_id_ndx != DNX_FIELD_CONTEXT_ID_DEFAULT(unit)
            && context_id_ndx != 1
            && context_id_ndx != dnx_data_field.context.default_itmh_pph_context_get(unit)
            && context_id_ndx != dnx_data_field.context.default_itmh_context_get(unit)
            && context_id_ndx != dnx_data_field.context.default_stacking_context_get(unit)
            && context_id_ndx != dnx_data_field.context.default_j1_itmh_context_get(unit)
            && context_id_ndx != dnx_data_field.context.default_j1_itmh_pph_context_get(unit)
            && context_id_ndx != dnx_data_field.context.default_oam_context_get(unit)
            && context_id_ndx != dnx_data_field.context.default_oam_upmep_context_get(unit)
            && context_id_ndx != dnx_data_field.context.default_oam_reflector_context_get(unit)
            && context_id_ndx != dnx_data_field.context.default_j1_learning_2ndpass_context_get(unit)
            && context_id_ndx != dnx_data_field.context.default_nat_context_get(unit)
            && context_id_ndx != dnx_data_field.context.default_rch_remove_context_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, DNX_FIELD_STAGE_IPMF1, context_id_ndx, &is_alloc));
            if (is_alloc)
            {
                SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, DNX_FIELD_STAGE_IPMF1, context_id_ndx));
            }
        }
    }
    /*
     * Clean up the contexts that aren't on iPMF1 and iPMF2.
     */
    DNX_FIELD_STAGE_QUAL_ITERATOR(stage)
    {
        
        if ((stage != DNX_FIELD_STAGE_IPMF1) && (stage != DNX_FIELD_STAGE_IPMF2) && (stage != DNX_FIELD_STAGE_EXTERNAL)
            && (stage != DNX_FIELD_STAGE_L4_OPS))
        {
            /*
             * NOTE: remove the context_id_index != 1, once we store the dnx_data for OAM WAs.
             * Find all allocated contexts within the stage except the default context and destroy them.
             */
            for (context_id_ndx = 0; context_id_ndx < nof_contexts[stage]; context_id_ndx++)
            {
                if (context_id_ndx != DNX_FIELD_CONTEXT_ID_DEFAULT(unit)
                    && context_id_ndx != 1
                    && context_id_ndx != dnx_data_field.context.default_itmh_pph_context_get(unit)
                    && context_id_ndx != dnx_data_field.context.default_itmh_context_get(unit)
                    && context_id_ndx != dnx_data_field.context.default_stacking_context_get(unit)
                    && context_id_ndx != dnx_data_field.context.default_j1_itmh_context_get(unit)
                    && context_id_ndx != dnx_data_field.context.default_j1_itmh_pph_context_get(unit)
                    && context_id_ndx != dnx_data_field.context.default_oam_context_get(unit)
                    && context_id_ndx != dnx_data_field.context.default_oam_upmep_context_get(unit)
                    && context_id_ndx != dnx_data_field.context.default_oam_reflector_context_get(unit)
                    && context_id_ndx != dnx_data_field.context.default_j1_learning_2ndpass_context_get(unit)
                    && context_id_ndx != dnx_data_field.context.default_nat_context_get(unit)
                    && context_id_ndx != dnx_data_field.context.default_rch_remove_context_get(unit))
                {
                    SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, stage, context_id_ndx, &is_alloc));
                    if (is_alloc)
                    {
                        SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, stage, context_id_ndx));
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function loops over the dnx_quals array.
 *  It check for qualifier of class USER and if it finds any,
 *  it sets the corresponding bit in the "qual_bitmap" bitmap to TRUE
 *  The corresponding bit is represented by qual_id.
 *
 * \param [in] unit                      - Device ID
 * \param [in] dnx_quals                 - DNX Qualifiers array
 * \param [out] qual_bitmap              - Bitmap array of user qualifiers
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ctest_dnx_field_util_extract_user_quals(
    int unit,
    dnx_field_qual_t * dnx_quals,
    uint8 qual_bitmap[])
{
    int qual_iter;
    dnx_field_qual_id_t qual_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_quals, _SHR_E_PARAM, "fg_basic_info");
    SHR_NULL_CHECK(qual_bitmap, _SHR_E_PARAM, "qual_bitmap");

    /** Loop over all the actions from the Context/FG info*/
    for (qual_iter = 0; qual_iter < CTEST_DNX_UTIL_MAX_NUM_QUALS_PER_FG; qual_iter++)
    {
        if (dnx_quals[qual_iter] == DNX_FIELD_QUAL_TYPE_INVALID)
        {
            break;
        }

       /** Check of the current action is user defined */
        if (DNX_QUAL_CLASS(dnx_quals[qual_iter]) == DNX_FIELD_QUAL_CLASS_USER)
        {
            qual_id = DNX_QUAL_ID(dnx_quals[qual_iter]);
            qual_bitmap[qual_id] = TRUE;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function loops over the dnx_actions array.
 *  It check for action of class USER and if it finds any,
 *  it sets the corresponding bit in the "action_bitmap" bitmap to TRUE
 *  The corresponding bit is represented by action_id.
 *
 * \param [in] unit                      - Device ID
 * \param [in] dnx_actions               - DNX Actions array
 * \param [out] action_bitmap            - Bitmap array of user actions
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ctest_dnx_field_util_extract_user_actions(
    int unit,
    dnx_field_action_t * dnx_actions,
    uint8 action_bitmap[])
{
    int action_iter;
    dnx_field_action_id_t action_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_actions, _SHR_E_PARAM, "fg_basic_info");
    SHR_NULL_CHECK(action_bitmap, _SHR_E_PARAM, "action_bitmap");

    /** Loop over all the actions from the FG info */
    for (action_iter = 0; action_iter < CTEST_DNX_UTIL_MAX_NUM_ACTIONS_PER_FG; action_iter++)
    {
        if (dnx_actions[action_iter] == DNX_FIELD_ACTION_INVALID)
        {
            break;
        }

       /** Check of the current action is user defined */
        if (DNX_ACTION_CLASS(dnx_actions[action_iter]) == DNX_FIELD_ACTION_CLASS_USER)
        {
            action_id = DNX_ACTION_ID(dnx_actions[action_iter]);
            action_bitmap[action_id] = TRUE;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function performs full clean-up of the field groups.
 *  It detaches all contexts from the field groups and then deletes them.
 *  NOTE:  For now user defined qualifiers and actions are only deleted through their
 *         field group association. Delete all user defined qualifiers and actions
 *         in separate functions once the functions to iterate over user defined qualifiers
 *         and actions are implemented.
 * \param [in] unit             - Device ID
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ctest_dnx_field_util_fg_clean(
    int unit)
{

    int context_id_iter = 0, action_bitmap_iter = 0, qual_bitmap_iter = 0, fg_id = 0;
    uint8 is_allocated = FALSE;
    dnx_field_group_full_info_t fg_info;
    bcm_field_action_t bcm_action;
    bcm_field_qualify_t bcm_qual;
    dnx_field_context_t *context_id_arr_p = NULL;
    uint8 *action_bitmap_p = NULL;
    uint8 *qual_bitmap_p = NULL;

    int attched_to_itmh_pph;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(context_id_arr_p, sizeof(*context_id_arr_p) * CTEST_DNX_UTIL_MAX_NUM_CONTEXT,
              "context_id_arr_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(action_bitmap_p, sizeof(*action_bitmap_p) * CTEST_DNX_UTIL_MAX_NUM_USER_ACTIONS,
              "action_bitmap_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(qual_bitmap_p, sizeof(*qual_bitmap_p) * CTEST_DNX_UTIL_MAX_NUM_USER_QUALS,
              "qual_bitmap_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    sal_memset(context_id_arr_p, DNX_FIELD_CONTEXT_ID_INVALID,
               sizeof(*context_id_arr_p) * CTEST_DNX_UTIL_MAX_NUM_CONTEXT);
    sal_memset(action_bitmap_p, FALSE, sizeof(*action_bitmap_p) * CTEST_DNX_UTIL_MAX_NUM_USER_ACTIONS);
    sal_memset(qual_bitmap_p, FALSE, sizeof(*qual_bitmap_p) * CTEST_DNX_UTIL_MAX_NUM_USER_QUALS);

    /** Loop over all field groups */
    for (fg_id = 0; fg_id < dnx_data_field.group.nof_fgs_get(unit); fg_id++)
    {
        /** Check if FG_ID is allocated */
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
        if (!is_allocated)
        {
            /** If FG_ ID is not allocated, move to next FG ID */
            continue;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id, &fg_info));
            /** Get the list of all contexts that the FG is attached to. */
            SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_id, context_id_arr_p));
            /*
             * Loop through the array of valid contexts for the current FG and performing detach
             * Storing all of the valid contexts in an array(*for_destroy) to be destroyed
             * after all FG's have been detached.
             */
            attched_to_itmh_pph = FALSE;
            for (context_id_iter = 0; context_id_iter < CTEST_DNX_UTIL_MAX_NUM_CONTEXT; context_id_iter++)
            {
                if (context_id_arr_p[context_id_iter] == DNX_FIELD_CONTEXT_ID_INVALID)
                {
                    break;
                }
                /** Do not detach from the ITHM_PPH context.*/
                if (context_id_arr_p[context_id_iter] == dnx_data_field.context.default_itmh_pph_context_get(unit)
                    || context_id_arr_p[context_id_iter] == dnx_data_field.context.default_itmh_context_get(unit)
                    || context_id_arr_p[context_id_iter] == dnx_data_field.context.default_stacking_context_get(unit)
                    || context_id_arr_p[context_id_iter] == dnx_data_field.context.default_j1_itmh_context_get(unit)
                    || context_id_arr_p[context_id_iter] == dnx_data_field.context.default_j1_itmh_pph_context_get(unit)
                    || context_id_arr_p[context_id_iter] == dnx_data_field.context.default_oam_context_get(unit)
                    || context_id_arr_p[context_id_iter] == dnx_data_field.context.default_oam_upmep_context_get(unit)
                    || context_id_arr_p[context_id_iter] ==
                    dnx_data_field.context.default_oam_reflector_context_get(unit)
                    || context_id_arr_p[context_id_iter] ==
                    dnx_data_field.context.default_j1_learning_2ndpass_context_get(unit))
                {
                    attched_to_itmh_pph = TRUE;
                    continue;
                }
                /** Detach context if ID is valid */
                SHR_IF_ERR_EXIT(dnx_field_group_context_detach(unit, fg_id, context_id_arr_p[context_id_iter]));
            }
            /** Do not delete the field group if it as attached to ITMH_PPH.*/
            if (attched_to_itmh_pph)
            {
                continue;
            }
           /** Extract user defined actions for deletion, fill the action_bitmap  */
            SHR_IF_ERR_EXIT(ctest_dnx_field_util_extract_user_actions
                            (unit, fg_info.group_basic_info.dnx_actions, action_bitmap_p));
           /** Extract user defined qualifiers for deletion, fill the bitmap qual_bitmap  */
            SHR_IF_ERR_EXIT(ctest_dnx_field_util_extract_user_quals
                            (unit, fg_info.group_basic_info.dnx_quals, qual_bitmap_p));
            /*
             * Delete all entries for this FG_ID * Currently only supports TCAM FG entry deletion
             */
            if (fg_info.group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
            {
                SHR_IF_ERR_EXIT(dnx_field_group_entry_delete_all(unit, fg_id));
            }
            /** Delete the FG and deallocate the FG_ID */
            SHR_IF_ERR_EXIT(dnx_field_group_delete(unit, fg_id));
        }
    }

    /** Loop over the local bitmap of actions and destroy them all */
    for (action_bitmap_iter = 0; action_bitmap_iter < CTEST_DNX_UTIL_MAX_NUM_USER_ACTIONS; action_bitmap_iter++)
    {
        if (action_bitmap_p[action_bitmap_iter] == TRUE)
        {
            bcm_action = action_bitmap_iter + dnx_data_field.action.user_1st_get(unit);
            SHR_IF_ERR_EXIT(dnx_field_action_destroy(unit, bcm_action));
        }
    }
    /** Loop over the local bitmap of qualifiers and destroy them all */
    for (qual_bitmap_iter = 0; qual_bitmap_iter < CTEST_DNX_UTIL_MAX_NUM_USER_QUALS; qual_bitmap_iter++)
    {
        if (qual_bitmap_p[qual_bitmap_iter] == TRUE)
        {
            bcm_qual = qual_bitmap_iter + dnx_data_field.qual.user_1st_get(unit);
            SHR_IF_ERR_EXIT(dnx_field_qual_destroy(unit, bcm_qual));
        }
    }

exit:
    SHR_FREE(context_id_arr_p);
    SHR_FREE(action_bitmap_p);
    SHR_FREE(qual_bitmap_p);
    SHR_FUNC_EXIT;
}

/**
 * See ctest_dnx_field_utils.h
 */
shr_error_e
ctest_dnx_field_util_clean(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Delete all field groups
     */
    SHR_IF_ERR_EXIT(ctest_dnx_field_util_fg_clean(unit));

    /*
     * Delete all preselectors
     */
    SHR_IF_ERR_EXIT(ctest_dnx_field_util_presel_clean(unit));

    /*
     * Delete all contexts
     */
    SHR_IF_ERR_EXIT(ctest_dnx_field_util_context_clean(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See ctest_dnx_field_utils.h
 */
shr_error_e
ctest_dnx_field_util_basic_tcam_fg_t_init(
    int unit,
    ctest_dnx_field_util_basic_tcam_fg_t * tcam_fg_util_p)
{
    int ii;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tcam_fg_util_p, _SHR_E_PARAM, "tcam_fg_util_p");

    tcam_fg_util_p->context = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);
    tcam_fg_util_p->field_stage = DNX_FIELD_STAGE_IPMF1;

    for (ii = 0; ii < CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS; ii++)
    {
        /*
         *  We are initializing the array to be filled with DP qualifier and value 0x1
         *  the rest of the qualifiers are bcmFieldQualifyCount
         */
        tcam_fg_util_p->bcm_quals[ii] = ctest_bcm_qualifiers_for_init[ii];
        tcam_fg_util_p->qual_values[ii].value[0] = ctest_bcm_qualifier_values_for_init[ii];

        /*
         * We are initializing the array to be filled with action DP , value 0x1,
         *  the rest of the actions are bcmFieldActionCount
         */
        tcam_fg_util_p->bcm_actions[ii] = ctest_bcm_actions_for_init[ii];
        tcam_fg_util_p->action_values[ii].value[0] = ctest_bcm_action_values_for_init[ii];

        /*
         * Initializing the qualifier info to valid values.
         */
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &tcam_fg_util_p->qual_info[ii]));

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates a TCAM field group to the relevant stage using the provided
 *  qualifiers and actions in the "ctest_dnx_field_util_basic_tcam_fg_t" structure.
 *  The provided BCM quals and actions are translated into DNX encoded and then set.
 *  It returns the Field Group ID and FG info.
 *
 * \param [in]  unit                      - Device ID
 * \param [in]  tcam_fg_util_p            - Input information from user(stage, bcm_quals, bcm_actions)
 * \param [out] fg_info_p                 - Field group info structure with set parameters
 * \param [out] fg_id_p                   - Field Group ID returned from DNX API.
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ctest_dnx_field_util_basic_tcam_fg_add(
    int unit,
    ctest_dnx_field_util_basic_tcam_fg_t * tcam_fg_util_p,
    dnx_field_group_info_t * fg_info_p,
    dnx_field_group_t * fg_id_p)
{
    int qual_action_iter;
    dnx_field_action_t dnx_action;
    dnx_field_qual_t dnx_qual;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, fg_info_p));
    qual_count = 0;
    action_count = 0;

    fg_info_p->field_stage = tcam_fg_util_p->field_stage;
    fg_info_p->fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    /** Loop over the supplied arrays of BCM qualifiers and actions, which we limit to 3 */
    for (qual_action_iter = 0; qual_action_iter < CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS; qual_action_iter++)
    {
        /** We treat bcmFieldQualifyCount as an Invalid value */
        if (tcam_fg_util_p->bcm_quals[qual_action_iter] != bcmFieldQualifyCount)
        {
            /** Translates the current BCM qualifier to DNX encoded and set it to the FG Info structure */
            SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx
                            (unit, tcam_fg_util_p->field_stage, tcam_fg_util_p->bcm_quals[qual_action_iter],
                             &dnx_qual));
            fg_info_p->dnx_quals[qual_action_iter] = dnx_qual;
            qual_count++;
        }

        /** We treat bcmFieldActionCount as an Invalid value */
        if (tcam_fg_util_p->bcm_actions[qual_action_iter] != bcmFieldActionCount)
        {
            /** Translates the current BCM action to DNX encoded and set it to the FG Info structure */
            SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx
                            (unit, tcam_fg_util_p->field_stage, tcam_fg_util_p->bcm_actions[qual_action_iter],
                             &dnx_action));
            fg_info_p->dnx_actions[qual_action_iter] = dnx_action;
            action_count++;
        }
    }

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing group add : %d \r\n",
                __func__, __LINE__, dnx_field_stage_text(unit, tcam_fg_util_p->field_stage), *fg_id_p);

    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, 0, fg_info_p, fg_id_p));

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Field group: %d  added \r\n",
                __func__, __LINE__, dnx_field_stage_text(unit, tcam_fg_util_p->field_stage), *fg_id_p);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function initializes the attach information for the context that is provided in
 *  "tcam_fg_util_p". The attach info structure(attach_info_p) is filled with all the relevant
 *  information.
 *  The context provided is created if it is not DNX_FIELD_CONTEXT_ID_DEFAULT(unit), before the field group
 *  passed is attached to it, with the filled-in attach info.
 *
 * \param [in] unit                      - Device ID
 * \param [in] tcam_fg_util_p            - Input information from user(stage and context)
 * \param [in] fg_info_p                 - Field group info structure with set parameters.
 * \param [in] fg_id                     - Field Group ID to attach.
 * \param [out] attach_info_p            - Attach info to be filled and attached.
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ctest_dnx_field_util_basic_context_attach(
    int unit,
    ctest_dnx_field_util_basic_tcam_fg_t * tcam_fg_util_p,
    dnx_field_group_info_t * fg_info_p,
    dnx_field_group_t fg_id,
    dnx_field_group_attach_info_t * attach_info_p)
{

    int action_iter, qual_iter;
    uint8 is_allocated;
    dnx_field_context_mode_t context_mode;
    uint32 flags = 0;
    dnx_field_stage_e stage_for_context_create;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, attach_info_p));
    /*
     * Setting the attach information of the context.
     */
    sal_memcpy(attach_info_p->dnx_quals, fg_info_p->dnx_quals, sizeof(attach_info_p->dnx_quals));
    sal_memcpy(attach_info_p->dnx_actions, fg_info_p->dnx_actions, sizeof(attach_info_p->dnx_actions));

    /** Loop over the supplied arrays of BCM qualifiers and actions, which we limit to 3 */
    for (action_iter = 0; action_iter < action_count; action_iter++)
    {
        /*
         * No action attach info needs to be set at this point (we use default priority).
         */
    }
    for (qual_iter = 0; qual_iter < qual_count; qual_iter++)
    {
        if (tcam_fg_util_p->qual_info[qual_iter].input_type != DNX_FIELD_INPUT_TYPE_INVALID)
        {
            attach_info_p->qual_info[qual_iter].input_type = tcam_fg_util_p->qual_info[qual_iter].input_type;
            attach_info_p->qual_info[qual_iter].input_arg = tcam_fg_util_p->qual_info[qual_iter].input_arg;
            attach_info_p->qual_info[qual_iter].offset = tcam_fg_util_p->qual_info[qual_iter].offset;
        }
        else
        {
            if (DNX_QUAL_CLASS(attach_info_p->dnx_quals[qual_iter]) == DNX_FIELD_QUAL_CLASS_HEADER
                || DNX_QUAL_CLASS(attach_info_p->dnx_quals[qual_iter]) == DNX_FIELD_QUAL_CLASS_LAYER_RECORD)
            {
                /** We set header qualifiers to use ABSOLUTE layer base */
                attach_info_p->qual_info[qual_iter].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
                attach_info_p->qual_info[qual_iter].input_arg = 0;
                attach_info_p->qual_info[qual_iter].offset = 0;
            }
            else if (DNX_QUAL_CLASS(attach_info_p->dnx_quals[qual_iter]) == DNX_FIELD_QUAL_CLASS_META)
            {
                attach_info_p->qual_info[qual_iter].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
            }
            else if (DNX_QUAL_CLASS(attach_info_p->dnx_quals[qual_iter]) == DNX_FIELD_QUAL_CLASS_META2)
            {
                attach_info_p->qual_info[qual_iter].input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "This function does not support the given qualifier %s, at index %d, class: %s \n",
                             dnx_field_dnx_qual_text(unit, attach_info_p->dnx_quals[qual_iter]), qual_iter,
                             dnx_field_qual_class_text(DNX_QUAL_CLASS(attach_info_p->dnx_quals[qual_iter])));
            }
        }
    }
    /** Default context is automatically created on init, for every stage */
    if (tcam_fg_util_p->context != DNX_FIELD_CONTEXT_ID_DEFAULT(unit))
    {
        /** We check if the context_id is already allocated(created), if yes, we don't want to create again */
        SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated
                        (unit, tcam_fg_util_p->field_stage, tcam_fg_util_p->context, &is_allocated));
        if (!is_allocated)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing context create for context: %d \r\n",
                        __func__, __LINE__, dnx_field_stage_text(unit, tcam_fg_util_p->field_stage),
                        tcam_fg_util_p->context);

            SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
            if (tcam_fg_util_p->field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                stage_for_context_create = DNX_FIELD_STAGE_IPMF1;
            }
            else
            {
                stage_for_context_create = tcam_fg_util_p->field_stage;
            }
            SHR_IF_ERR_EXIT(dnx_field_context_create
                            (unit, DNX_FIELD_CONTEXT_FLAG_WITH_ID, stage_for_context_create, &context_mode,
                             &tcam_fg_util_p->context));
        }

    }
    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing context attach on context: %d \r\n",
                __func__, __LINE__, dnx_field_stage_text(unit, tcam_fg_util_p->field_stage), tcam_fg_util_p->context);

    SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, flags, fg_id, tcam_fg_util_p->context, attach_info_p));

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, .Field group : %d has been attached to context: %d \r\n",
                __func__, __LINE__, fg_id, tcam_fg_util_p->context);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function adds an entry to the TCAM.
 *  Currently it adds all qualifiers and actions set in the attach info and
 *  gets the values from the supplied tcam_fg_util_p structure(filled by the user).
 *  It returns a filled in entry_info_p structure
 *
 *
 * \param [in] unit                      - Device ID
 * \param [in] tcam_fg_util_p            - Input information from user(stage and context)
 * \param [in] attach_info_p             - Attach info to be filled and attached.
 * \param [in] fg_id                     - Field Group ID to attach.
 * \param [out] entry_info_p             - Entry information to be set in TCAM
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ctest_dnx_field_util_basic_entry_add(
    int unit,
    ctest_dnx_field_util_basic_tcam_fg_t * tcam_fg_util_p,
    dnx_field_group_attach_info_t * attach_info_p,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p)
{
    int action_iter, qual_iter, ii;
    uint32 entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, entry_info_p));
   /** Setting mid-range priority */
    entry_info_p->priority = 5;

   /** Loop over the supplied arrays of BCM actions, which we limit to 3 */
    for (action_iter = 0; action_iter < action_count; action_iter++)
    {
       /** Setting action info and value to the entry */
        entry_info_p->payload_info.action_info[action_iter].dnx_action = attach_info_p->dnx_actions[action_iter];
        entry_info_p->payload_info.action_info[action_iter].action_value[0] =
            tcam_fg_util_p->action_values[action_iter].value[0];
    }

   /** Loop over the supplied array of BCM qualifiers , which we limit to 3 */
    for (qual_iter = 0; qual_iter < qual_count; qual_iter++)
    {
       /** Setting qualifier info and value to the entry */
        entry_info_p->key_info.qual_info[qual_iter].dnx_qual = attach_info_p->dnx_quals[qual_iter];
        for (ii = 0; ii < DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY; ii++)
        {
            entry_info_p->key_info.qual_info[qual_iter].qual_value[ii] =
                tcam_fg_util_p->qual_values[qual_iter].value[ii];
            entry_info_p->key_info.qual_info[qual_iter].qual_mask[ii] = 0xFFFFFFFF;
        }

    }

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing entry add to field group: %d \r\n",
                __func__, __LINE__, dnx_field_stage_text(unit, tcam_fg_util_p->field_stage), fg_id);

    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, fg_id, entry_info_p, &entry_handle));

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s . Entry added to field group: %d \r\n",
                __func__, __LINE__, dnx_field_stage_text(unit, tcam_fg_util_p->field_stage), fg_id);
exit:
    SHR_FUNC_EXIT;
}

/**
 * See ctest_dnx_field_utils.h
 */
shr_error_e
ctest_dnx_field_util_basic_tcam_fg_full(
    int unit,
    ctest_dnx_field_util_basic_tcam_fg_t * tcam_fg_util_p,
    dnx_field_group_t * fg_id_p)
{
    dnx_field_group_info_t fg_info;
    dnx_field_group_attach_info_t attach_info;
    dnx_field_entry_t entry_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Creating a field group with the info supplied by the user inside tcam_fg_util_p.
     */
    SHR_IF_ERR_EXIT(ctest_dnx_field_util_basic_tcam_fg_add(unit, tcam_fg_util_p, &fg_info, fg_id_p));
    /*
     * Creating(if context supplied is not DNX_FIELD_CONTEXT_ID_DEFAULT(unit)) context and
     * attaching the supplied FG to it.
     */
    SHR_IF_ERR_EXIT(ctest_dnx_field_util_basic_context_attach(unit, tcam_fg_util_p, &fg_info, *fg_id_p, &attach_info));
    /*
     * Adding single entry containing all qualifiers and actions(for now), to the previously created FG.
     */
    SHR_IF_ERR_EXIT(ctest_dnx_field_util_basic_entry_add(unit, tcam_fg_util_p, &attach_info, *fg_id_p, &entry_info));

exit:
    SHR_FUNC_EXIT;

}

/**
 * See ctest_dnx_field_utils.h
 */
shr_error_e
ctest_bcm_field_util_basic_tcam_fg_t_init(
    int unit,
    ctest_bcm_field_util_basic_tcam_fg_t * tcam_fg_util_p)
{
    int ii;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tcam_fg_util_p, _SHR_E_PARAM, "tcam_fg_util_p");

    tcam_fg_util_p->fg_id = -1;
    tcam_fg_util_p->context = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);
    tcam_fg_util_p->stage = bcmFieldStageIngressPMF1;

    for (ii = 0; ii < CTEST_BCM_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS; ii++)
    {
        /*
         *  We are initializing the array to be filled with DP qualifier and value 0x1
         *  the rest of the qualifiers are bcmFieldQualifyCount
         */
        tcam_fg_util_p->bcm_qual[ii] = ctest_bcm_qualifiers_for_init[ii];
        tcam_fg_util_p->qual_values[ii].value[0] = ctest_bcm_qualifier_values_for_init[ii];

        /*
         * We are initializing the array to be filled with action DP , value 0x1,
         *  the rest of the actions are bcmFieldActionCount
         */
        tcam_fg_util_p->bcm_actions[ii] = ctest_bcm_actions_for_init[ii];
        tcam_fg_util_p->action_values[ii].value[0] = ctest_bcm_action_values_for_init[ii];

        /*
         * Initializing the qualifier info to valid values.
         */
        tcam_fg_util_p->qual_info[ii].input_type = BCM_FIELD_INVALID;
        tcam_fg_util_p->qual_info[ii].input_arg = BCM_FIELD_INVALID;
        tcam_fg_util_p->qual_info[ii].offset = BCM_FIELD_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates a TCAM field group to the relevant stage using the provided
 *  qualifiers and actions in the "ctest_dnx_field_util_basic_tcam_fg_t" structure.
 *  The provided BCM quals and actions are translated into DNX encoded and then set.
 *  It returns the Field Group ID and FG info.
 *
 * \param [in]  unit                      - Device ID
 * \param [in/out]  tcam_fg_util_p        - Input information from user(stage, bcm_quals, bcm_actions).
 *                                          Output is 'fg_id' if not explicitly set.
 * \param [out] fg_info_p                 - Field group info structure with set parameters
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ctest_bcm_field_util_basic_tcam_fg_add(
    int unit,
    ctest_bcm_field_util_basic_tcam_fg_t * tcam_fg_util_p,
    bcm_field_group_info_t * fg_info_p)
{
    int qual_action_iter;
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_info_t_init(fg_info_p);

    fg_info_p->stage = tcam_fg_util_p->stage;
    fg_info_p->fg_type = bcmFieldGroupTypeTcam;
    /** Loop over the supplied arrays of BCM qualifiers and actions, which we limit to 3 */
    for (qual_action_iter = 0; qual_action_iter < CTEST_BCM_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS; qual_action_iter++)
    {
        /** We treat bcmFieldQualifyCount as an Invalid value */
        if (tcam_fg_util_p->bcm_qual[qual_action_iter] != bcmFieldQualifyCount)
        {
            fg_info_p->qual_types[qual_action_iter] = tcam_fg_util_p->bcm_qual[qual_action_iter];
            fg_info_p->nof_quals++;
        }

        /** We treat bcmFieldActionCount as an Invalid value */
        if (tcam_fg_util_p->bcm_actions[qual_action_iter] != bcmFieldActionCount)
        {
            fg_info_p->action_types[qual_action_iter] = tcam_fg_util_p->bcm_actions[qual_action_iter];
            fg_info_p->nof_actions++;
        }
    }
    if (tcam_fg_util_p->fg_id == -1)
    {
        flags = BCM_FIELD_FLAG_MSB_RESULT_ALIGN;
    }
    else
    {
        flags = BCM_FIELD_FLAG_WITH_ID | BCM_FIELD_FLAG_MSB_RESULT_ALIGN;
    }

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing group add : %d \r\n",
                __func__, __LINE__, dnx_field_bcm_stage_text(tcam_fg_util_p->stage), tcam_fg_util_p->fg_id);

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, flags, fg_info_p, &tcam_fg_util_p->fg_id));

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Field group: %d  added \r\n",
                __func__, __LINE__, dnx_field_stage_text(unit, tcam_fg_util_p->stage), tcam_fg_util_p->fg_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function initializes the attach information for the context that is provided in
 *  "tcam_fg_util_p". The attach info structure(attach_info_p) is filled with all the relevant
 *  information.
 *  The context provided is created if it is not DNX_FIELD_CONTEXT_ID_DEFAULT(unit), before the field group
 *  passed is attached to it, with the filled-in attach info.
 *
 * \param [in] unit                      - Device ID
 * \param [in] fg_info_p                 - Field group info structure with set parameters.
 * \param [in/out] tcam_fg_util_p            - Input information from user(stage and context)
 * \param [out] attach_info_p            - Attach info to be filled and attached.
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ctest_bcm_field_util_basic_context_attach(
    int unit,
    ctest_bcm_field_util_basic_tcam_fg_t * tcam_fg_util_p,
    bcm_field_group_info_t * fg_info_p,
    bcm_field_group_attach_info_t * attach_info_p)
{

    int action_iter, qual_iter, rv;
    dnx_field_stage_e dnx_stage;
    dnx_field_qual_t dnx_qual;
    bcm_field_context_info_t context_info;
    bcm_field_stage_t stage_for_context_create;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_attach_info_t_init(attach_info_p);
    /*
     * Setting the attach information of the context.
     */
    sal_memcpy(attach_info_p->key_info.qual_types, fg_info_p->qual_types, sizeof(attach_info_p->key_info.qual_types));
    sal_memcpy(attach_info_p->payload_info.action_types, fg_info_p->action_types,
               sizeof(attach_info_p->payload_info.action_types));
    attach_info_p->payload_info.nof_actions = fg_info_p->nof_actions;
    attach_info_p->key_info.nof_quals = fg_info_p->nof_quals;

    /** Loop over the supplied arrays of BCM qualifiers and actions, which we limit to 3 */
    for (action_iter = 0; action_iter < attach_info_p->payload_info.nof_actions; action_iter++)
    {
        /** Setting different priority values - with FES IDs 2, 7, 12(We want to avoid corner cases for now) */
        attach_info_p->payload_info.action_info[action_iter].priority =
            BCM_FIELD_ACTION_PRIORITY(0, ((5 * action_iter) + 2));
    }
    for (qual_iter = 0; qual_iter < attach_info_p->key_info.nof_quals; qual_iter++)
    {
        if (tcam_fg_util_p->qual_info[qual_iter].input_type != BCM_FIELD_INVALID)
        {
            attach_info_p->key_info.qual_info[qual_iter].input_type = tcam_fg_util_p->qual_info[qual_iter].input_type;
            attach_info_p->key_info.qual_info[qual_iter].input_arg = tcam_fg_util_p->qual_info[qual_iter].input_arg;
            attach_info_p->key_info.qual_info[qual_iter].offset = tcam_fg_util_p->qual_info[qual_iter].offset;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, fg_info_p->stage, &dnx_stage));
            SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx
                            (unit, dnx_stage, attach_info_p->key_info.qual_types[qual_iter], &dnx_qual));

            if (DNX_QUAL_CLASS(dnx_qual) == DNX_FIELD_QUAL_CLASS_HEADER
                || DNX_QUAL_CLASS(dnx_qual) == DNX_FIELD_QUAL_CLASS_LAYER_RECORD)
            {
                /** We set header qualifiers to use ABSOLUTE layer base */
                attach_info_p->key_info.qual_info[qual_iter].input_type = bcmFieldInputTypeLayerAbsolute;
                attach_info_p->key_info.qual_info[qual_iter].input_arg = 0;
                attach_info_p->key_info.qual_info[qual_iter].offset = 0;
            }
            else if (DNX_QUAL_CLASS(dnx_qual) == DNX_FIELD_QUAL_CLASS_META)
            {
                attach_info_p->key_info.qual_info[qual_iter].input_type = bcmFieldInputTypeMetaData;
                attach_info_p->key_info.qual_info[qual_iter].input_arg = BCM_FIELD_INVALID;
                attach_info_p->key_info.qual_info[qual_iter].offset = BCM_FIELD_INVALID;
            }
            else if (DNX_QUAL_CLASS(dnx_qual) == DNX_FIELD_QUAL_CLASS_META2)
            {
                attach_info_p->key_info.qual_info[qual_iter].input_type = bcmFieldInputTypeMetaData;
                attach_info_p->key_info.qual_info[qual_iter].input_arg = BCM_FIELD_INVALID;
                attach_info_p->key_info.qual_info[qual_iter].offset = BCM_FIELD_INVALID;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "This function does not support the given qualifier %s, at index %d, class: %s \n",
                             dnx_field_dnx_qual_text(unit, attach_info_p->key_info.qual_types[qual_iter]), qual_iter,
                             dnx_field_qual_class_text(DNX_QUAL_CLASS(dnx_qual)));
            }
        }
    }
    /** Default context is automatically created on init, for every stage */
    if (tcam_fg_util_p->context != DNX_FIELD_CONTEXT_ID_DEFAULT(unit))
    {
        /** We check if the context_id is already allocated(created), if yes, we don't want to create again */
        rv = bcm_field_context_info_get(unit, fg_info_p->stage, tcam_fg_util_p->context, &context_info);
        if (rv == _SHR_E_NOT_FOUND)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing context create for context: %d \r\n",
                        __func__, __LINE__, dnx_field_bcm_stage_text(tcam_fg_util_p->stage), tcam_fg_util_p->context);

            bcm_field_context_info_t_init(&context_info);
            if (tcam_fg_util_p->stage == bcmFieldStageIngressPMF2)
            {
                stage_for_context_create = bcmFieldStageIngressPMF1;
            }
            else
            {
                stage_for_context_create = tcam_fg_util_p->stage;
            }
            SHR_IF_ERR_EXIT(bcm_field_context_create
                            (unit, BCM_FIELD_FLAG_WITH_ID, stage_for_context_create, &context_info,
                             &tcam_fg_util_p->context));
        }

    }
    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing context attach on context: %d \r\n",
                __func__, __LINE__, dnx_field_bcm_stage_text(tcam_fg_util_p->stage), tcam_fg_util_p->context);

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach
                    (unit, 0, tcam_fg_util_p->fg_id, tcam_fg_util_p->context, attach_info_p));

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, .Field group : %d has been attached to context: %d \r\n",
                __func__, __LINE__, tcam_fg_util_p->fg_id, tcam_fg_util_p->context);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function adds an entry to the TCAM.
 *  Currently it adds all qualifiers and actions set in the attach info and
 *  gets the values from the supplied tcam_fg_util_p structure(filled by the user).
 *  It returns a filled in entry_info_p structure
 *
 *
 * \param [in] unit                      - Device ID
 * \param [in] tcam_fg_util_p            - Input information from user(stage, context and fg_id)
 * \param [in] attach_info_p             - Attach info to be filled and attached.
 * \param [out] entry_info_p             - Entry information to be set in TCAM
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ctest_bcm_field_util_basic_entry_add(
    int unit,
    ctest_bcm_field_util_basic_tcam_fg_t * tcam_fg_util_p,
    bcm_field_group_attach_info_t * attach_info_p,
    bcm_field_entry_info_t * entry_info_p)
{
    int action_iter, qual_iter, ii;
    bcm_field_entry_t entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_entry_info_t_init(entry_info_p);
   /** Setting mid-range priority */
    entry_info_p->priority = 5;
    entry_info_p->nof_entry_actions = attach_info_p->payload_info.nof_actions;
    entry_info_p->nof_entry_quals = attach_info_p->key_info.nof_quals;

   /** Loop over the supplied arrays of BCM actions, which we limit to 3 */
    for (action_iter = 0; action_iter < attach_info_p->payload_info.nof_actions; action_iter++)
    {
       /** Setting action info and value to the entry */
        entry_info_p->entry_action[action_iter].type = attach_info_p->payload_info.action_types[action_iter];
        for (ii = 0; ii < DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY; ii++)
        {
            entry_info_p->entry_action[action_iter].value[ii] = tcam_fg_util_p->action_values[action_iter].value[ii];
        }
    }

   /** Loop over the supplied array of BCM qualifiers , which we limit to 3 */
    for (qual_iter = 0; qual_iter < attach_info_p->key_info.nof_quals; qual_iter++)
    {
       /** Setting qualifier info and value to the entry */
        entry_info_p->entry_qual[action_iter].type = attach_info_p->key_info.qual_types[qual_iter];
        for (ii = 0; ii < DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY; ii++)
        {
            entry_info_p->entry_qual[action_iter].value[ii] = tcam_fg_util_p->qual_values[qual_iter].value[ii];
            entry_info_p->entry_qual[action_iter].mask[ii] = 0xFFFFFFFF;
        }

    }

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing entry add to field group: %d \r\n",
                __func__, __LINE__, dnx_field_bcm_stage_text(tcam_fg_util_p->stage), tcam_fg_util_p->fg_id);

    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, tcam_fg_util_p->fg_id, entry_info_p, &entry_handle));

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s . Entry added to field group: %d \r\n",
                __func__, __LINE__, dnx_field_stage_text(unit, tcam_fg_util_p->stage), tcam_fg_util_p->fg_id);
exit:
    SHR_FUNC_EXIT;
}

/**
 * See ctest_dnx_field_utils.h
 */
shr_error_e
ctest_bcm_field_util_basic_tcam_fg_full(
    int unit,
    ctest_bcm_field_util_basic_tcam_fg_t * tcam_fg_util_p)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Creating a field group with the info supplied by the user inside tcam_fg_util_p.
     */
    SHR_IF_ERR_EXIT(ctest_bcm_field_util_basic_tcam_fg_add(unit, tcam_fg_util_p, &fg_info));
    /*
     * Creating(if context supplied is not DNX_FIELD_CONTEXT_ID_DEFAULT(unit)) context and
     * attaching the supplied FG to it.
     */
    SHR_IF_ERR_EXIT(ctest_bcm_field_util_basic_context_attach(unit, tcam_fg_util_p, &fg_info, &attach_info));
    /*
     * Adding single entry containing all qualifiers and actions(for now), to the previously created FG.
     */
    SHR_IF_ERR_EXIT(ctest_bcm_field_util_basic_entry_add(unit, tcam_fg_util_p, &attach_info, &entry_info));

exit:
    SHR_FUNC_EXIT;

}

/**
 * See ctest_dnx_field_utils.h
 */
shr_error_e
ctest_dnx_field_utils_packet_tx(
    int unit,
    bcm_port_t src_port,
    dnx_field_utils_packet_info_t * packet_p)
{
    rhhandle_t packet_handle;
    int protocol;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(packet_p, _SHR_E_PARAM, "packet_p");

    sal_memset(&packet_handle, 0, sizeof(packet_handle));

    /*
     * Create packet handler, which will contain packet info, like protocols and relevant fields to them.
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_handle));

    /*
     * Construct the packet, which will be transmitted
     */
    for (protocol = 0;
         (protocol < DNX_FIELD_UTILS_NOF_PROTOCOLS_IN_PACKET) && (sal_strcmp(packet_p->header_info[protocol], ""));
         protocol++)
    {
        if ((sal_strlen(packet_p->header_info[protocol])) > DNX_FIELD_UTILS_STR_SIZE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The given string (%s) is bigger than max allowed (%d) !!!",
                         packet_p->header_info[protocol], DNX_FIELD_UTILS_STR_SIZE);
        }

        /*
         * Enter here if the given element contains no ".".
         * In that case, assume it represents a protocol.
         */
        if (sal_strstr(packet_p->header_info[protocol], ".") == NULL)
        {
            /*
             * Add protocol to the packet
             */
            SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_handle, packet_p->header_info[protocol]));
        }
        else
        {
            if (!(sal_strcmp(packet_p->header_info[protocol + 1], "")))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Value for field (%s) is not exposed!!!", packet_p->header_info[protocol]);
            }
            /*
             * Fill the Protocol Fields, if any.
             * The expected format of a field is <protocol>.<field>.
             * For example:
             *      ETH.DA, ETH.SA, ETH.Type
             *      VLAN.VID, VLAN.PCP, VLAN.DEI
             *      IPv4.SIP, IPv4.DIP, IPv4.Version
             */
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_handle, packet_p->header_info[protocol],
                                                           packet_p->header_info[protocol + 1]));
            protocol++;
        }
    }

    /*
     * Sending the constructed packet
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, src_port, packet_handle, SAND_PACKET_RESUME));

exit:
    diag_sand_packet_free(unit, packet_handle);
    SHR_FUNC_EXIT;
}

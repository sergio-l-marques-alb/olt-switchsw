/** \file field_presel.c
 * $Id$
 *
 * Field preselector procedures for DNX.
 *
 * Management for contex selection i.e program selection table and line
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <shared/bsl.h>
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <shared/error.h>
#include <bcm/field.h>
#include <bcm/error.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_port.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>

/*
 * }Include files
 */

 /*
  * Defines
  * {
  */

/** 
 * The value to be written in the CTX_ID_VALID field to make the presel entry change the context ID to
 * what is written in CTX_ID field.
 */
#define DNX_FIELD_PRESEL_CTX_ID_VALID_SET (1)

 /*
  * }
  */

/**
* \brief
*  Clear the dnx_field_presel_qual_data_t, set it to preferred init values
* \param [in] unit          - Device ID
* \param [in] qual_data_p   - Pointer to input structure of
*                             dnx_field_presel_qual_data_t that needs to be init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_field_presel_qual_data_t_init(
    int unit,
    dnx_field_presel_qual_data_t * qual_data_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_data_p, _SHR_E_PARAM, "qual_data_p");

    qual_data_p->qual_type_dbal_field = DBAL_NOF_FIELDS;
    qual_data_p->qual_value = 0;
    qual_data_p->qual_mask = 0;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See field_presel.h
 */
shr_error_e
dnx_field_presel_entry_id_t_init(
    int unit,
    dnx_field_presel_entry_id_t * entry_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_id_p, _SHR_E_PARAM, "entry_id_p");
    entry_id_p->presel_id = DNX_FIELD_PRESEL_ID_INVALID;
    entry_id_p->stage = DNX_FIELD_STAGE_NOF;
exit:
    SHR_FUNC_EXIT;
}
/**
 * See field_presel.h
 */
shr_error_e
dnx_field_presel_entry_data_t_init(
    int unit,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    int qual_idx;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_data_p, _SHR_E_PARAM, "entry_data_p");

    entry_data_p->entry_valid = FALSE;
    entry_data_p->nof_qualifiers = 0;
    entry_data_p->context_id = DNX_FIELD_CONTEXT_ID_INVALID;

    for (qual_idx = 0; qual_idx < DNX_FIELD_CS_QUAL_NOF_MAX; qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_presel_qual_data_t_init(unit, &(entry_data_p->qual_data[qual_idx])));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief Write the Context Selection key to the hardware
* \param [in] unit       - Device ID
* \param [in] flags      - Preselection flags
* \param [in] entry_id_p   - Identifier of the preselector (User-provided presel_id and stage)
* \param [in] entry_data_p - Preselection entry data that will be writted into HW (PRESEL-QUALS, PROGRAM_ID, ENTRY_VALID)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_presel_hw_set(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * entry_id_p,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;
    int qual_idx;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_id_p, _SHR_E_PARAM, "entry_id_p");
    SHR_NULL_CHECK(entry_data_p, _SHR_E_PARAM, "entry_data_p");

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Setting Context Selection data to preselector ID %d...\n"), entry_id_p->presel_id));

    /*
     * Get a proper DBAL table according to the given stage 
     */
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_table_id(unit, entry_id_p->stage, &dbal_table_id));
    /*
     * Allocate DBAL handle to the returned dbal_table_id
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /*
     * Setting the entry access ID this indicates the line of the entry in the HW,
     * in our case it is corresponding to entry_id_p->presel_id.
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_id_p->presel_id));

    /*
     * entry_data_p->entry_valid determines if we create/update an entry or delete it.
     */
    if (entry_data_p->entry_valid)
    {
        /*
         * Setting key fields
         */
        for (qual_idx = 0; qual_idx < entry_data_p->nof_qualifiers; qual_idx++)
        {
            dbal_entry_key_field_arr32_masked_set(unit,
                                                  entry_handle_id,
                                                  entry_data_p->qual_data[qual_idx].qual_type_dbal_field,
                                                  &(entry_data_p->qual_data[qual_idx].qual_value),
                                                  &(entry_data_p->qual_data[qual_idx].qual_mask));
        }

        /*
         * Setting value fields
         */
        if (entry_id_p->stage != DNX_FIELD_STAGE_EPMF)
        {
            dbal_entry_value_field32_set
                (unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID_VALID, INST_SINGLE,
                 DNX_FIELD_PRESEL_CTX_ID_VALID_SET);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, INST_SINGLE,
                                     entry_data_p->context_id);
        /*
         * Write the presel. 
         * Note we use DBAL_COMMIT_FORCE instead of DBAL_COMMIT because we can also overrun an existing presel.
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit
                        (unit, entry_handle_id, DBAL_COMMIT_FORCE | DBAL_COMMIT_OVERRIDE_DEFAULT));
    }
    else
    {
        /*
         * Delete the entry, but first check if it exists.
         */
        rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(rv);
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief Read the Context Selection key from the hardware and
*        take the data for the used presel qualifiers only
* \param [in] unit        - Device ID
* \param [in] flags       - Preselection flags
* \param [in] entry_id_p  - Identifier of the preselector
* \param [out] entry_data_p - Preselection entry data for used presel qualifiers
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_presel_hw_get(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * entry_id_p,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_name;
    /*
     * Used to store the whole qual_data, which is collected from the HW.
     */
    dnx_field_presel_entry_data_t entry_data;
    int qual_idx;
    /*
     * Pointer to the array of dnx cs quals sets
     */
    const dbal_fields_e *dnx_cs_qual_info;
    /*
     * Number of presel qualifiers per stage
     */
    int dnx_cs_qual_nof;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_id_p, _SHR_E_PARAM, "entry_id_p");
    SHR_NULL_CHECK(entry_data_p, _SHR_E_PARAM, "entry_data_p");

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Getting Context Selection data for preselector ID %d...\n"), entry_id_p->presel_id));
    /*
     * Get a proper DBAL table according to the given stage 
     */
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_table_id(unit, entry_id_p->stage, &dbal_table_name));
    /*
     * Allocate DBAL handle to the returned dbal_table_id
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_name, &entry_handle_id));

    /*
     * Setting the entry access ID this indicates the line of the entry in the HW 
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_id_p->presel_id));

    /*
     * Perform the DBAL read
     * if the entry is found it is valid.
     */
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    if (rv == _SHR_E_NOT_FOUND)
    {
        entry_data_p->entry_valid = FALSE;
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
        entry_data_p->entry_valid = TRUE;

        /*
         * Iterate all supported DNX qualifiers for stage preselection
         */
        SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, &entry_data));
        SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_dnx_get(unit, entry_id_p->stage, &dnx_cs_qual_info, &dnx_cs_qual_nof));
        entry_data_p->nof_qualifiers = 0;
        for (qual_idx = 0; qual_idx < dnx_cs_qual_nof; qual_idx++)
        {
            /*
             * Get a key fields
             */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get(unit,
                                                                         entry_handle_id,
                                                                         dnx_cs_qual_info[qual_idx],
                                                                         &(entry_data.qual_data[qual_idx].qual_value),
                                                                         &(entry_data.qual_data[qual_idx].qual_mask)));

            /*
             * A non-zero mask indicates that the qualifier is used.
             * Only then we count it and fill the output structure with its data.
             */
            if (entry_data.qual_data[qual_idx].qual_mask)
            {
                entry_data_p->qual_data[entry_data_p->nof_qualifiers].qual_type_dbal_field = dnx_cs_qual_info[qual_idx];
                entry_data_p->qual_data[entry_data_p->nof_qualifiers].qual_value =
                    entry_data.qual_data[qual_idx].qual_value;
                entry_data_p->qual_data[entry_data_p->nof_qualifiers].qual_mask =
                    entry_data.qual_data[qual_idx].qual_mask;
                entry_data_p->nof_qualifiers++;
            }
        }

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, INST_SINGLE, &(entry_data_p->context_id)));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verifies input for dnx_field_presel_set function.
*
* \param [in] unit         - Device ID
* \param [in] flags        - preselection flags
* \param [in] entry_id_p   - Identifier of the preselector: 
*                          User-provided preselector ID and stage
* \param [in] entry_data_p - Data to be written in hardware: 
*                          PMF Context ID and valid, qualifiers
*                          data
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
*/
static shr_error_e
dnx_field_presel_set_verify(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * entry_id_p,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    int qual_idx;
    int qual_idx_2;
    unsigned int start_bit, nof_bits;
    unsigned int max_quals;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_id_p, _SHR_E_PARAM, "entry_id_p");
    SHR_NULL_CHECK(entry_data_p, _SHR_E_PARAM, "entry_data_p");

    if (entry_data_p->entry_valid)
    {
        DNX_FIELD_CONTEXT_IS_ALLOCATED(entry_id_p->stage, entry_data_p->context_id, 0);

        /*
         * Initialization to appease the compiler.
         */
        max_quals = 0;

        /*
         * Find the maximum number of qualifiers per stage.
         * Because in iPMF2 the user cannot use program selection profile, the maximum number for it is actually
         * one less then then the number we assign. however, we only test it later so that if we have
         * program selection profile, it will give an error for that and not number of qualifiers.
         */
        max_quals = DNX_FIELD_CS_QUAL_NOF(entry_id_p->stage);
        if (max_quals <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage for preselectors: \"%s\"(%d).\r\n",
                         dnx_field_stage_text(unit, entry_id_p->stage), entry_id_p->stage);
        }

        if (entry_data_p->nof_qualifiers > max_quals)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Number of qualifiers cannot exceed %d for stage \"%s\". "
                         "Received %d qualifiers for context %d.\r\n",
                         max_quals, dnx_field_stage_text(unit, entry_id_p->stage),
                         entry_data_p->nof_qualifiers, entry_data_p->context_id);
        }

        /** Sanity check, can be removed.*/
        if (entry_data_p->nof_qualifiers > DNX_FIELD_CS_QUAL_NOF_MAX)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Number of qualifiers cannot exceed array size of %d. "
                         "Received %d qualifiers for context %d.\r\n",
                         DNX_FIELD_CS_QUAL_NOF_MAX, entry_data_p->nof_qualifiers, entry_data_p->context_id);
        }

        for (qual_idx = 0; qual_idx < entry_data_p->nof_qualifiers; qual_idx++)
        {
            if (entry_data_p->qual_data[qual_idx].qual_type_dbal_field == DBAL_FIELD_PMF1_PROGRAM_SELECTION_PROFILE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Program Selection Profile preselector is for internal use only, qualifier index %d, "
                             "context ID %d.\r\n", qual_idx, entry_data_p->context_id);
            }
            /*
             * verify that InLIF profile doesn't exceed maimum profile ID according to configuration 
             */
            if ((entry_id_p->stage == DNX_FIELD_STAGE_IPMF1) &&
                ((entry_data_p->qual_data[qual_idx].qual_type_dbal_field == DBAL_FIELD_IN_LIF_PROFILE_MAP_0) ||
                 (entry_data_p->qual_data[qual_idx].qual_type_dbal_field == DBAL_FIELD_IN_LIF_PROFILE_MAP_1)))
            {
                SHR_IF_ERR_EXIT(dnx_field_port_profile_bits_get
                                (unit, DNX_FIELD_PORT_PROFILE_TYPE_IN_LIF_INGRESS, &start_bit, &nof_bits));
                if (entry_data_p->qual_data[qual_idx].qual_value >= (1 << nof_bits))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Inlif profile can't exceed the maximum profile %d.\r\n", ((1 << nof_bits) - 1));
                }
            }
            for (qual_idx_2 = 0; qual_idx_2 < qual_idx; qual_idx_2++)
            {
                if (entry_data_p->qual_data[qual_idx].qual_type_dbal_field ==
                    entry_data_p->qual_data[qual_idx_2].qual_type_dbal_field)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Qual type %d appears more than once in the preselector, in %d and %d, "
                                 "for context ID %d.\r\n",
                                 entry_data_p->qual_data[qual_idx].qual_type_dbal_field, qual_idx, qual_idx_2,
                                 entry_data_p->context_id);
                }
            }
        }

        /*
         * Checking for number of CS qualifiers given that program selection profile isn't an eligible qualifier.
         * Note we can remove this verification and fail on the illegal qualifier.
         */
        if ((entry_id_p->stage == DNX_FIELD_STAGE_IPMF2) && (entry_data_p->nof_qualifiers > max_quals - 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Number of qualifiers for iPMF2 cannot exceed %d. Received %d qaulifiers for context %d.\r\n",
                         max_quals - 1, entry_data_p->nof_qualifiers, entry_data_p->context_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function takes as argument a presel qualifier DBAL type and returns the corresponding FG type and key ID of the FG this qualifier cascades from
*  if the qualifier is a cascading qualifier, otherwise a FG type invalid is returned.
*
* \param [in] unit - Device ID
* \param [in] qual_dbal_type - The presel qualifier DBAL type
* \param [out] fg_type_p     - The FG type that the qualifier DBAL type cascades from
*                              (INVALID if qualifier DBAL type is not cascading)
* \param [out] fg_key_p      - The key ID of the FG that is cascaded by the qualifier DBAL type
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_presel_cascading_fg_type_key_get(
    int unit,
    dbal_fields_e qual_dbal_type,
    dnx_field_group_type_e * fg_type_p,
    dnx_field_key_id_t *fg_key_p)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (qual_dbal_type)
    {
        case DBAL_FIELD_STATE_TABLE_DATA:
            *fg_type_p = DNX_FIELD_GROUP_TYPE_STATE_TABLE;
            fg_key_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_J;
            break;
        case DBAL_FIELD_PMF1_TCAM_ACTION_0_MSB:
            *fg_type_p = DNX_FIELD_GROUP_TYPE_TCAM;
            fg_key_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_A;
            break;
        case DBAL_FIELD_PMF1_TCAM_ACTION_1_MSB:
            *fg_type_p = DNX_FIELD_GROUP_TYPE_TCAM;
            fg_key_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_B;
            break;
        case DBAL_FIELD_PMF1_TCAM_ACTION_2_MSB:
            *fg_type_p = DNX_FIELD_GROUP_TYPE_TCAM;
            fg_key_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_C;
            break;
        case DBAL_FIELD_PMF1_TCAM_ACTION_3_MSB:
            *fg_type_p = DNX_FIELD_GROUP_TYPE_TCAM;
            fg_key_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_D;
            break;
        default:
            *fg_type_p = DNX_FIELD_GROUP_TYPE_INVALID;
            fg_key_p->id[0] = DBAL_NOF_ENUM_FIELD_KEY_VALUES;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Increases the cascading reference of each FG referenced by the given cascading presel qualifiers on the given 'cascacded_from_context_id' context.
*  This function loops over all the given presel qualifiers, and for each one that is cascaded it
*  increases by one the cascading reference of the FG referenced by the qualifier.
*
* \param [in] unit                     - Device ID
* \param [in] cascaded_from_context_id - The cascaded from context ID
* \param [in] entry_data_p             - The presel qualifiers data
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_presel_cascading_refs_inc(
    int unit,
    dnx_field_context_t cascaded_from_context_id,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    int qual_idx;
    dnx_field_group_t fg_id;
    dnx_field_presel_qual_data_t qual_data;
    dnx_field_group_type_e fg_type;
    dnx_field_key_id_t fg_key;
    uint16 nof_cascading_refs;

    SHR_FUNC_INIT_VARS(unit);

    for (qual_idx = 0; qual_idx < entry_data_p->nof_qualifiers; qual_idx++)
    {
        qual_data = entry_data_p->qual_data[qual_idx];
        SHR_IF_ERR_EXIT(dnx_field_presel_cascading_fg_type_key_get
                        (unit, qual_data.qual_type_dbal_field, &fg_type, &fg_key));
        if (fg_type == DNX_FIELD_GROUP_TYPE_INVALID)
        {
            /*
             * Not a cascading qualifiers, Skip it 
             */
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_convert_context_key_to_fg_id
                        (unit, cascaded_from_context_id, fg_type, fg_key, &fg_id));
        
        if (fg_id == DNX_FIELD_GROUP_INVALID)
        {
            /*
             * User using a cascading presel qualifier with no equivalent FG, so we skip ref count 
             */
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        nof_cascading_refs.get(unit, fg_id, cascaded_from_context_id, &nof_cascading_refs));

        nof_cascading_refs++;

        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        nof_cascading_refs.set(unit, fg_id, cascaded_from_context_id, nof_cascading_refs));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Decreases the cascading reference of each FG referenced by the given cascading presel qualifiers on the given 'cascacded_from_context_id' context.
*  This function loops over all the given presel qualifiers, and for each one that is cascaded it
*  decreases by one the cascading reference of the FG referenced by the qualifier.
*
* \param [in] unit                     - Device ID
* \param [in] cascaded_from_context_id - The cascaded from context ID
* \param [in] entry_data_p             - The presel qualifiers data
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_presel_cascading_refs_dec(
    int unit,
    dnx_field_context_t cascaded_from_context_id,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    int qual_idx;
    dnx_field_group_t fg_id;
    dnx_field_presel_qual_data_t qual_data;
    dnx_field_group_type_e fg_type;
    dnx_field_key_id_t fg_key;
    uint16 nof_cascading_refs;

    SHR_FUNC_INIT_VARS(unit);

    for (qual_idx = 0; qual_idx < entry_data_p->nof_qualifiers; qual_idx++)
    {
        qual_data = entry_data_p->qual_data[qual_idx];
        SHR_IF_ERR_EXIT(dnx_field_presel_cascading_fg_type_key_get
                        (unit, qual_data.qual_type_dbal_field, &fg_type, &fg_key));
        if (fg_type == DNX_FIELD_GROUP_TYPE_INVALID)
        {
            /*
             * Not a cascading qualifiers, Skip it 
             */
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_convert_context_key_to_fg_id
                        (unit, cascaded_from_context_id, fg_type, fg_key, &fg_id));
        
        if (fg_id == DNX_FIELD_GROUP_INVALID)
        {
            /*
             * User using a cascading presel qualifier with no equivalent FG, so we skip ref count 
             */
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        nof_cascading_refs.get(unit, fg_id, cascaded_from_context_id, &nof_cascading_refs));

        if (nof_cascading_refs <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error decreasing cascading reference count for FG %d on context %d: Reference value is 0 or less\r\n",
                         fg_id, cascaded_from_context_id);
        }
        nof_cascading_refs--;

        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        nof_cascading_refs.set(unit, fg_id, cascaded_from_context_id, nof_cascading_refs));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Removes all qualifiers with zero masks from the entry data, as a qualifier with zero mask is identical to not 
*  having the qualifier in the entry.
* \param [in] unit                 - Device ID
* \param [in] entry_data_p         - The presel qualifiers data
* \param [out] entry_data_pruned_p - The presel qualifiers data without qualifiers with zero masks.
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_presel_prune_zero_masks(
    int unit,
    dnx_field_presel_entry_data_t * entry_data_p,
    dnx_field_presel_entry_data_t * entry_data_pruned_p)
{
    unsigned int qual_idx_src;
    unsigned int qual_idx_dst;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * To make sure no fields are missed, we first copy the entire structure.
     */
    sal_memcpy(entry_data_pruned_p, entry_data_p, sizeof(*entry_data_pruned_p));

    qual_idx_dst = 0;
    for (qual_idx_src = 0; qual_idx_src < entry_data_p->nof_qualifiers; qual_idx_src++)
    {
        if (entry_data_p->qual_data[qual_idx_src].qual_mask != 0)
        {
            sal_memcpy(&(entry_data_pruned_p->qual_data[qual_idx_dst]), &(entry_data_p->qual_data[qual_idx_src]),
                       sizeof(entry_data_pruned_p->qual_data[0]));
            qual_idx_dst++;
        }
    }

    entry_data_pruned_p->nof_qualifiers = qual_idx_dst;

    /*
     * Initialize the rest of the array. Not necessary.
     */
    for (; qual_idx_dst < DNX_FIELD_CS_QUAL_NOF_MAX; qual_idx_dst++)
    {
        SHR_IF_ERR_EXIT(dnx_field_presel_qual_data_t_init(unit, &(entry_data_pruned_p->qual_data[qual_idx_dst])));
    }

exit:
    SHR_FUNC_EXIT;
}

/* See field_presel.h */
shr_error_e
dnx_field_presel_set(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * entry_id_p,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    dnx_field_presel_entry_data_t entry_data_intern_pruned;
    int cascaded_idx;
    dnx_field_presel_entry_data_t prev_entry_data;
    dnx_field_context_t prev_cascaded_from;
    dnx_field_context_t cascaded_from;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_presel_set_verify(unit, flags, entry_id_p, entry_data_p));

    /*
     * Note we change entry_data_p to add the cascaded from context for the iPNF2 scenario.
     * We also prune it from the qualifiers that have zero masks.
     * For those two reasons, we use an internal variable.
     */
    SHR_IF_ERR_EXIT(dnx_field_presel_prune_zero_masks(unit, entry_data_p, &entry_data_intern_pruned));

    /*
     * For External stage, the context is actually the apptype, and we need to convert it to ACL context.
     */
    if (entry_id_p->stage == DNX_FIELD_STAGE_EXTERNAL && entry_data_intern_pruned.entry_valid)
    {
        dbal_enum_value_field_kbp_fwd_opcode_e opcode_id;
        dbal_enum_value_field_kbp_fwd_opcode_e base_static_opcode_id;
        uint8 fwd_nof_contexts;
        kbp_mngr_fwd_acl_context_mapping_t
            fwd_acl_ctx_mapping[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
        uint8 base_static_acl_ctx;
        bcm_field_AppType_t apptype;
        bcm_field_AppType_t base_static_apptype;

        apptype = entry_data_intern_pruned.context_id;

        if (apptype < bcmFieldAppTypeCount)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "For external stage, the Presel point to an Apptype and not a context. "
                         "The Apptype used in a presel must be dynamic. Apprtype used: %d.\n",
                         apptype);
        }

        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, apptype, &opcode_id));
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_to_contexts_get(unit, opcode_id, &fwd_nof_contexts, fwd_acl_ctx_mapping));
        /** Sanity check.*/
        if (fwd_nof_contexts != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For Apptype %d, opcode_id %d, the number of FWD contexts is %d."
                         "Only one context per opcode is supported.\n",
                         apptype, opcode_id, fwd_nof_contexts);
        }
        if (fwd_acl_ctx_mapping[0].nof_acl_contexts != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For Apptype %d, opcode_id %d, FWD context %d, "
                         "the number of ACL contexts is %d. Only one context per opcode is supported.\n",
                         apptype, opcode_id, fwd_acl_ctx_mapping[0].fwd_context,
                         fwd_acl_ctx_mapping[0].nof_acl_contexts);
        }

        /** Set the context to be the ACL context, not Apptype.*/
        entry_data_intern_pruned.context_id = fwd_acl_ctx_mapping[0].acl_contexts[0];

        /* Add a qualifier for the Apptype we cascaded from.*/
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_cascaded_get(unit, apptype, &base_static_apptype));
        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, base_static_apptype, &base_static_opcode_id));
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_to_contexts_get(unit, base_static_opcode_id, &fwd_nof_contexts, fwd_acl_ctx_mapping));
        
        if (fwd_nof_contexts != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For Apptype %d, opcode_id %d, the number of FWD contexts is %d."
                         "Only one context per opcode is supported.\n",
                         base_static_apptype, base_static_opcode_id, fwd_nof_contexts);
        }
        if (fwd_acl_ctx_mapping[0].nof_acl_contexts != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For Apptype %d, opcode_id %d, FWD context %d, "
                         "the number of ACL contexts is %d. Only one context per opcode is supported.\n",
                         base_static_apptype, base_static_opcode_id, fwd_acl_ctx_mapping[0].fwd_context,
                         fwd_acl_ctx_mapping[0].nof_acl_contexts);
        }
        base_static_acl_ctx = fwd_acl_ctx_mapping[0].acl_contexts[0];
        cascaded_idx = entry_data_intern_pruned.nof_qualifiers;
        entry_data_intern_pruned.qual_data[cascaded_idx].qual_type_dbal_field = DBAL_FIELD_FWD2_CONTEXT_ID;
        entry_data_intern_pruned.qual_data[cascaded_idx].qual_value = base_static_acl_ctx;
        entry_data_intern_pruned.qual_data[cascaded_idx].qual_mask = 0x3F;
        entry_data_intern_pruned.nof_qualifiers++;
        
        
    }

    if (entry_id_p->stage == DNX_FIELD_STAGE_IPMF2)
    {

        SHR_IF_ERR_EXIT(dnx_field_presel_get(unit, flags, entry_id_p, &prev_entry_data));
        if (prev_entry_data.entry_valid)
        {
            /*
             * Decrease the cascading reference count of each FG cascaded by the cascading presel qualifiers
             * that are now going to be removed from the CS TCAM.
             */
            SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                            (unit, entry_id_p->stage, prev_entry_data.context_id, &prev_cascaded_from));
            SHR_IF_ERR_EXIT(dnx_field_presel_cascading_refs_dec(unit, prev_cascaded_from, &prev_entry_data));
        }
        if (entry_data_intern_pruned.entry_valid)
        {
            int profile_num;

            SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                            (unit, entry_id_p->stage, entry_data_intern_pruned.context_id, &cascaded_from));

            /*
             * Increase the cascading reference count of each FG cascaded by the cascading presel qualifiers
             * that are now going to be added to the CS TCAM.
             */
            SHR_IF_ERR_EXIT(dnx_field_presel_cascading_refs_inc(unit, cascaded_from, &entry_data_intern_pruned));
            /*
             * Add the profile number of the cascaded_from context as a presel qual.
             * (This will make the preselection occur only when coming from the cascaded_from context)
             */
            SHR_IF_ERR_EXIT(dnx_algo_field_link_profile_get(unit, cascaded_from, &profile_num));
            /*
             * Sanity check 
             */
            if (profile_num == DNX_ALGO_FIELD_CONTEXT_LINK_PROFILE_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Link profile was not allocated for context %d", cascaded_from);
            }
            cascaded_idx = entry_data_intern_pruned.nof_qualifiers;
            entry_data_intern_pruned.qual_data[cascaded_idx].qual_type_dbal_field =
                DBAL_FIELD_PMF1_PROGRAM_SELECTION_PROFILE;
            entry_data_intern_pruned.qual_data[cascaded_idx].qual_value = profile_num;
            entry_data_intern_pruned.qual_data[cascaded_idx].qual_mask = 0x1F;
            entry_data_intern_pruned.nof_qualifiers++;
        }
    }
    SHR_IF_ERR_EXIT(dnx_field_presel_hw_set(unit, flags, entry_id_p, &entry_data_intern_pruned));

exit:
    SHR_FUNC_EXIT;
}

/* See field_presel.h */
shr_error_e
dnx_field_presel_get(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * entry_id_p,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_id_p, _SHR_E_PARAM, "entry_id_p");
    SHR_NULL_CHECK(entry_data_p, _SHR_E_PARAM, "entry_data_p");

    SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, entry_data_p));

    SHR_IF_ERR_EXIT(dnx_field_presel_hw_get(unit, flags, entry_id_p, entry_data_p));

    if (entry_id_p->stage == DNX_FIELD_STAGE_IPMF2 && entry_data_p->entry_valid)
    {
        int qual_idx, tmp_idx;
        uint8 found = FALSE;
        /**
         * For valid entries of iPMF2 stages we remove the auto added PROGRAM_SELECTION_PROFILE qual
         */
        for (qual_idx = 0; qual_idx < entry_data_p->nof_qualifiers; qual_idx++)
        {
            if (entry_data_p->qual_data[qual_idx].qual_type_dbal_field == DBAL_FIELD_PMF1_PROGRAM_SELECTION_PROFILE)
            {
                /*
                 * Remove element from array by shifting array to element place 
                 */
                for (tmp_idx = qual_idx; tmp_idx < entry_data_p->nof_qualifiers - 1; tmp_idx++)
                {
                    /*
                     * Struct assignment 
                     */
                    entry_data_p->qual_data[tmp_idx] = entry_data_p->qual_data[tmp_idx + 1];
                }
                entry_data_p->nof_qualifiers--;
                found = TRUE;
                break;
            }
        }
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Program Selection qualifier not present in iPMF2 preselection for context %d",
                         entry_data_p->context_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**See field_presel.h file*/
shr_error_e
dnx_field_presel_id_bcm_to_dnx_convert(
    int unit,
    uint32 flags,
    bcm_field_presel_entry_id_t * bcm_entry_id_p,
    dnx_field_presel_entry_id_t * dnx_entry_id_p)
{
    uint32 cs_nof_lines;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_entry_id_p, _SHR_E_PARAM, "bcm_entry_id_p");
    SHR_NULL_CHECK(dnx_entry_id_p, _SHR_E_PARAM, "dnx_entry_id_p");

    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_entry_id_p->stage, &(dnx_entry_id_p->stage)));
    dnx_entry_id_p->presel_id = bcm_entry_id_p->presel_id;
    /*
     * Check the correct stage, and get its max presel ID 
     */
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_nof_lines(unit, dnx_entry_id_p->stage, &cs_nof_lines));
    if (dnx_entry_id_p->presel_id >= cs_nof_lines)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "ContextSelector ID %d is not valid for PMF stage:\"%s\"\n"
                     "Please fill entry_id.presel_id within range 0 to %d !\n",
                     dnx_entry_id_p->presel_id, dnx_field_stage_text(unit, dnx_entry_id_p->stage), cs_nof_lines);
    }

exit:
    SHR_FUNC_EXIT;
}

/**See field_presel.h file*/
shr_error_e
dnx_field_presel_data_bcm_to_dnx_convert(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * entry_id_p,
    bcm_field_presel_entry_data_t * bcm_entry_data_p,
    dnx_field_presel_entry_data_t * dnx_entry_data_p)
{
    int qual_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_id_p, _SHR_E_PARAM, "bcm_entry_id_p");
    SHR_NULL_CHECK(bcm_entry_data_p, _SHR_E_PARAM, "bcm_entry_data_p");
    SHR_NULL_CHECK(dnx_entry_data_p, _SHR_E_PARAM, "dnx_entry_data_p");

    dnx_entry_data_p->entry_valid = bcm_entry_data_p->entry_valid;
    dnx_entry_data_p->context_id = bcm_entry_data_p->context_id;
    dnx_entry_data_p->nof_qualifiers = bcm_entry_data_p->nof_qualifiers;

    /*
     * Iterate all the used qualifiers
     */
    for (qual_idx = 0; qual_idx < bcm_entry_data_p->nof_qualifiers; qual_idx++)
    {
        /**
         * Context selection qualifier bcmFieldQualifyForwardingType
         * in IPMF3 stage works only for FWD layer 0.
         */
        if (bcm_entry_data_p->qual_data[qual_idx].qual_type == bcmFieldQualifyForwardingType &&
            entry_id_p->stage == DNX_FIELD_STAGE_IPMF3 && bcm_entry_data_p->qual_data[qual_idx].qual_arg != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Context selection qualifier bcmFieldQualifyForwardingType in IPMF3"
                         " stage works only for FWD layer 0 (qual_arg = 0)!\n");
        }
        /*
         * Map BCM qualifier to DNX qualifier.
         */
        SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_info_bcm_to_dnx(unit,
                                                              BCM_CORE_ALL,
                                                              entry_id_p->stage,
                                                              dnx_entry_data_p->context_id,
                                                              &bcm_entry_data_p->qual_data[qual_idx],
                                                              &dnx_entry_data_p->qual_data[qual_idx]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**See field_presel.h file*/
shr_error_e
dnx_field_presel_data_dnx_to_bcm_convert(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * dnx_entry_id_p,
    dnx_field_presel_entry_data_t * dnx_entry_data_p,
    bcm_field_presel_entry_data_t * bcm_entry_data_p)
{
    int qual_idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_entry_id_p, _SHR_E_PARAM, "dnx_entry_id_p");
    SHR_NULL_CHECK(dnx_entry_data_p, _SHR_E_PARAM, "dnx_entry_data_p");
    SHR_NULL_CHECK(bcm_entry_data_p, _SHR_E_PARAM, "bcm_entry_data_p");

    bcm_entry_data_p->entry_valid = dnx_entry_data_p->entry_valid;
    bcm_entry_data_p->context_id = dnx_entry_data_p->context_id;
    bcm_entry_data_p->nof_qualifiers = dnx_entry_data_p->nof_qualifiers;
    /*
     * Iterate all the used qualifiers
     */
    for (qual_idx = 0; qual_idx < dnx_entry_data_p->nof_qualifiers; qual_idx++)
    {
        /*
         * Map DNX qualifier to BCM qualifier
         */
        SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_dnx_to_bcm(unit,
                                                         dnx_entry_id_p->stage, dnx_entry_data_p->context_id,
                                                         dnx_entry_data_p->qual_data[qual_idx].qual_type_dbal_field,
                                                         &(bcm_entry_data_p->qual_data[qual_idx].qual_type),
                                                         (uint32 *) &(bcm_entry_data_p->qual_data[qual_idx].qual_arg)));
        if (bcm_entry_data_p->qual_data[qual_idx].qual_type == bcmFieldQualifyForwardingType)
        {
            dnx_field_map_layer_type_dnx_to_bcm(unit, dnx_entry_data_p->qual_data[qual_idx].qual_value,
                                                &(bcm_entry_data_p->qual_data[qual_idx].qual_value));
        }
        else if (bcm_entry_data_p->qual_data[qual_idx].qual_type == bcmFieldQualifyVlanFormat)
        {
            dnx_field_map_vlan_format_dnx_to_bcm(unit, dnx_entry_data_p->qual_data[qual_idx].qual_value,
                                                 &(bcm_entry_data_p->qual_data[qual_idx].qual_value));
        }
        else
        {
            bcm_entry_data_p->qual_data[qual_idx].qual_value = dnx_entry_data_p->qual_data[qual_idx].qual_value;
        }
        bcm_entry_data_p->qual_data[qual_idx].qual_mask = dnx_entry_data_p->qual_data[qual_idx].qual_mask;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Init a catch all entry for the default context for all stages.
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_presel_default_context_default_cs_init(
    int unit)
{
    dnx_field_presel_entry_id_t entry_id;
    dnx_field_presel_entry_data_t entry_data;
    uint32 cs_flags;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_presel_entry_id_t_init(unit, &entry_id));
    SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, &entry_data));
    
    entry_data.entry_valid = TRUE;
    entry_data.context_id = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);
    entry_data.nof_qualifiers = 0;
    cs_flags = 0;

    /** Configure context selection for iPMF1*/
    entry_id.stage = DNX_FIELD_STAGE_IPMF1;
    entry_id.presel_id = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_cs_lines - 1;
    SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, cs_flags, &entry_id, &entry_data));
    /** Configure context selection for iPMF3*/
    entry_id.stage = DNX_FIELD_STAGE_IPMF3;
    entry_id.presel_id = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF3)->nof_cs_lines - 1;
    SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, cs_flags, &entry_id, &entry_data));
    /** Configure context selection for ePMF*/
    entry_id.stage = DNX_FIELD_STAGE_EPMF;
    entry_id.presel_id = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EPMF)->nof_cs_lines - 1;
    SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, cs_flags, &entry_id, &entry_data));

exit:
    SHR_FUNC_EXIT;
}

/**See field_presel.h file*/
shr_error_e
dnx_field_presel_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_presel_default_context_default_cs_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**See field_presel.h file*/
shr_error_e
dnx_field_presel_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * The deinit function has nothing to deinit. 
     */
    SHR_FUNC_EXIT;
}

/** \file field_range.c
 *
 * Field range procedures for DNX.
 *
 * Management for range qualifier.
 */
#include <shared/bsl.h>
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/**
 * Include files.
 * {
 */
#include <bcm_int/dnx/field/field_range.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <bcm_int/dnx/field/field_map.h>

/**
 * }Include files
 */
/**
 * Range Types array.
 * Used in the mapping function dnx_field_range_type_text.
 * {
 */
const char *dnx_field_range_types_names[DNX_FIELD_RANGE_TYPE_NOF] = {
    [DNX_FIELD_RANGE_TYPE_L4_SRC_PORT] = "L4 Source Port Range",
    [DNX_FIELD_RANGE_TYPE_L4_DST_PORT] = "L4 Destination Port Range",
    [DNX_FIELD_RANGE_TYPE_OUT_LIF] = "Out LIF Range",
    [DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE] = "Packet Header Size Range"
};
/**
 * See header in field_range.h
 */
shr_error_e
dnx_field_range_info_t_init(
    int unit,
    dnx_field_range_info_t * range_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(range_info_p, _SHR_E_PARAM, "range_info_p");
    /**
     * Make the range and the min and max value invalid.
     */
    range_info_p->range_type = DNX_FIELD_RANGE_TYPE_INVALID;
    range_info_p->min_val = DNX_FIELD_RANGE_ID_INVALID;
    range_info_p->max_val = DNX_FIELD_RANGE_ID_INVALID;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_range.h
 */
char *
dnx_field_range_type_text(
    dnx_field_range_type_e range_type)
{
    char *range_type_name;
    if ((range_type < DNX_FIELD_RANGE_TYPE_FIRST) || (range_type >= DNX_FIELD_RANGE_TYPE_NOF))
    {
        range_type_name = "Invalid Range Type";
    }
    else if (dnx_field_range_types_names[range_type] == NULL)
    {
        range_type_name = "Unnamed Range Type";
    }
    else
    {
        range_type_name = (char *) dnx_field_range_types_names[range_type];
    }

    return range_type_name;
}
/**
 * \brief
 *  Verify all parameters.
 * \param [in] unit       - Device ID
 * \param [in] field_stage - Indicate the PMF stage
 * \param [in] range_id   - Identifier of the range: User-provided range ID
 * \param [in] range_info_p - The range_info struct.
 *            Using the range_type to verify the stage, range_id, min and max val for different types.
 * \param [in] is_for_set - Shows if the verify function is used in set or in get function.
 *                            In get function we need to check only the field_stage and the range_id!
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_range_verify(
    int unit,
    dnx_field_stage_e field_stage,
    uint32 range_id,
    dnx_field_range_info_t * range_info_p,
    uint8 is_for_set)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(range_info_p, _SHR_E_PARAM, "range_info_p");

    /**
     * Switching between all range types and verify the parameters
     */
    switch (range_info_p->range_type)
    {
        case DNX_FIELD_RANGE_TYPE_L4_SRC_PORT:
        case DNX_FIELD_RANGE_TYPE_L4_DST_PORT:
            /**
             * For L4 Ops the maximum range_id must be DNX_DATA_MAX_FIELD_IPMF1_NOF_L4_OPS_RANGES for IPMF1 and IPMF2
             * or DNX_DATA_MAX_FIELD_EPMF_NOF_RANGES_FOR_L4_OPS for EPMF.
             */
            if (!DNX_FIELD_RANGE_L4_RANGE_ID_VALID(unit, range_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             " The RANGE_ID: %d is bigger then DNX_DATA_MAX_FIELD_IPMF1_NOF_L4_OPS_RANGES/DNX_DATA_MAX_FIELD_EPMF_NOF_RANGES_FOR_L4_OPS! The range for L4 Ports is [0-DNX_DATA_MAX_FIELD_IPMF1_NOF_L4_OPS_RANGES]! \n",
                             range_id);
            }
            /**
             * The range [min_val,max_val] is being checked with the MACRO DNX_FIELD_RANGE_L4_RANGE_VALID.
             * We check it only if the verify is used for set.
             */
            if (is_for_set == DNX_FIELD_RANGE_IS_FOR_SET)
            {
                if (!DNX_FIELD_RANGE_L4_RANGE_VALID(range_info_p->min_val, range_info_p->max_val))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "The range for L4 Ops Range qualifier must be [DNX_FIELD_RANGE_QUALS_MIN_VAL-DNX_FIELD_RANGE_L4_OPS_MAX_VAL]. You give the range [%d - %d] \n",
                                 range_info_p->min_val, range_info_p->max_val);
                }
            }
            break;
        case DNX_FIELD_RANGE_TYPE_OUT_LIF:
            /**
             * For IPMF1,IPMF2 the range_id must be smaller then DNX_DATA_MAX_FIELD_IPMF1_NOF_RANGES_FOR_OUT_LIF.
             * For IPMF3 the range_id must be smaller then DNX_DATA_MAX_FIELD_IPMF3_NOF_RANGES_FOR_OUT_LIF.
             * The range [min_val,max_val] is being checked with the MACRO DNX_FIELD_RANGE_OUT_LIF_RANGE_VALID.
             */
            if (field_stage == DNX_FIELD_STAGE_IPMF1 || field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                if (!DNX_FIELD_RANGE_OUT_LIF_IPMF1_RANGE_ID_VALID(unit, range_id))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " The RANGE_ID: %d is bigger then DNX_DATA_MAX_FIELD_IPMF1_NOF_RANGES_FOR_OUT_LIF! \n",
                                 range_id);
                }
                if (is_for_set == DNX_FIELD_RANGE_IS_FOR_SET)
                {
                    if (!DNX_FIELD_RANGE_OUT_LIF_RANGE_VALID(range_info_p->min_val, range_info_p->max_val))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     " The range for OUT LIFF Range qualifier must be [DNX_FIELD_RANGE_QUALS_MIN_VAL-DNX_FIELD_RANGE_OUT_LIF_MAX_VAL]. You give the range [%d - %d]\n",
                                     range_info_p->min_val, range_info_p->max_val);
                    }
                }
            }
            else if (field_stage == DNX_FIELD_STAGE_IPMF3)
            {
                if (!DNX_FIELD_RANGE_OUT_LIF_IPMF3_RANGE_ID_VALID(unit, range_id))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " The RANGE_ID: %d is bigger then DNX_DATA_MAX_FIELD_IPMF3_NOF_RANGES_FOR_OUT_LIF! \n",
                                 range_id);
                }
                if (is_for_set == DNX_FIELD_RANGE_IS_FOR_SET)
                {
                    if (!DNX_FIELD_RANGE_OUT_LIF_RANGE_VALID(range_info_p->min_val, range_info_p->max_val))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     " The range for OUT LIFF Range qualifier must be [DNX_FIELD_RANGE_QUALS_MIN_VAL-DNX_FIELD_RANGE_OUT_LIF_MAX_VAL]. You give the range [%d - %d]\n",
                                     range_info_p->min_val, range_info_p->max_val);
                    }
                }
            }
            break;
        case DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE:
            /**
             * For Packet Header the stage must be IPMF1 or IPMF2.
             * For IPMF1,IPMF2 the range_id must be smaller then DNX_DATA_MAX_FIELD_IPMF1_NOF_PKT_HDR_RANGES.
             * The range [min_val,max_val] is being checked with the MACRO DNX_FIELD_RANGE_PKT_HDR_RANGE_VALID.
             */
            if (field_stage == DNX_FIELD_STAGE_IPMF1 || field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                if (!DNX_FIELD_RANGE_PKT_HDR_RANGE_ID_VALID(unit, range_id))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " The RANGE_ID: %d is bigger then DNX_DATA_MAX_FIELD_IPMF1_NOF_PKT_HDR_RANGES! \n",
                                 range_id);
                }
                if (is_for_set == DNX_FIELD_RANGE_IS_FOR_SET)
                {
                    if (!DNX_FIELD_RANGE_PKT_HDR_RANGE_VALID(range_info_p->min_val, range_info_p->max_val))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     " The range for PKT HDR Range qualifier must be [DNX_FIELD_RANGE_QUALS_MIN_VAL-255]. You give the range [%d - %d]\n",
                                     range_info_p->min_val, range_info_p->max_val);
                    }
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal range type = %s for range_id %d\n",
                         dnx_field_range_type_text(range_info_p->range_type), range_id);
            break;
    }
    /**
     * Check if the max_val is bigger than min_val.
     */
    if (range_info_p->max_val < range_info_p->min_val)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " The maximum value: %d must be bigger or equal then minimum value: %d \n",
                     range_info_p->max_val, range_info_p->min_val);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_range.h
 */
shr_error_e
dnx_field_range_set(
    int unit,
    uint32 flags,
    dnx_field_stage_e field_stage,
    uint32 range_id,
    dnx_field_range_info_t * range_info_p)
{
    uint32 entry_handle_id;
    dnx_field_dbal_entry_t field_dbal_entry;
    bcm_field_stage_t bcm_stage;
    bcm_field_range_info_t bcm_range_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(range_info_p, _SHR_E_PARAM, "range_info_p");

    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "Setting Range info to range ID %d...\n"), range_id));

    /**
     * Covert the dnx_stage to bcm
     */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));
    /**
     * Convert range_info struct from dnx to bcm
     */
    SHR_IF_ERR_EXIT(dnx_field_map_range_info_dnx_to_bcm(unit, *range_info_p, &bcm_range_info));
    /**
     * Get all needed dbal Fields.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_range_bcm_to_dnx(unit, bcm_stage, range_id, &bcm_range_info, &field_dbal_entry));
    /**
     * Verify the range_id, min and max for each range_type
     */
    SHR_IF_ERR_EXIT(dnx_field_range_verify(unit, field_stage, range_id, range_info_p, DNX_FIELD_RANGE_IS_FOR_SET));
    /**
     * Allocate DBAL handle to the returned dbal_table_id.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, field_dbal_entry.table_id, &entry_handle_id));
    /**
     * Setting the DBAL table key.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, field_dbal_entry.key_dbal_pair[0].field_id, range_id);
    /**
     * Setting the DBAL fields.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, field_dbal_entry.res_dbal_pair[0].field_id, INST_SINGLE,
                                 range_info_p->min_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, field_dbal_entry.res_dbal_pair[1].field_id, INST_SINGLE,
                                 range_info_p->max_val);
    /**
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See header in field_range.h
 */
shr_error_e
dnx_field_range_get(
    int unit,
    dnx_field_stage_e field_stage,
    uint32 *range_id_p,
    dnx_field_range_info_t * range_info_p)
{
    uint32 entry_handle_id;
    dnx_field_dbal_entry_t field_dbal_entry;
    bcm_field_stage_t bcm_stage;
    bcm_field_range_info_t bcm_range_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(range_info_p, _SHR_E_PARAM, "range_info_p");

    /**
     * Covert the dnx_stage to bcm
     */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));
    /**
     * Convert range_info struct from dnx to bcm
     */
    SHR_IF_ERR_EXIT(dnx_field_map_range_info_dnx_to_bcm(unit, *range_info_p, &bcm_range_info));
    /**
     * Get all needed dbal Fields.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_range_bcm_to_dnx(unit, bcm_stage, *range_id_p, &bcm_range_info, &field_dbal_entry));
    /**
     * Verify the stage, * range_id_p for each range_type
     */
    SHR_IF_ERR_EXIT(dnx_field_range_verify(unit, field_stage, *range_id_p, range_info_p, DNX_FIELD_RANGE_IS_FOR_GET));
    /**
     * Allocate DBAL handle to the returned dbal_table_name
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, field_dbal_entry.table_id, &entry_handle_id));
    /**
     * Setting the DBAL table key.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, field_dbal_entry.key_dbal_pair[0].field_id, *range_id_p);
    /**
     * Get the min_val
     */
    dbal_value_field32_request(unit, entry_handle_id, field_dbal_entry.res_dbal_pair[0].field_id, INST_SINGLE,
                               &(range_info_p->min_val));
    /**
     * Get the max_val
     */
    dbal_value_field32_request(unit, entry_handle_id, field_dbal_entry.res_dbal_pair[1].field_id, INST_SINGLE,
                               &(range_info_p->max_val));
    /** 
     * Performing the action after this call the pointers that we set in field32_request()
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

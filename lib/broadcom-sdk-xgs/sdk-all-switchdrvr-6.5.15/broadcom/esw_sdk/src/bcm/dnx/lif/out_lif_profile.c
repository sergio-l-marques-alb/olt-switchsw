/** \file out_lif_profile.c
 *
 *  out_lif profile management for DNX.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF
/*
 * Include files
 * {
 */
#include <bcm_int/dnx/algo/lif_mngr/algo_out_lif_profile.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/l3/l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_profile_access.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>

/*
 * }
 */
/*
 * Defines and Macros
 * {
 */
/*
 * }
 */
/**
 * \brief - Configure ETPP_OUT_LIF_PROFILE_TABLE table 
 */
static shr_error_e
dnx_etpp_out_lif_profile_configure_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * ETPP_OUT_LIF_PROFILE_TABLE init
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETPP_OUT_LIF_PROFILE_TABLE, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_ORIENTATION, INST_SINGLE,
                                 DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE,
                                 DNX_OUT_LIF_PROFILE_SPLIT_HORIZON_ENABLE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PRIORITY_MAP_PROFILE, INST_SINGLE,
                                 DNX_OUT_LIF_PROFILE_OAM_PROFILE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure ERPP_OUT_LIF_PROFILE_TABLE table 
 */
static shr_error_e
dnx_erpp_out_lif_profile_filters_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * ERPP_OUT_LIF_PROFILE_TABLE init
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ERPP_OUT_LIF_PROFILE_TABLE, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_MULTICAST_SAME_INTERFACE_FILTERS, INST_SINGLE,
                                 TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_UC_SAME_INTERFACE_FILTERS, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_INDICATION_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RQP_DISCARD_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INVALID_OTM_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DSS_STACKING_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAG_MULTICAST_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXCLUDE_SRC_ENABLE, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_EQUALS_DESTINATION_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UNACCEPTABLE_FRAME_TYPE_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UNKNOWN_DA_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLEM_PP_TRAP_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLEM_NON_PP_TRAP_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_SCOPING_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_ZERO_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_ONE_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MTU_CHECK_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_FILTERS_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_FILTERS_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_out_lif_profile_module_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_configure_init(unit));
    SHR_IF_ERR_EXIT(dnx_erpp_out_lif_profile_filters_init(unit));

exit:

    SHR_FUNC_EXIT;
}

/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_out_lif_profile_module_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * The respective DeInit functions need to be put here.
     */
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_out_lif_profile_exchange(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 flags,
    out_lif_profile_info_t * out_lif_profile_info,
    int old_out_lif_profile,
    int *new_out_lif_profile)
{
    out_lif_template_data_t out_lif_profile_template_data;
    out_lif_template_data_orientation_t out_lif_template_data_orientation;
    out_lif_template_data_oam_profile_t default_template_data_oam_profile;
    uint32 entry_handle_id;
    uint8 is_last, is_first;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * local variables init
     */
    sal_memset(&out_lif_template_data_orientation, 0, sizeof(out_lif_template_data_orientation_t));
    sal_memset(&out_lif_profile_template_data, 0, sizeof(out_lif_template_data_t));
    sal_memset(&default_template_data_oam_profile, 0, sizeof(out_lif_template_data_oam_profile_t));
    is_last = 0;
    is_first = 0;

    /** If the table is IPV4_TUNNEL, calculate according to the ARR prefix and support only split horizon */
    if ((dbal_table_id == DBAL_TABLE_EEDB_IPV4_TUNNEL) ||
            (dbal_table_id == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB) ||
            (dbal_table_id == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB))
    {
        out_lif_template_data_orientation.out_lif_orientation = out_lif_profile_info->out_lif_orientation;
        SHR_IF_ERR_EXIT(algo_lif_profile_db.out_lif_profile_orientation.exchange
                            (unit, _SHR_CORE_ALL, 0, &out_lif_template_data_orientation, old_out_lif_profile,
                             NULL, new_out_lif_profile, &is_first, &is_last));
    }
    /** If the table is MPLS_TUNNEL, calculate according to the ARR prefix and support only oam profile */
    else if (dbal_table_id == DBAL_TABLE_EEDB_MPLS_TUNNEL)
    {
        default_template_data_oam_profile.oam_lif_profile = out_lif_profile_info->oam_lif_profile;
        SHR_IF_ERR_EXIT(algo_lif_profile_db.out_lif_profile_oam_profile.exchange
                                    (unit, _SHR_CORE_ALL, 0, &default_template_data_oam_profile, old_out_lif_profile,
                                     NULL, new_out_lif_profile, &is_first, &is_last));
    }
    else
    {
        /** Allow all field to be managed for 6b range of outlif profile*/
        out_lif_profile_template_data.out_lif_orientation = out_lif_profile_info->out_lif_orientation;
        out_lif_profile_template_data.etpp_split_horizon_enable = DNX_OUT_LIF_PROFILE_SPLIT_HORIZON_ENABLE;
        out_lif_profile_template_data.oam_lif_profile = out_lif_profile_info->oam_lif_profile;
        SHR_IF_ERR_EXIT(algo_lif_profile_db.out_lif_profile.exchange
                        (unit, _SHR_CORE_ALL, 0, &out_lif_profile_template_data, old_out_lif_profile,
                         NULL, new_out_lif_profile, &is_first, &is_last));
    }
    /*
     * write to HW
     * if we have a new out_lif_profile then write out_lif_profile properties to HW
     */
    if (is_first)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETPP_OUT_LIF_PROFILE_TABLE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, *new_out_lif_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_ORIENTATION, INST_SINGLE,
                                     out_lif_profile_info->out_lif_orientation);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE,
                                     DNX_OUT_LIF_PROFILE_SPLIT_HORIZON_ENABLE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PRIORITY_MAP_PROFILE, INST_SINGLE,
                                     out_lif_profile_info->oam_lif_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   out_lif_profile.h file
 */
void
out_lif_profile_info_t_init(
    int unit,
    out_lif_profile_info_t * out_lif_profile_info)
{
    out_lif_profile_info->out_lif_orientation = DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION;
    out_lif_profile_info->etpp_split_horizon_enable = DNX_OUT_LIF_PROFILE_SPLIT_HORIZON_ENABLE;
    out_lif_profile_info->oam_lif_profile = DNX_OUT_LIF_PROFILE_OAM_PROFILE;
}

/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_out_lif_profile_get_data(
    int unit,
    int out_lif_profile,
    out_lif_profile_info_t * out_lif_profile_info,
    dbal_tables_e dbal_table_id)
{
    int ref_count;
    out_lif_template_data_t out_lif_profile_template_data;
    out_lif_template_data_orientation_t out_lif_template_data_orientation;
    out_lif_template_data_oam_profile_t default_template_data_oam_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&out_lif_template_data_orientation, 0, sizeof(out_lif_template_data_orientation_t));
    sal_memset(&out_lif_profile_template_data, 0, sizeof(out_lif_template_data_t));
    sal_memset(&default_template_data_oam_profile, 0, sizeof(out_lif_template_data_oam_profile_t));

    SHR_NULL_CHECK(out_lif_profile_info, _SHR_E_PARAM, "NULL input - out_lif_profile_info");
    /*
     * Make sure the whole output structure is, initially, set to zero.
     */
    sal_memset(out_lif_profile_info, 0, sizeof(*out_lif_profile_info));

    /*
     * check if out_lif_profile is in range
     */
    if ((out_lif_profile < 0) | (out_lif_profile >= dnx_data_lif.out_lif.nof_out_lif_profiles_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "out_lif_profile out of range : %d", out_lif_profile);
    }

    /** If the table is IPV4_TUNNEL, calculate according to the ARR prefix and support only split horizon */
    if ((dbal_table_id == DBAL_TABLE_EEDB_IPV4_TUNNEL) ||
                (dbal_table_id == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB) ||
                (dbal_table_id == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB))
    {
        SHR_IF_ERR_EXIT(algo_lif_profile_db.out_lif_profile_orientation.profile_data_get
                            (unit, _SHR_CORE_ALL, out_lif_profile, &ref_count, &out_lif_template_data_orientation));
        out_lif_profile_info->out_lif_orientation = out_lif_template_data_orientation.out_lif_orientation;
    }
    else if (dbal_table_id == DBAL_TABLE_EEDB_MPLS_TUNNEL)
    {
        SHR_IF_ERR_EXIT(algo_lif_profile_db.out_lif_profile_oam_profile.profile_data_get
                                    (unit, _SHR_CORE_ALL, out_lif_profile, &ref_count, &default_template_data_oam_profile));
        out_lif_profile_info->oam_lif_profile = default_template_data_oam_profile.oam_lif_profile;
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_lif_profile_db.out_lif_profile.profile_data_get
                        (unit, _SHR_CORE_ALL, out_lif_profile, &ref_count, &out_lif_profile_template_data));
        /*
         * assign out_lif_orientation, split horizon and oam profile
         */
        out_lif_profile_info->out_lif_orientation = out_lif_profile_template_data.out_lif_orientation;
        out_lif_profile_info->etpp_split_horizon_enable = out_lif_profile_template_data.etpp_split_horizon_enable;
        out_lif_profile_info->oam_lif_profile = out_lif_profile_template_data.oam_lif_profile;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_out_lif_profile_get_ref_count(
    int unit,
    int out_lif_profile,
    int *ref_count)
{
    out_lif_template_data_t out_lif_profile_template_data;
    out_lif_template_data_orientation_t out_lif_template_data_orientation;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&out_lif_profile_template_data, 0, sizeof(out_lif_profile_template_data));
    sal_memset(&out_lif_template_data_orientation, 0, sizeof(out_lif_template_data_orientation_t));

    if (out_lif_profile < DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE)
    {
        SHR_IF_ERR_EXIT(algo_lif_profile_db.out_lif_profile.profile_data_get
                (unit,_SHR_CORE_ALL, out_lif_profile, ref_count, &out_lif_profile_template_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_lif_profile_db.out_lif_profile_orientation.profile_data_get
                        (unit,_SHR_CORE_ALL, out_lif_profile, ref_count, &out_lif_template_data_orientation));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_out_lif_profile_set(
    int unit,
    bcm_gport_t port,
    int new_out_lif_profile)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dbal_tables_e dbal_table_id;
    int local_out_lif;
    uint32 entry_handle_id, res_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    local_out_lif = gport_hw_resources.local_out_lif;
    dbal_table_id = gport_hw_resources.outlif_dbal_table_id;
    res_type = gport_hw_resources.outlif_dbal_result_type;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, res_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, new_out_lif_profile);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_out_lif_profile_get(
    int unit,
    bcm_gport_t port,
    int *out_lif_profile_p)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dbal_tables_e dbal_table_id;
    int local_out_lif, result_type;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(out_lif_profile_p, _SHR_E_PARAM, "old_out_lif_profile_p");

    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    local_out_lif = gport_hw_resources.local_out_lif;
    dbal_table_id = gport_hw_resources.outlif_dbal_table_id;
    result_type = gport_hw_resources.outlif_dbal_result_type;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /** read entry OutLIF profile */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, (uint32 *) out_lif_profile_p));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

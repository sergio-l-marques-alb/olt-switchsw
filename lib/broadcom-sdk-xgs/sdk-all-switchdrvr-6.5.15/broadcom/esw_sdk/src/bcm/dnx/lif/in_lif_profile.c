/** \file in_lif_profile.c
 *
 *  in_lif profile management for DNX.
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
#include <bcm_int/dnx/algo/lif_mngr/algo_in_lif_profile.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_profile_access.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>

/*
 * }
 */

/*
 * Defines
 */
/*
 * defines the mask of the in_lif_profile bits that are used for the ingress attributes template manager
 */
#define IN_LIF_PROFILE_USED_BITS_MASK (NOF_IN_LIF_PROFILES - 1)
/*
 * defines the mask of the eth_rif_profile bits that are used for the ingress attributes template manager
 */
#define ETH_RIF_PROFILE_USED_BITS_MASK (NOF_ETH_RIF_PROFILES - 1)
/*
 * defines the mask of the eth_rif_profile bits that are used for the egress attributes
 */
#define ETH_RIF_EGRESS_PROFILE_MASK (1 << (ETH_RIF_PROFILE_EGRESS_RESERVED_NOF_BITS + ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS)) - 1
/*
 * defines the mask of the in_lif_profile bits that are used for the egress attributes
 */
#define IN_LIF_EGRESS_PROFILE_MASK (1 << (IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS + IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS)) - 1
/*
 * defines the offset of the in_lif_profile bit that is used for the IPMC bridge fallback
 */
#define IN_LIF_PROFILE_IPMC_BRIDGE_FALLBACK_BIT_OFFSET 3
/*
 * defines the max number of supported bits for in_lif_profile in JR sys header mode
 */
#define IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE 2
/*
 * defines the mask of the in_lif_profile lsb
 */
#define IN_LIF_PROFILE_MAX_USED_BIT_MASK 1

/*
 * Enums
 */
/*
 * this enum is used to get cs_in_lif_profile
 * see function dnx_in_lif_profile_algo_convert_to_cs_in_lif_profile
 */
typedef enum
{
    /**
     * LIF type is PWE
     */
    PWE,
    /**
     * LIF type is GRE8
     */
    GRE8,
    /*
     * LIF type is LSP
     */
    LSP,
    /*
     * LIF type is ETH-RIF
     */
    RIF
} in_lif_lif_type_e;

/**
 * \brief -
 *  returns the egress part of in_lif_profile
 *
 * \param [in] unit - unit number
 * \param [in] egress_fields_info - a struct that includes the given egress fields values
 * \param [in] old_in_lif_profile - old inlif profile
 * \param [out] egress_in_lif_profile - the return egress in_lif_profile value
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   none
 *
 * \see
 *   dnx_in_lif_profile_exchange.
 */
static shr_error_e
dnx_in_lif_profile_algo_convert_to_egress_profile(
    int unit,
    in_lif_profile_info_egress_t egress_fields_info,
    int old_in_lif_profile,
    int *egress_in_lif_profile)
{
    int in_lif_profile_orientation_number_of_values, inner_dp_number_of_values, outer_dp_number_of_values,
        same_interface_number_of_values, system_headers_mode, old_egress_in_lif_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(egress_in_lif_profile, _SHR_E_PARAM, "A valid pointer for egress_in_lif_profile is needed!");
    same_interface_number_of_values = dnx_data_lif.in_lif.in_lif_profile_allocate_same_interface_mode_get(unit);
    outer_dp_number_of_values = dnx_data_lif.in_lif.in_lif_profile_allocate_policer_outer_dp_get(unit);
    inner_dp_number_of_values = dnx_data_lif.in_lif.in_lif_profile_allocate_policer_inner_dp_get(unit);
    in_lif_profile_orientation_number_of_values = dnx_data_lif.in_lif.in_lif_profile_allocate_orientaion_get(unit);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {

        if (utilex_log2_round_up(in_lif_profile_orientation_number_of_values) <
            utilex_log2_round_up(egress_fields_info.in_lif_orientation))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Not enough number of values for orientation were allocated!\n");
        }

        if (utilex_log2_round_up(inner_dp_number_of_values) <
            utilex_log2_round_up(egress_fields_info.ive_inner_qos_dp_profile))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Not enough number of values for inner dp were allocated!\n");
        }

        if (utilex_log2_round_up(outer_dp_number_of_values) <
            utilex_log2_round_up(egress_fields_info.ive_outer_qos_dp_profile))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Not enough number of values for outer DP were allocated!\n");
        }

        if (utilex_log2_round_up(same_interface_number_of_values) <
            utilex_log2_round_up(egress_fields_info.lif_same_interface_mode))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Not enough number of values for same interface were allocated!\n");
        }
        *egress_in_lif_profile = egress_fields_info.ive_inner_qos_dp_profile |
            (egress_fields_info.ive_outer_qos_dp_profile << IVE_INNER_QOS_DP_PROFILE_NOF_BITS) |
            (egress_fields_info.in_lif_orientation << (IVE_INNER_QOS_DP_PROFILE_NOF_BITS +
                                                       IVE_OUTER_QOS_DP_PROFILE_NOF_BITS)) |
            (egress_fields_info.lif_same_interface_mode <<
             (IVE_INNER_QOS_DP_PROFILE_NOF_BITS + IVE_OUTER_QOS_DP_PROFILE_NOF_BITS + IN_LIF_ORIENTATION_NOF_BITS));
    }
    else
    {
        /** If orientation is enabled by soc property, then the in_lif_profile is populated only with its values*/
        if ((in_lif_profile_orientation_number_of_values > 0) && (same_interface_number_of_values == 2))
        {
            *egress_in_lif_profile = ((egress_fields_info.in_lif_orientation << 1) & IN_LIF_PROFILE_MAX_USED_BIT_MASK)
                | (egress_fields_info.lif_same_interface_mode & IN_LIF_PROFILE_MAX_USED_BIT_MASK);
        }
        /** only orientation is enabled*/
        else if ((in_lif_profile_orientation_number_of_values > 0) && (same_interface_number_of_values == 0))
        {
            *egress_in_lif_profile = egress_fields_info.in_lif_orientation;
        }
        /** If both are enabled, the same if for bridge is the LSB and the logical same if is the MSB*/
        else if ((same_interface_number_of_values == 3)
                 || ((in_lif_profile_orientation_number_of_values == 0) && (same_interface_number_of_values == 1)))
        {
            old_egress_in_lif_profile =
                old_in_lif_profile >> (IN_LIF_PROFILE_PMF_RESERVED_START_BIT - IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE);
            if (egress_fields_info.lif_same_interface_mode == DEVICE_SCOPE)
            {
                *egress_in_lif_profile = old_egress_in_lif_profile | 1;
            }
            else if (egress_fields_info.lif_same_interface_mode == DEVICE_SCOPE_DISABLE)
            {
                *egress_in_lif_profile = old_egress_in_lif_profile & 2;
            }
            else if (egress_fields_info.lif_same_interface_mode == SYSTEM_SCOPE)
            {
                *egress_in_lif_profile = (old_egress_in_lif_profile & 1) | 2;
            }
            else if (egress_fields_info.lif_same_interface_mode == SYSTEM_SCOPE_DISABLE)
            {
                *egress_in_lif_profile = (old_egress_in_lif_profile & 1);
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  extracts egress in_lif_profile values from in_lif_profile
 *
 * \param [in] unit - unit number
 * \param [in] in_lif_profile - the whole in_lif_profile value
 * \param [in] intf_type - intf type
 * \param [out] egress_in_lif_profile_info - the return struct that includes egress in_lif_profile fields values
 * \param [out] in_lif_profile_jr_mode - the inlif profile in jr sys headers mode
 * \param [out] in_lif_profile_exclude_orientation - the inlif profile in jr sys headers mode excluding orientation
 * \param [out] in_lif_orientation - the inlif orientation in jr sys headers mode
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   none
 *
 * \see
 *   dnx_in_lif_profile_exchange.
 */
static shr_error_e
dnx_in_lif_profile_algo_extract_egress_profile(
    int unit,
    int in_lif_profile,
    in_lif_profile_info_egress_t * egress_in_lif_profile_info,
    int *in_lif_profile_jr_mode,
    int *in_lif_profile_exclude_orientation,
    int *in_lif_orientation,
    in_lif_intf_type_e intf_type)
{

    int egress_in_lif_profile;
    int orientation, same_interface, system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(egress_in_lif_profile_info, _SHR_E_PARAM,
                   "A valid pointer for egress_in_lif_profile_info is needed!");

    egress_in_lif_profile = 0;
    orientation = dnx_data_lif.in_lif.in_lif_profile_allocate_orientaion_get(unit);
    same_interface = dnx_data_lif.in_lif.in_lif_profile_allocate_same_interface_mode_get(unit);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        if (intf_type == ETH_RIF)
        {
            egress_in_lif_profile = in_lif_profile >> ETH_RIF_PROFILE_EGRESS_RESERVED_START_BIT;
        }
        else
        {
            egress_in_lif_profile = in_lif_profile >> IN_LIF_PROFILE_EGRESS_RESERVED_START_BIT;
        }

        (*egress_in_lif_profile_info).ive_inner_qos_dp_profile =
            egress_in_lif_profile & ((1 << IVE_INNER_QOS_DP_PROFILE_NOF_BITS) - 1);
        egress_in_lif_profile = egress_in_lif_profile >> IVE_INNER_QOS_DP_PROFILE_NOF_BITS;
        (*egress_in_lif_profile_info).ive_outer_qos_dp_profile =
            egress_in_lif_profile & ((1 << IVE_OUTER_QOS_DP_PROFILE_NOF_BITS) - 1);
        egress_in_lif_profile = egress_in_lif_profile >> IVE_OUTER_QOS_DP_PROFILE_NOF_BITS;
        (*egress_in_lif_profile_info).in_lif_orientation =
            egress_in_lif_profile & ((1 << IN_LIF_ORIENTATION_NOF_BITS) - 1);
        egress_in_lif_profile = egress_in_lif_profile >> IN_LIF_ORIENTATION_NOF_BITS;
        (*egress_in_lif_profile_info).lif_same_interface_mode =
            egress_in_lif_profile & ((1 << SAME_INTERFACE_MODE_NOF_BITS) - 1);
        *in_lif_orientation = 0;
        *in_lif_profile_exclude_orientation = 0;
        *in_lif_profile_jr_mode = 0;
    }
    else
    {
        egress_in_lif_profile =
            in_lif_profile >> (IN_LIF_PROFILE_PMF_RESERVED_START_BIT - IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE);
        /** If orientation is enabled by soc property, and same interface is not,
         * then the in_lif_profile is populated only with its values and orienation could be in full range
         */
        if ((orientation > 0) && (same_interface == 0))
        {
            egress_in_lif_profile_info->in_lif_orientation = egress_in_lif_profile;
            *in_lif_profile_jr_mode = egress_in_lif_profile_info->in_lif_orientation;
            *in_lif_profile_exclude_orientation = egress_in_lif_profile_info->in_lif_orientation;
            *in_lif_orientation = egress_in_lif_profile_info->in_lif_orientation;
        }
        /** If orientation and 1b same interface is enabled, then orientation is limited to up to 2 values - 0/1 */
        else if ((orientation > 0) && (same_interface == 2))
        {
            egress_in_lif_profile_info->in_lif_orientation =
                (egress_in_lif_profile >> 1) & IN_LIF_PROFILE_MAX_USED_BIT_MASK;;
            egress_in_lif_profile_info->lif_same_interface_mode =
                egress_in_lif_profile & IN_LIF_PROFILE_MAX_USED_BIT_MASK;
            *in_lif_profile_jr_mode = egress_in_lif_profile_info->in_lif_orientation;
        }
        /** If both are enabled, the same if for bridge is the LSB and the logical same if is the MSB*/
        else if ((orientation == 0) && (same_interface == 3))
        {
            egress_in_lif_profile_info->lif_same_interface_mode = egress_in_lif_profile;
            *in_lif_profile_jr_mode = egress_in_lif_profile_info->lif_same_interface_mode;
            *in_lif_profile_exclude_orientation = *in_lif_profile_jr_mode;
            *in_lif_orientation = 0;
        }
        /** If only 1 bit for same if is enabled - the profile is limited to 0/1 values*/
        else if ((orientation == 0) && (same_interface == 2))
        {
            egress_in_lif_profile_info->lif_same_interface_mode =
                egress_in_lif_profile & IN_LIF_PROFILE_MAX_USED_BIT_MASK;
            *in_lif_profile_jr_mode = egress_in_lif_profile_info->lif_same_interface_mode;
            *in_lif_profile_exclude_orientation = *in_lif_profile_jr_mode;
            *in_lif_orientation = 0;
        }
        (*egress_in_lif_profile_info).ive_inner_qos_dp_profile = 0;
        (*egress_in_lif_profile_info).ive_outer_qos_dp_profile = 0;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  write all egress possible values to HW in init time:
 *  egress attributes are retrieved from the egress reserved bits in a direct way,
 *  no template manager is used, therefore the values we write to egress HW is known
 *  in advanced and in order to save time and enhance performace we do all the HW
 *  work in init stage, after that we only return profiles that point to these HW entries.
 *  when exchange api is called no egress HW is accessed.
 *
 * \param [in] unit - unit number
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   none
 *
 * \see
 *   dnx_in_lif_profile_module_init.
 */
static shr_error_e
dnx_in_lif_profile_set_egress_hw(
    int unit)
{
    uint32 entry_handle_id;
    int pmf_reserved_nof_bits, nof_ingress_profiles, system_headers_mode, in_lif_profile_jr_mode;
    int pmf_reserved_bits_value, ingress_bits_value, egress_reserved_start_bit, same_interface;
    in_lif_profile_info_egress_t egress_fields_info;
    int in_lif_egress_profile, max_in_lif_egress_profile, in_lif_profile_exclude_orientation, in_lif_orientation;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    pmf_reserved_nof_bits = 0;
    nof_ingress_profiles = 0;
    pmf_reserved_bits_value = 0;
    ingress_bits_value = 0;
    in_lif_profile_exclude_orientation = 0;
    in_lif_orientation = 0;
    sal_memset(&egress_fields_info, 0, sizeof(in_lif_profile_info_egress_t));

    /*
     * write all possible egress HW combinations, we do this part at init in order to save time
     * when we exchange, these written HW values won't change anyway
     */
    pmf_reserved_nof_bits = IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS;
    nof_ingress_profiles = NOF_IN_LIF_PROFILES;

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    same_interface = dnx_data_lif.in_lif.in_lif_profile_allocate_same_interface_mode_get(unit);
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        max_in_lif_egress_profile = (1 << IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS) - 1;
        egress_reserved_start_bit = IN_LIF_PROFILE_EGRESS_RESERVED_START_BIT;
    }
    else
    {
        max_in_lif_egress_profile = (1 << (IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE)) - 1;
        egress_reserved_start_bit = IN_LIF_PROFILE_PMF_RESERVED_START_BIT - IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_PROFILE_TABLE, &entry_handle_id));

    for (in_lif_egress_profile = 0; in_lif_egress_profile <= max_in_lif_egress_profile; in_lif_egress_profile++)
    {

        SHR_IF_ERR_EXIT(dnx_in_lif_profile_algo_extract_egress_profile
                        (unit, in_lif_egress_profile << egress_reserved_start_bit, &egress_fields_info,
                         &in_lif_profile_jr_mode, &in_lif_profile_exclude_orientation, &in_lif_orientation, LIF));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_ORIENTATION, INST_SINGLE,
                                     egress_fields_info.in_lif_orientation);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_DP_PROFILE, INST_SINGLE,
                                     egress_fields_info.ive_inner_qos_dp_profile);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_DP_PROFILE, INST_SINGLE,
                                     egress_fields_info.ive_outer_qos_dp_profile);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE, INST_SINGLE,
                                     egress_fields_info.lif_same_interface_mode);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_MODE_IN_LIF_PROFILE, INST_SINGLE,
                                     in_lif_profile_jr_mode);

        for (pmf_reserved_bits_value = 0; pmf_reserved_bits_value < (1 << pmf_reserved_nof_bits);
             pmf_reserved_bits_value++)
        {
            for (ingress_bits_value = 0; ingress_bits_value < nof_ingress_profiles; ingress_bits_value++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE,
                                           ingress_bits_value | (in_lif_egress_profile <<
                                                                 (egress_reserved_start_bit)) |
                                           (pmf_reserved_bits_value << (IN_LIF_PROFILE_PMF_RESERVED_START_BIT)));
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
        /** If we are in JR sys hdr mode - configure the orientation table*/
        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_JR_MODE_INLIF_PROFILE_ORIENTATION_MAP, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_MODE_IN_LIF_PROFILE,
                                       in_lif_profile_jr_mode);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_IN_LIF_ORIENTATION, INST_SINGLE,
                                         egress_fields_info.in_lif_orientation);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /**Configure the egress table to receive the inlif profile*/
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_JR_MODE_INLIF_PROFILE_EGRESS_REMAP, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_MODE_SYSTEM_HEADER_RESERVED_VALUE, 0);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_IN_LIF_ORIENTATION, in_lif_orientation);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_MODE_IN_LIF_PROFILE,
                                       in_lif_profile_exclude_orientation);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                         0 | (in_lif_egress_profile << (egress_reserved_start_bit)) | (0 <<
                                                                                                       (IN_LIF_PROFILE_PMF_RESERVED_START_BIT)));
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            /** Due to the HW limitation - indexing the orientation table - DBAL_TABLE_JR_MODE_INLIF_PROFILE_ORIENTATION_MAP
             * with the resolved 2 bits of inlif profile, instead of the full 8 bits of inlif profile,
             * it's not possible to maintain situation such as: orientation = 00 or 01 and inlif profile = 00 for both cases.
             */
            if (same_interface == 0)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_MODE_SYSTEM_HEADER_RESERVED_VALUE, 0);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_IN_LIF_ORIENTATION, in_lif_orientation);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_MODE_IN_LIF_PROFILE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                             0 | (in_lif_egress_profile << (egress_reserved_start_bit)) | (0 <<
                                                                                                           (IN_LIF_PROFILE_PMF_RESERVED_START_BIT)));
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_IN_LIF_PROFILE_TABLE, entry_handle_id));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Function used to set default values to ENABLERS_VECTORS
 */
static shr_error_e
dnx_enablers_vectors_table_init(
    int unit,
    int routing_enablers_profile,
    int is_dealloc)
{

    uint32 entry_handle_id, default_enablers[2];
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * set default enablers vector value
     * By default, Eth enabler is turned on.
     */
    default_enablers[0] = INITIAL_ENABLERS_VECTOR_LSB;
    default_enablers[1] = INITIAL_ENABLERS_VECTOR_MSB;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ENABLERS_VECTORS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ROUTING_ENABLERS_PROFILE, routing_enablers_profile);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_ENABLERS_VECTOR, INST_SINGLE, default_enablers);
    if (is_dealloc)
    {
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * Function used to set default values to VSI_PROFILE_TABLE
 */
static shr_error_e
dnx_vsi_profile_table_init(
    int unit,
    int in_lif_profile,
    int is_dealloc,
    int pmf_reserved_nof_bits,
    int egress_reserved_nof_bits,
    int intf_type)
{
    int max_counter_value, hw_write_counter;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_PROFILE_TABLE, &entry_handle_id));
    /** Restore the default init configuration for all the egress allocated bits*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROUTING_ENABLERS_PROFILE, INST_SINGLE,
                                 DEFAULT_ROUTING_ENABLERS_PROFILE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE_IVL_SVL, INST_SINGLE,
                                 DEFAULT_VSI_PROFILE_IVL_SVL_MODE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE_L2_MC_FWD_TYPE, INST_SINGLE,
                                 DEFAULT_VSI_PROFILE_L2_MC_FWD_TYPE_MODE);
    if (is_dealloc)
    {
        /*
         * loop over the possible values of the reserved bits, and duplicate HW value to all the possible keys
         * the key is built by shifting the reserved bits value left and append new_in_lif_profile
         */
        max_counter_value = 1 << (pmf_reserved_nof_bits + egress_reserved_nof_bits);
        for (hw_write_counter = 0; hw_write_counter < max_counter_value; hw_write_counter++)
        {
            if (intf_type == ETH_RIF)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE,
                                           CALCULATE_VSI_PROFILE_FROM_ETH_RIF_PROFILE(VSI_MSB_PREFIX,
                                                                                      (in_lif_profile |
                                                                                       (hw_write_counter <<
                                                                                        (NOF_ETH_RIF_PROFILE_BITS -
                                                                                         (pmf_reserved_nof_bits +
                                                                                          egress_reserved_nof_bits))))));
            }
            else
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE,
                                           in_lif_profile | (hw_write_counter <<
                                                             (NOF_IN_LIF_PROFILE_BITS -
                                                              (pmf_reserved_nof_bits + egress_reserved_nof_bits))));
            }
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE, in_lif_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /*
         * routing enablers profile init
         * this table has a key of 10 bits, and can be accessed with two kinds of profiles:
         * 1. in_lif_profile : 8 bits, in this case the 2 MSBs of the key are set to 0 by ARR
         * 2. eth_rif_profile: 7 bits, in this case the 3 MSBs are set to a static prefix 111 by ARR
         * so we need to initiate the two entries 0000000000 and 1110000000 with the default values
         */
        /*
         * in_lif_profile
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE, DNX_IN_LIF_PROFILE_DEFAULT);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * eth_rif_profile
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE,
                                   CALCULATE_VSI_PROFILE_FROM_ETH_RIF_PROFILE(VSI_MSB_PREFIX,
                                                                              DNX_IN_LIF_PROFILE_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Function used to set default values to IN_LIF_PROFILE_TABLE
 */
static shr_error_e
dnx_in_lif_profile_table_init(
    int unit,
    int in_lif_profile,
    int is_dealloc,
    int pmf_reserved_nof_bits,
    int egress_reserved_nof_bits,
    int intf_type)
{
    int max_counter_value, hw_write_counter;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * IN_LIF_PROFILE_TABLE init
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_PROFILE_TABLE, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CS_IN_LIF_VTT_PROFILE, INST_SINGLE,
                                 DNX_IN_LIF_PROFILE_CS_DEFAULT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CS_IN_LIF_FWD_PROFILE, INST_SINGLE,
                                 DNX_IN_LIF_FWD_PROFILE_CS_DEFAULT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RPF_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_RPF_MODE_DISABLE);
    /*
     * trap profiling fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L_2_CP_TRAP_HANDLING_PROFILE, INST_SINGLE,
                                 DEFAULT_TRAP_PROFILES_DATA);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ICMPV4_TRAP_HANDLING_PROFILE, INST_SINGLE,
                                 DEFAULT_TRAP_PROFILES_DATA);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ICMPV6_TRAP_HANDLING_PROFILE, INST_SINGLE,
                                 DEFAULT_TRAP_PROFILES_DATA);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ARP_TRAP_HANDLING_PROFILE, INST_SINGLE,
                                 DEFAULT_TRAP_PROFILES_DATA);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGMP_TRAP_HANDLING_PROFILE, INST_SINGLE,
                                 DEFAULT_TRAP_PROFILES_DATA);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DHCP_TRAP_HANDLING_PROFILE, INST_SINGLE,
                                 DEFAULT_TRAP_PROFILES_DATA);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NON_AUTH_8021X_TRAP_HANDLING_PROFILE, INST_SINGLE,
                                 DEFAULT_TRAP_PROFILES_DATA);

    if (is_dealloc)
    {
        max_counter_value = 1 << (pmf_reserved_nof_bits + egress_reserved_nof_bits);
        /*
         * loop over the possible values of the reserved bits, and duplicate HW value to all the possible keys
         * the key is built by shifting the reserved bits value left and append in_lif_profile
         */
        for (hw_write_counter = 0; hw_write_counter < max_counter_value; hw_write_counter++)
        {
            if (intf_type == ETH_RIF)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE,
                                           CALCULATE_IN_LIF_PROFILE_FROM_ETH_RIF_PROFILE
                                           (IN_LIF_PROFILE_TABLE_MSB_PREFIX,
                                            (in_lif_profile |
                                             (hw_write_counter <<
                                              (NOF_ETH_RIF_PROFILE_BITS -
                                               (pmf_reserved_nof_bits + egress_reserved_nof_bits))))));
            }
            else
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE,
                                           in_lif_profile | (hw_write_counter <<
                                                             (NOF_IN_LIF_PROFILE_BITS -
                                                              (pmf_reserved_nof_bits + egress_reserved_nof_bits))));
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, in_lif_profile);
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
    else
    {
        /*
         * in_lif_orientation
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_ORIENTATION, INST_SINGLE,
                                     DEFAULT_IN_LIF_ORIENTATION);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_SAME_INTERFACE, INST_SINGLE,
                                    DBAL_ENUM_FVAL_IN_LIF_SAME_INTERFACE_DEVICE_SCOPE);
        /*
         * ive outer/inner dp profile
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_DP_PROFILE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_DP_PROFILE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_MODE_IN_LIF_PROFILE, INST_SINGLE,
                                     DNX_IN_LIF_PROFILE_DEFAULT);

        /*
         * this table has a key of 8 bits, and can be accessed with two kinds of profiles:
         * 1. in_lif_profile : 8 bits, in this case the key = all these 8 bits
         * 2. eth_rif_profile: 7 bits, in this case the MSB bit is set to a static prefix "1" by PEMLA
         * so we need to initiate the two entries 00000000 and 10000000
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, DNX_IN_LIF_PROFILE_DEFAULT);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE,
                                   CALCULATE_IN_LIF_PROFILE_FROM_ETH_RIF_PROFILE
                                   (IN_LIF_PROFILE_MSB_BITS_MASK_FOR_ETH_RIF_PROFILE, DNX_IN_LIF_PROFILE_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_JR_MODE_INLIF_PROFILE_ORIENTATION_MAP, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_MODE_IN_LIF_PROFILE,
                                   DNX_IN_LIF_PROFILE_DEFAULT);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_IN_LIF_ORIENTATION, INST_SINGLE,
                                     DEFAULT_IN_LIF_ORIENTATION);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   in_lif_profile.h file
 */
shr_error_e
dnx_in_lif_profile_module_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_table_init(unit, DNX_IN_LIF_PROFILE_DEFAULT, FALSE, 0, 0, 0));
    SHR_IF_ERR_EXIT(dnx_vsi_profile_table_init(unit, DNX_IN_LIF_PROFILE_DEFAULT, FALSE, 0, 0, 0));
    SHR_IF_ERR_EXIT(dnx_enablers_vectors_table_init(unit, DEFAULT_ROUTING_ENABLERS_PROFILE, FALSE));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_set_egress_hw(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   in_lif_profile.h file
 */
shr_error_e
dnx_in_lif_profile_module_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * The respective DeInit functions need to be put here.
     */
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get in-lif-profile template data accoring to interface type.
 *
 * \param [in] unit - The unit ID
 * \param [in] in_lif_profile - in_lif_profile id to get template data for
 * \param [in] intf_type - can be one of the following:
 *                         ETH_RIF - interface type is ETH_RIF
 *                         LIF - interface type is LIF
 * \param [out] in_lif_profile_template_data - in-lif-profile template data.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   The template data may be got from DNX_ALGO_ETH_RIF_PROFILE,
 *   DNX_ALGO_ETH_RIF_PROFILE or DNX_ALGO_IN_LIF_PROFILE according
 *   to the in_lif_profile id and intf_type.
 *
 * \see
 *   dnx_in_lif_profile_exchange
 *   dnx_algo_in_lif_profile_template_create
 *   dnx_algo_in_lif_profile_and_eth_rif_shared_template_create
 */
static shr_error_e
dnx_in_lif_profile_template_data_get(
    int unit,
    int in_lif_profile,
    in_lif_intf_type_e intf_type,
    in_lif_template_data_t * in_lif_profile_template_data)
{
    int ref_count;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(in_lif_profile_template_data, _SHR_E_PARAM, "NULL input - in_lif_profile_template_data");

    sal_memset(in_lif_profile_template_data, 0, sizeof(in_lif_template_data_t));

    /*
     * check if in_lif_profile is in range
     */
    in_lif_profile = in_lif_profile & ETH_RIF_PROFILE_USED_BITS_MASK;
    if ((in_lif_profile < 0) | (in_lif_profile >= dnx_data_lif.in_lif.nof_in_lif_profiles_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "in_lif_profile out of range : %d", in_lif_profile);
    }

    /*
     * determine the template manager to look in according to intf_type
     */
    if (intf_type == ETH_RIF)
    {
        /*
         * the template manager includes only the ingress attributes, so only the relevant part of the in_lif_profile is used as key for the template manager
         */
        in_lif_profile = in_lif_profile & ETH_RIF_PROFILE_USED_BITS_MASK;
        SHR_IF_ERR_EXIT(algo_lif_profile_db.eth_rif_profile.profile_data_get
                        (unit, _SHR_CORE_ALL, in_lif_profile, &ref_count, in_lif_profile_template_data));
    }
    else if (intf_type == LIF)
    {
        /*
         * the template manager includes only the ingress attributes, so only the relevant part of the in_lif_profile is used as key for the template manager
         */
        in_lif_profile = in_lif_profile & IN_LIF_PROFILE_USED_BITS_MASK;
        SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_profile.profile_data_get
                        (unit, _SHR_CORE_ALL, in_lif_profile, &ref_count, in_lif_profile_template_data));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "invalid intf_type\n");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Helper function for dnx_in_lif_profile_routing_enablers_vector_convert.
 *   disables the proper bit in routing_enablers_vector if the corresponding flag BCM_L3_INGRESS_ROUTE_DISABLE_XXX is given by the user.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     The unit number.
 *   \param [in] routing_enablers_vector -
 *     Pointer to routing_enablers_vector \n
 *     \b As \b output - \n
 *       the Enablers Vector to modify: 2x32 bits array
 *   \param [in] in_lif_profile_info -
 *     Pointer to in_lif_profile_info \n
 *     \b As \b input - \n
 *       includes the lif profile properties, only flags is used
 *   \param [in] flag -
 *     The flag which determines the enabler to disable.
 *   \param [in] enabler_offset -
 *     The enabler's offset in the enablers vector.
 * \par INDIRECT INPUT
 *   * \b *in_lif_profile_info \n
 *     See 'in_lif_profile_info' in DIRECT INPUT above
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * \b *routing_enablers_vector \n
 *     See 'routing_enablers_vector' in DIRECT INPUT above
 * \see
 *   * None
 */
static shr_error_e
dnx_in_lif_profile_routing_enablers_vector_assign(
    int unit,
    uint32 *routing_enablers_vector,
    in_lif_profile_info_t * in_lif_profile_info,
    int flag,
    int enabler_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(in_lif_profile_info, _SHR_E_PARAM, "NULL input - in_lif_profile_info");

    /*
     * check if the flag is set by the user
     */
    if (in_lif_profile_info->ingress_fields.l3_ingress_flags & flag)
    {
        /*
         * in case the enabler's offset is lower than 32 we need to modify the
         * LSB 32 bit out of the 64 bit enablers vector
         * else we need to modify the MSB 32 bits
         */
        if (enabler_offset < 32)
        {
            routing_enablers_vector[0] &= ~SAL_BIT(enabler_offset);
        }
        else
        {
            routing_enablers_vector[1] &= ~(SAL_BIT(enabler_offset - 32));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Helper for dnx_in_lif_profile_get_data.
 *   translates in_lif_profile_template_data to cs_in_lif_profile_flags
 * \param [in] unit -
 *     The unit number.
 * \param [in] in_lif_profile_template_data -
 *   The in_lif_profile Template Manager data
 * \param [out] cs_in_lif_profile_flags
 *   cs_in_lif_profile_flags value according to the given in_lif_profile_template_data
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   * None
 */
static shr_error_e
dnx_in_lif_profile_translate_to_cs_in_lif_profile_flags(
    int unit,
    in_lif_template_data_t in_lif_profile_template_data,
    uint32 *cs_in_lif_profile_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cs_in_lif_profile_flags, _SHR_E_PARAM, "NULL input - cs_in_lif_profile_flags");

    *cs_in_lif_profile_flags = 0;

    if (in_lif_profile_template_data.lif_type == PWE)
    {
        if ((in_lif_profile_template_data.cs_in_lif_profile & 0x1) ==
            DBAL_ENUM_FVAL_LIF_VTT4_CSP_1BIT_PWE_TAG_NAMESPACE_LIF_SCOPED)
        {
            *cs_in_lif_profile_flags |= DNX_IN_LIF_PROFILE_CS_PROFILE_PWE_LIF_SCOPED;
        }
    }
    if (in_lif_profile_template_data.lif_type == GRE8)
    {
        switch ((in_lif_profile_template_data.cs_in_lif_profile >> 3) & 0x1)
        {
            case DBAL_ENUM_FVAL_LIF_VTTX_CSP_1BIT_IPVX_GRE8_TNI_MODE_TNI_INDICATES_FORWARD_DOMAIN:
                *cs_in_lif_profile_flags |= DNX_IN_LIF_PROFILE_CS_PROFILE_GRE8_USE_TNI;
                break;
            case DBAL_ENUM_FVAL_LIF_VTTX_CSP_1BIT_IPVX_GRE8_TNI_MODE_NO_TNI:
                *cs_in_lif_profile_flags |= DNX_IN_LIF_PROFILE_CS_PROFILE_GRE8_USE_TNI;
                break;
            default:
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Helper for dnx_in_lif_profile_get_data.
 *   translates in_lif_profile_template_data to l3_ingress_flags
 * \param [in] unit -
 *   The unit number.
 * \param [in] in_lif_profile_template_data -
 *   The in_lif_profile Template Manager data
 * \param [out] l3_ingress_flags
 *   l3_ingress_flags value according to the given in_lif_profile_template_data
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   * None
 */
static shr_error_e
dnx_in_lif_profile_translate_to_l3_ingress_flags(
    int unit,
    in_lif_template_data_t in_lif_profile_template_data,
    uint32 *l3_ingress_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(l3_ingress_flags, _SHR_E_PARAM, "NULL input - l3_ingress_flags");

    *l3_ingress_flags = 0;
    if (in_lif_profile_template_data.lif_type == LSP)
    {
        if ((in_lif_profile_template_data.cs_in_lif_profile & 0x1) ==
            (DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE_UC_PRIVATE_PUBLIC_OPTIMIZED))
        {
            *l3_ingress_flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;
        }

        if (((in_lif_profile_template_data.cs_in_lif_profile & 0x4) >> 2) ==
            (DBAL_ENUM_FVAL_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_PER_INTERFACE_NAMESPACE))
        {
            *l3_ingress_flags |= BCM_L3_INGRESS_MPLS_INTF_NAMESPACE;
        }
    }

    if (in_lif_profile_template_data.lif_type == RIF)
    {
        if ((in_lif_profile_template_data.cs_in_lif_profile & 0x1) ==
            (DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE_UC_PRIVATE_PUBLIC_OPTIMIZED))
        {
            *l3_ingress_flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;
        }
        if (in_lif_profile_template_data.cs_in_lif_profile &
            (DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE <<
             IN_LIF_PROFILE_IPMC_BRIDGE_FALLBACK_BIT_OFFSET))
        {
            *l3_ingress_flags |= BCM_L3_INGRESS_IPMC_BRIDGE_FALLBACK;
        }
        if (((in_lif_profile_template_data.cs_in_lif_profile & 0x4) >> 2) ==
            (DBAL_ENUM_FVAL_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_PER_INTERFACE_NAMESPACE))
        {
            *l3_ingress_flags |= BCM_L3_INGRESS_MPLS_INTF_NAMESPACE;
        }
        if (in_lif_profile_template_data.l2_mc_fwd_type == DBAL_ENUM_FVAL_VSI_PROFILE_L2_MC_FWD_TYPE_IPV4MC)
        {
            *l3_ingress_flags |= BCM_L3_INGRESS_L3_MCAST_L2;
        }
    }

    if (in_lif_profile_template_data.lif_type == GRE8)
    {
        if ((in_lif_profile_template_data.cs_in_lif_profile & 0x1) ==
            DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE_UC_PRIVATE_PUBLIC_OPTIMIZED)
        {
            *l3_ingress_flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;
        }

        if (((in_lif_profile_template_data.cs_in_lif_profile & 0x4) >> 2) ==
            (DBAL_ENUM_FVAL_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_PER_INTERFACE_NAMESPACE))
        {
            *l3_ingress_flags |= BCM_L3_INGRESS_MPLS_INTF_NAMESPACE;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Helper for dnx_in_lif_profile_algo_handle_routing_enablers_profile.
 *   Converts BCM_L3_INGRESS_ROUTE_DISABLE_XXX to in_lif_profile template data.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     The unit number.
 *   \param [in] routing_enablers_vector -
 *     Pointer to routing_enablers_vector \n
 *     \b As \b output - \n
 *       the Enablers Vector to modify: 2x32 bits array
 *   \param [in] in_lif_profile_info -
 *     Pointer to in_lif_profile_info \n
 *     \b As \b input - \n
 *       includes the lif profile properties, only flags is used
 * \par INDIRECT INPUT
 *   * \b *in_lif_profile_info \n
 *     See 'in_lif_profile_info' in DIRECT INPUT above
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * \b *routing_enablers_vector \n
 *     See 'routing_enablers_vector' in DIRECT INPUT above
 * \see
 *   * None
 */
static shr_error_e
dnx_in_lif_profile_routing_enablers_vector_convert(
    int unit,
    uint32 *routing_enablers_vector,
    in_lif_profile_info_t * in_lif_profile_info)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(in_lif_profile_info, _SHR_E_PARAM, "NULL input - in_lif_profile_info");

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_routing_enablers_vector_assign
                    (unit, routing_enablers_vector, in_lif_profile_info, BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST,
                     IPV4_ENABLER_OFFSET));

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_routing_enablers_vector_assign
                    (unit, routing_enablers_vector, in_lif_profile_info, BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST,
                     IPV4_MC_ENABLER_OFFSET));

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_routing_enablers_vector_assign
                    (unit, routing_enablers_vector, in_lif_profile_info, BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST,
                     IPV6_ENABLER_OFFSET));

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_routing_enablers_vector_assign
                    (unit, routing_enablers_vector, in_lif_profile_info, BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST,
                     IPV6_MC_ENABLER_OFFSET));

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_routing_enablers_vector_assign
                    (unit, routing_enablers_vector, in_lif_profile_info, BCM_L3_INGRESS_ROUTE_DISABLE_MPLS,
                     MPLS_ENABLER_OFFSET));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   handles routing enablers profile by using template manager from algo,
 *   returns the new profile number
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     The unit number.
 *   \param [in] old_in_lif_profile -
 *     old LIF profile which should be exchanged
 *   \param [in] routing_enablers_profile -
 *     Pointer to routing_enablers_profile \n
 *     \b As \b input - \n
 *       the new enablers vector according to user's flags
 *   \param [in] in_lif_profile_info -
 *     Pointer to in_lif_profile_info \n
 *     \b As \b input - \n
 *       includes the lif profile properties, only flags is used
 *   \param [in] is_first -
 *     Pointer to is_first \n
 *     \b As \b output - \n
 *       says if the new profile was added to the template
 *       for the first time
 *   \param [in] is_last -
 *     Pointer to is_last \n
 *     \b As \b output - \n
 *       says if the old profile has no more pointers in template manager
 * \param [in] intf_type -
 *   can be one of the following:
 *       ETH_RIF - interface type is ETH_RIF
 *       LIF - interface type is LIF
 * \par INDIRECT INPUT
 *   * \b *routing_enablers_profile \n
 *     See 'routing_enablers_profile' in DIRECT INPUT above
 *   * \b *in_lif_profile_info \n
 *     See 'in_lif_profile_info' in DIRECT INPUT above
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * \b *is_first \n
 *     See 'is_first' in DIRECT INPUT above
 *   * \b *is_last \n
 *     See 'is_last' in DIRECT INPUT above
 * \remark
 *   we don't clean HW tables in case of last profile case, because of the
 *   issue that when we delete the profile till we move the enablers profile
 *   to the new place traffic will get some "undefine" state.
 * \see
 *   * None
 */
static shr_error_e
dnx_in_lif_profile_algo_handle_routing_enablers_profile(
    int unit,
    int old_in_lif_profile,
    int *routing_enablers_profile,
    in_lif_profile_info_t * in_lif_profile_info,
    uint8 *is_first,
    uint8 *is_last,
    in_lif_intf_type_e intf_type)
{
    uint32 routing_enablers_vector[2], entry_handle_id, old_routing_enablers_profile;
    int ref_count;
    in_lif_template_data_t old_in_lif_profile_data;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * init local vars
     */
    ref_count = 0;
    old_routing_enablers_profile = DEFAULT_ROUTING_ENABLERS_PROFILE;
    /*
     * init to default, later we will disable enablers according to input ingress flags
     */
    routing_enablers_vector[0] = ENABLERS_VECTOR_DEFAULT_0;
    routing_enablers_vector[1] = ENABLERS_VECTOR_DEFAULT_1;

    /*
     * verify input
     */
    old_in_lif_profile = old_in_lif_profile & ETH_RIF_PROFILE_USED_BITS_MASK;
    if ((old_in_lif_profile < 0) || (old_in_lif_profile > dnx_data_lif.in_lif.nof_in_lif_profiles_get(unit) - 1))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "old_in_lif_profile out of range: %d, must be between 0 and %d\n",
                     old_in_lif_profile, dnx_data_lif.in_lif.nof_in_lif_profiles_get(unit) - 1);
    }
    SHR_NULL_CHECK(in_lif_profile_info, _SHR_E_FAIL, "in_lif_profile_info");

    /*
     * disable the proper bits in enablers vector according to user's flags
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_routing_enablers_vector_convert
                    (unit, routing_enablers_vector, in_lif_profile_info));

    /*
     * get old enablers profile
     */
    if (intf_type == LIF)
    {
        /*
         * the template manager key is only the ingress part of the in_lif_profile
         */
        old_in_lif_profile = old_in_lif_profile & IN_LIF_PROFILE_USED_BITS_MASK;
        SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_profile.profile_data_get
                        (unit, _SHR_CORE_ALL, old_in_lif_profile, &ref_count, &old_in_lif_profile_data));
    }
    else if (intf_type == ETH_RIF)
    {
        /*
         * the template manager key is only the ingress part of the in_lif_profile
         */
        old_in_lif_profile = old_in_lif_profile & ETH_RIF_PROFILE_USED_BITS_MASK;
        SHR_IF_ERR_EXIT(algo_lif_profile_db.eth_rif_profile.profile_data_get
                        (unit, _SHR_CORE_ALL, old_in_lif_profile, &ref_count, &old_in_lif_profile_data));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "invalid intf_type\n");
    }
    old_routing_enablers_profile = old_in_lif_profile_data.routing_enablers_profile;

    SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_profile_l3_enablers.exchange
                    (unit, _SHR_CORE_ALL, 0, routing_enablers_vector, old_routing_enablers_profile,
                     NULL, routing_enablers_profile, is_first, is_last));

    /*
     * in case the new profile is used for the first time, need to write it to HW
     */
    if (*is_first)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ENABLERS_VECTORS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id,
                                   DBAL_FIELD_ROUTING_ENABLERS_PROFILE, *routing_enablers_profile);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id,
                                         DBAL_FIELD_ENABLERS_VECTOR, INST_SINGLE, routing_enablers_vector);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   calculates cs_in_lif_profile_value according to in_lif_profile properties
 * \param [in] unit -
 *   The unit number.
 * \param [in] intf_type -
 *   can be ETH_RIF or LIF
 * \param [in] inlif_dbal_table_id -
 *   used to retrieve information about dbal table of the LIF
 * \param [in] in_lif_profile_info -
 *   Pointer to in_lif_profile_info
 *   includes the lif profile properties
 * \param [out] cs_in_lif_profile_value
 *   the concluded cs_in_lif_profile value
 * \param [out] cs_in_lif_fwd_profile_value
 *   the concluded cs_in_lif_fwd_profile value
 * \param [out] lif_type
 *   the concluded lif_type value
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   * None
 */
static shr_error_e
dnx_in_lif_profile_algo_convert_to_cs_in_lif_profile(
    int unit,
    in_lif_intf_type_e intf_type,
    dbal_tables_e inlif_dbal_table_id,
    in_lif_profile_info_t * in_lif_profile_info,
    uint8 *cs_in_lif_profile_value,
    uint8 *cs_in_lif_fwd_profile_value,
    in_lif_lif_type_e * lif_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if (inlif_dbal_table_id == DBAL_TABLE_IN_LIF_FORMAT_PWE)
    {
        /*
         * LIF type is PWE, only DNX_IN_LIF_PROFILE_CS_PROFILE_PWE_LIF_SCOPED and
         * in_lif_profile_info_t.pwe_nof_eth_tags are relevant
         */
        if ((in_lif_profile_info->ingress_fields.cs_in_lif_profile_flags & DNX_IN_LIF_PROFILE_CS_PROFILE_GRE8_USE_TNI)
            || (in_lif_profile_info->ingress_fields.l3_ingress_flags & BCM_L3_INGRESS_GLOBAL_ROUTE)
            || (in_lif_profile_info->ingress_fields.l3_ingress_flags & BCM_L3_INGRESS_MPLS_INTF_NAMESPACE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "invalid fields or flags for PWE LIF\n");
        }

        /*
         * First set the number of SD tags on lif
         */
        *lif_type = PWE;
        *cs_in_lif_profile_value = (DBAL_ENUM_FVAL_LIF_VTT4_CSP_2BIT_PWE_NOF_SD_TAGS_0_SD_TAGS +
                                    in_lif_profile_info->ingress_fields.pwe_nof_eth_tags) << 1;
        /*
         * Second, set the context scope (LiF/Network) scoped
         */
        if (in_lif_profile_info->ingress_fields.cs_in_lif_profile_flags & DNX_IN_LIF_PROFILE_CS_PROFILE_PWE_LIF_SCOPED)
        {
            *cs_in_lif_profile_value |= (DBAL_ENUM_FVAL_LIF_VTT4_CSP_1BIT_PWE_TAG_NAMESPACE_LIF_SCOPED & 0x1);
        }
        else
        {
            *cs_in_lif_profile_value |= (DBAL_ENUM_FVAL_LIF_VTT4_CSP_1BIT_PWE_TAG_NAMESPACE_NWK_SCOPED & 0x1);
        }
    }
    else if (inlif_dbal_table_id == DBAL_TABLE_IN_LIF_IPvX_TUNNELS)
    {
        /*
         * LIF type is GRE8, only DNX_IN_LIF_PROFILE_CS_PROFILE_GRE8_USE_TNI
         * and BCM_L3_INGRESS_GLOBAL_ROUTE are relevant
         */
        if ((in_lif_profile_info->ingress_fields.cs_in_lif_profile_flags & DNX_IN_LIF_PROFILE_CS_PROFILE_PWE_LIF_SCOPED)
            || (in_lif_profile_info->ingress_fields.pwe_nof_eth_tags > ZERO_TAGS))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "invalid fields or flags for GRE8 LIF\n");
        }
        *lif_type = GRE8;
        /*
         * coverity explanation: coverity has found that we could have copy paste error here and says we should use
         * cs_in_lif_profile_flags instead of l3_ingress_flags but BCM_L3_INGRESS_GLOBAL_ROUTE should be assigned in
         * l3_ingress_flags
         */
        /** coverity[copy_paster_error:FALSE] */
        if (in_lif_profile_info->ingress_fields.l3_ingress_flags & BCM_L3_INGRESS_GLOBAL_ROUTE)
        {
            *cs_in_lif_profile_value =
                DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE_UC_PRIVATE_PUBLIC_OPTIMIZED & 0x1;
            *cs_in_lif_profile_value |=
                ((DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_MC_DB_MODE_MC_PRIVATE_PUBLIC_OPTIMIZED & 0x1) << 1);
        }
        else
        {
            *cs_in_lif_profile_value = (DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE_UC_PRIVATE_ONLY & 0x1);
            *cs_in_lif_profile_value |=
                ((DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_MC_DB_MODE_MC_PRIVATE_ONLY & 0x1) << 1);
        }
        if (in_lif_profile_info->ingress_fields.cs_in_lif_profile_flags & DNX_IN_LIF_PROFILE_CS_PROFILE_GRE8_USE_TNI)
        {
            *cs_in_lif_profile_value |=
                ((DBAL_ENUM_FVAL_LIF_VTTX_CSP_1BIT_IPVX_GRE8_TNI_MODE_TNI_INDICATES_FORWARD_DOMAIN & 0x1) << 3);
        }
        else
        {
            *cs_in_lif_profile_value |= ((DBAL_ENUM_FVAL_LIF_VTTX_CSP_1BIT_IPVX_GRE8_TNI_MODE_NO_TNI & 0x1) << 3);
        }

        if (in_lif_profile_info->ingress_fields.l3_ingress_flags & BCM_L3_INGRESS_MPLS_INTF_NAMESPACE)
        {
            *cs_in_lif_profile_value |=
                ((DBAL_ENUM_FVAL_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_PER_INTERFACE_NAMESPACE & 0x1) << 2);
        }
        else
        {
            *cs_in_lif_profile_value |=
                ((DBAL_ENUM_FVAL_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_PER_PLATFORM_NAMESPACE & 0x1) << 2);
        }
    }
    else if (inlif_dbal_table_id == DBAL_TABLE_IN_LIF_FORMAT_LSP)
    {
        /*
         * LIF type is LSP, only BCM_L3_INGRESS_MPLS_INTF_NAMESPACE and BCM_L3_INGRESS_GLOBAL_ROUTE
         * are relevant
         */
        if ((in_lif_profile_info->ingress_fields.cs_in_lif_profile_flags & DNX_IN_LIF_PROFILE_CS_PROFILE_GRE8_USE_TNI)
            || (in_lif_profile_info->ingress_fields.pwe_nof_eth_tags > ZERO_TAGS)
            || (in_lif_profile_info->
                ingress_fields.cs_in_lif_profile_flags & DNX_IN_LIF_PROFILE_CS_PROFILE_PWE_LIF_SCOPED))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "invalid fields or flags for PWE LIF\n");
        }
        *lif_type = LSP;
        if (in_lif_profile_info->ingress_fields.l3_ingress_flags & BCM_L3_INGRESS_GLOBAL_ROUTE)
        {
            *cs_in_lif_profile_value =
                (DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE_UC_PRIVATE_PUBLIC_OPTIMIZED & 0x1);
            *cs_in_lif_profile_value |=
                ((DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_MC_DB_MODE_MC_PRIVATE_PUBLIC_OPTIMIZED & 0x1) << 1);
        }
        else
        {
            *cs_in_lif_profile_value = (DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE_UC_PRIVATE_ONLY & 0x1);
            *cs_in_lif_profile_value |=
                ((DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_MC_DB_MODE_MC_PRIVATE_ONLY & 0x1) << 1);
        }

        if (in_lif_profile_info->ingress_fields.l3_ingress_flags & BCM_L3_INGRESS_MPLS_INTF_NAMESPACE)
        {
            *cs_in_lif_profile_value |=
                ((DBAL_ENUM_FVAL_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_PER_INTERFACE_NAMESPACE & 0x1) << 2);
        }
        else
        {
            *cs_in_lif_profile_value |=
                ((DBAL_ENUM_FVAL_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_PER_PLATFORM_NAMESPACE & 0x1) << 2);
        }
    }
    else if (inlif_dbal_table_id == DBAL_TABLE_ING_VSI_INFO_DB)
    {
        /*
         * LIF type is RIF. only BCM_L3_INGRESS_GLOBAL_ROUTE and TBD(IPMC_BF) are relevant
         */
        if ((in_lif_profile_info->ingress_fields.cs_in_lif_profile_flags & DNX_IN_LIF_PROFILE_CS_PROFILE_GRE8_USE_TNI)
            || (in_lif_profile_info->ingress_fields.pwe_nof_eth_tags > ZERO_TAGS)
            || (in_lif_profile_info->ingress_fields.cs_in_lif_profile_flags & BCM_L3_INGRESS_MPLS_INTF_NAMESPACE)
            || (in_lif_profile_info->
                ingress_fields.cs_in_lif_profile_flags & DNX_IN_LIF_PROFILE_CS_PROFILE_PWE_LIF_SCOPED))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "invalid fields or flags for RIF\n");
        }
        *lif_type = RIF;
        if (in_lif_profile_info->ingress_fields.l3_ingress_flags & BCM_L3_INGRESS_GLOBAL_ROUTE)
        {
            *cs_in_lif_profile_value =
                (DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE_UC_PRIVATE_PUBLIC_OPTIMIZED & 0x1);
            *cs_in_lif_profile_value |=
                ((DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_MC_DB_MODE_MC_PRIVATE_PUBLIC_OPTIMIZED & 0x1) << 1);
        }
        else
        {
            *cs_in_lif_profile_value = (DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE_UC_PRIVATE_ONLY & 0x1);
            *cs_in_lif_profile_value |=
                ((DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_MC_DB_MODE_MC_PRIVATE_ONLY & 0x1) << 1);
        }

        if (in_lif_profile_info->ingress_fields.l3_ingress_flags & BCM_L3_INGRESS_MPLS_INTF_NAMESPACE)
        {
            *cs_in_lif_profile_value |=
                ((DBAL_ENUM_FVAL_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_PER_INTERFACE_NAMESPACE & 0x1) << 2);
        }
        else
        {
            *cs_in_lif_profile_value |=
                ((DBAL_ENUM_FVAL_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_PER_PLATFORM_NAMESPACE & 0x1) << 2);
        }

        if (in_lif_profile_info->ingress_fields.l3_ingress_flags & BCM_L3_INGRESS_IPMC_BRIDGE_FALLBACK)
        {
            *cs_in_lif_fwd_profile_value |=
                (DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE
                 << IN_LIF_PROFILE_IPMC_BRIDGE_FALLBACK_BIT_OFFSET);
        }
        else
        {
            *cs_in_lif_fwd_profile_value |=
                (DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_DISABLE
                 << IN_LIF_PROFILE_IPMC_BRIDGE_FALLBACK_BIT_OFFSET);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   calculates RPF mode value, returns RPF mode
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     The unit number.
 *   \param [in] rpf_mode -
 *     Pointer to rpf_mode \n
 *     \b As \b output - \n
 *       the rpf_mode to return
 *   \param [in] in_lif_profile_info -
 *     Pointer to in_lif_profile_info \n
 *     \b As \b input - \n
 *       includes the lif profile properties, only flags is used
 * \par INDIRECT INPUT
 *   * \b *in_lif_profile_info \n
 *     See 'in_lif_profile_info' in DIRECT INPUT above
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * \b *rpf_mode \n
 *     See 'rpf_mode' in DIRECT INPUT above
 * \see
 *   * None
 */
static shr_error_e
dnx_in_lif_profile_algo_convert_to_rpf_mode(
    int unit,
    uint8 *rpf_mode,
    in_lif_profile_info_t * in_lif_profile_info)
{

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * handle RPF_MODE
     */
    switch (in_lif_profile_info->ingress_fields.urpf_mode)
    {
        case bcmL3IngressUrpfLoose:
            *rpf_mode = DBAL_ENUM_FVAL_RPF_MODE_LOOSE;
            break;
        case bcmL3IngressUrpfStrict:
            *rpf_mode = DBAL_ENUM_FVAL_RPF_MODE_STRICT;
            break;
        case bcmL3IngressUrpfDisable:
            *rpf_mode = DBAL_ENUM_FVAL_RPF_MODE_DISABLE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "urpf_mode invalid : %d\n", in_lif_profile_info->ingress_fields.urpf_mode);
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Allocate a da_not_found_dst_profile per LIF according to the default
 *  destinations for unknown packets.
 *
 * \param [in] unit - The unit ID
 * \param [in] old_in_lif_profile - old in_lif_profile the old
 *             da_not_found_dst_profile residing in.
 * \param [in] intf_type - can be one of the following:
 *             ETH_RIF - interface type is ETH_RIF
 *             LIF - interface type is LIF
 * \param [in] default_frwrd_dst - Default destinations for unknown packets.
 * \param [out] da_not_found_dst_profile - da_not_found_dst_profile per LIF.
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   The default destination may be a set of offset to VSI.default_frwrd_destionation,
 *   or a set of destinations for unknown UC, MC and BC packets.
 *
 * \see
 *   dnx_in_lif_profile_exchange.
 */
static shr_error_e
dnx_in_lif_profile_algo_convert_lif_flood_profile(
    int unit,
    int old_in_lif_profile,
    in_lif_intf_type_e intf_type,
    dnx_default_frwrd_dst_t default_frwrd_dst[DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES],
    int *da_not_found_dst_profile)
{
    int lif_flood_profile, port_flood_profile;
    uint8 first_reference, last_reference;
    in_lif_template_data_t in_lif_profile_template_data;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(da_not_found_dst_profile, _SHR_E_PARAM, "A valid pointer for da_not_found_dst_profile is needed!");

    /*
     * Get in_lif_profile template data
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_template_data_get
                    (unit, old_in_lif_profile, intf_type, &in_lif_profile_template_data));

    /*
     * Exchange the lif_flood_profile.
     */
    lif_flood_profile = 0;
    first_reference = FALSE;
    SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_da_not_found_destination.exchange
                    (unit, _SHR_CORE_ALL, 0, default_frwrd_dst, in_lif_profile_template_data.da_not_found_dst_profile,
                     NULL, &lif_flood_profile, &first_reference, &last_reference));

    *da_not_found_dst_profile = lif_flood_profile;

    /*
     * If new lif_flood_profile is used for the first time, update its hw table.
     */
    if (first_reference)
    {
        port_flood_profile = DBAL_ENUM_FVAL_DA_NOT_FOUND_ACTION_PROFILE_NONE;
        SHR_IF_ERR_EXIT(dnx_port_lif_flood_profile_action_set
                        (unit, port_flood_profile, lif_flood_profile, default_frwrd_dst));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   in_lif_profile.h file
 */
shr_error_e
dnx_in_lif_profile_dealloc(
    int unit,
    int in_lif_profile,
    int *new_in_lif_profile,
    in_lif_intf_type_e intf_type)
{
    uint8 is_first, is_last, is_last_lif_profile;
    in_lif_template_data_t default_in_lif_profile_template_data;
    uint32 routing_enablers_vector[2], old_routing_enablers_profile;
    int ref_count = 0, routing_enablers_profile, pmf_reserved_nof_bits, egress_reserved_nof_bits, system_headers_mode;
    in_lif_template_data_t old_in_lif_profile_data;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&default_in_lif_profile_template_data, 0, sizeof(default_in_lif_profile_template_data));
    is_first = 0;
    is_last = 0;
    is_last_lif_profile = 0;

    /*
     * init to default, Eth enabler is turned on.
     */
    routing_enablers_vector[0] = INITIAL_ENABLERS_VECTOR_LSB;
    routing_enablers_vector[1] = INITIAL_ENABLERS_VECTOR_MSB;
    pmf_reserved_nof_bits = 0;
    egress_reserved_nof_bits = 0;
    old_routing_enablers_profile = 0;

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    /*
     * check if in_lif_profile is in range
     */
    in_lif_profile = in_lif_profile & ETH_RIF_PROFILE_USED_BITS_MASK;
    if ((in_lif_profile < 0) | (in_lif_profile >= dnx_data_lif.in_lif.nof_in_lif_profiles_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "in_lif_profile out of range : %d", in_lif_profile);
    }

    /*
     * there are 2 options:
     * the profile is ETH-RIF profile, in this case the old profile exists in DNX_ALGO_ETH_RIF_PROFILE
     * the profile is LIF profile, in this case the old profile exists in DNX_ALGO_IN_LIF_PROFILE
     */
    if (intf_type == ETH_RIF)
    {
        pmf_reserved_nof_bits = ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS;
        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
        {
            egress_reserved_nof_bits = ETH_RIF_PROFILE_EGRESS_RESERVED_NOF_BITS;
        }
        else
        {
            egress_reserved_nof_bits = 0;
        }
        in_lif_profile = in_lif_profile & ETH_RIF_PROFILE_USED_BITS_MASK;
        SHR_IF_ERR_EXIT(algo_lif_profile_db.eth_rif_profile.profile_data_get
                        (unit, _SHR_CORE_ALL, in_lif_profile, &ref_count, &old_in_lif_profile_data));

        SHR_IF_ERR_EXIT(algo_lif_profile_db.eth_rif_profile.exchange
                        (unit, _SHR_CORE_ALL, 0, &default_in_lif_profile_template_data,
                         in_lif_profile, NULL, new_in_lif_profile, &is_first, &is_last));
    }
    /** Free the sw state allocated profiles by exchanging the profile with the default one*/
    else if (intf_type == LIF)
    {
        pmf_reserved_nof_bits = IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS;
        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
        {
            egress_reserved_nof_bits = IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS;
        }
        else
        {
            egress_reserved_nof_bits = IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE;
        }

        in_lif_profile = in_lif_profile & IN_LIF_PROFILE_USED_BITS_MASK;
        SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_profile.profile_data_get
                        (unit, _SHR_CORE_ALL, in_lif_profile, &ref_count, &old_in_lif_profile_data));

        SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_profile.exchange
                        (unit, _SHR_CORE_ALL, 0, &default_in_lif_profile_template_data,
                         in_lif_profile, NULL, new_in_lif_profile, &is_first, &is_last_lif_profile));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "invalid intf_type\n");
    }
    /** Free the internal allocated profile by exchanging the profile with the default one*/
    if (ref_count)
    {
        old_routing_enablers_profile = old_in_lif_profile_data.routing_enablers_profile;

        SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_profile_l3_enablers.exchange
                        (unit, _SHR_CORE_ALL, 0, routing_enablers_vector, old_routing_enablers_profile,
                         NULL, &routing_enablers_profile, &is_first, &is_last));
    }

    if (is_last_lif_profile)
    {
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_table_init
                        (unit, in_lif_profile, TRUE, pmf_reserved_nof_bits, egress_reserved_nof_bits, intf_type));
        SHR_IF_ERR_EXIT(dnx_vsi_profile_table_init
                        (unit, in_lif_profile, TRUE, pmf_reserved_nof_bits, egress_reserved_nof_bits, intf_type));
        SHR_IF_ERR_EXIT(dnx_enablers_vectors_table_init(unit, old_routing_enablers_profile, TRUE));
        *new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   in_lif_profile.h file
 */
shr_error_e
dnx_in_lif_profile_exchange(
    int unit,
    in_lif_profile_info_t * in_lif_profile_info,
    int old_in_lif_profile,
    int *new_in_lif_profile,
    in_lif_intf_type_e intf_type,
    dbal_tables_e inlif_dbal_table_id)
{
    in_lif_template_data_t in_lif_profile_template_data, default_in_lif_profile_template_data;
    uint32 entry_handle_id;
    int routing_enablers_profile, inner_old_in_lif_profile, shared_in_lif_profile,
        update_in_lif_profile, inner_in_lif_profile, da_not_found_dst_profile;
    int egress_in_lif_profile, hw_write_counter, pmf_reserved_nof_bits, egress_reserved_nof_bits,
        nof_ingress_profiles, max_counter_value, pmf_reserved_start_bit, pmf_in_lif_profile, pmf_bits_mask,
        system_headers_mode, in_lif_profile_egress_reserved_start_bit;
    uint8 cs_in_lif_profile_value, rpf_mode, is_first, is_last, routing_profile_is_first, routing_profile_is_last,
        cs_in_lif_fwd_profile_value;
    in_lif_lif_type_e lif_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * local variables init
     */
    nof_ingress_profiles = 0;
    pmf_reserved_nof_bits = 0;
    egress_reserved_nof_bits = 0;
    max_counter_value = 0;
    inner_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    da_not_found_dst_profile = 0;
    is_first = 0;
    is_last = 0;
    routing_profile_is_first = 0;
    routing_profile_is_last = 0;
    rpf_mode = 0;
    inner_old_in_lif_profile = 0;
    shared_in_lif_profile = 0;
    update_in_lif_profile = 0;
    sal_memset(&default_in_lif_profile_template_data, 0, sizeof(default_in_lif_profile_template_data));
    cs_in_lif_profile_value = 0;
    cs_in_lif_fwd_profile_value = 0;
    hw_write_counter = 0;
    egress_in_lif_profile = 0;
    pmf_reserved_start_bit = 0;
    pmf_in_lif_profile = 0;
    pmf_bits_mask = 0;
    lif_type = RIF;
    in_lif_profile_egress_reserved_start_bit = 0;

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    /*
     * handle routing enablers profile
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_algo_handle_routing_enablers_profile
                    (unit, old_in_lif_profile, &routing_enablers_profile, in_lif_profile_info,
                     &routing_profile_is_first, &routing_profile_is_last, intf_type));
    /*
     * handle RPF
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_algo_convert_to_rpf_mode(unit, &rpf_mode, in_lif_profile_info));

    /*
     * handle cs_in_lif_profile
     */
    dnx_in_lif_profile_algo_convert_to_cs_in_lif_profile(unit, intf_type, inlif_dbal_table_id, in_lif_profile_info,
                                                         &cs_in_lif_profile_value, &cs_in_lif_fwd_profile_value,
                                                         &lif_type);

    /*
     * handle da_not_found_dst_profile
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_algo_convert_lif_flood_profile
                    (unit, old_in_lif_profile, intf_type, in_lif_profile_info->ingress_fields.default_frwrd_dst,
                     &da_not_found_dst_profile));

    /*
     * handle the in_lif template (outer template)
     */
    sal_memset(&in_lif_profile_template_data, 0, sizeof(in_lif_profile_template_data));
    in_lif_profile_template_data.routing_enablers_profile = routing_enablers_profile;
    in_lif_profile_template_data.rpf_mode = rpf_mode;
    in_lif_profile_template_data.cs_in_lif_profile = cs_in_lif_profile_value;
    in_lif_profile_template_data.cs_in_lif_fwd_profile = cs_in_lif_fwd_profile_value;
    in_lif_profile_template_data.lif_type = lif_type;
    in_lif_profile_template_data.da_not_found_dst_profile = da_not_found_dst_profile;
    in_lif_profile_template_data.protocol_trap_profiles = in_lif_profile_info->ingress_fields.protocol_trap_profiles;
    if (intf_type == ETH_RIF)
    {
        in_lif_profile_template_data.mact_mode = in_lif_profile_info->ingress_fields.mact_mode;
        in_lif_profile_template_data.l2_mc_fwd_type = in_lif_profile_info->ingress_fields.l2_mc_fwd_type;
    }

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_algo_convert_to_egress_profile
                    (unit, in_lif_profile_info->egress_fields, old_in_lif_profile, &egress_in_lif_profile));

    /*
     * handle the in_lif template (outer template)
     */
    if (intf_type == ETH_RIF)
    {
        SHR_IF_ERR_EXIT(algo_lif_profile_db.eth_rif_profile.exchange
                        (unit, _SHR_CORE_ALL, 0, &in_lif_profile_template_data,
                         old_in_lif_profile & ETH_RIF_PROFILE_USED_BITS_MASK, NULL, new_in_lif_profile, &is_first,
                         &is_last));
    }
    else if (intf_type == LIF)
    {
        SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_profile.exchange
                        (unit, _SHR_CORE_ALL, 0, &in_lif_profile_template_data,
                         old_in_lif_profile & IN_LIF_PROFILE_USED_BITS_MASK, NULL, new_in_lif_profile, &is_first,
                         &is_last));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "invalid intf_type\n");
    }

    /*
     * write to HW
     */
    if (intf_type == ETH_RIF)
    {
        pmf_reserved_nof_bits = ETH_RIF_PROFILE_PMF_RESERVED_NOF_BITS;
        pmf_reserved_start_bit = ETH_RIF_PROFILE_PMF_RESERVED_START_BIT;
        egress_reserved_nof_bits = ETH_RIF_PROFILE_EGRESS_RESERVED_NOF_BITS;
        nof_ingress_profiles = NOF_ETH_RIF_PROFILES;
        pmf_bits_mask = (((1 << pmf_reserved_nof_bits) - 1) << pmf_reserved_start_bit);
        pmf_in_lif_profile = old_in_lif_profile & pmf_bits_mask;
    }
    else
    {
        pmf_reserved_nof_bits = IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS;
        pmf_reserved_start_bit = IN_LIF_PROFILE_PMF_RESERVED_START_BIT;
        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
        {
            egress_reserved_nof_bits = IN_LIF_PROFILE_EGRESS_RESERVED_NOF_BITS;
            in_lif_profile_egress_reserved_start_bit = IN_LIF_PROFILE_EGRESS_RESERVED_START_BIT;
        }
        else
        {
            egress_reserved_nof_bits = IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE;
            in_lif_profile_egress_reserved_start_bit =
                IN_LIF_PROFILE_PMF_RESERVED_START_BIT - IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE;
        }
        nof_ingress_profiles = NOF_IN_LIF_PROFILES;
        pmf_bits_mask = (((1 << pmf_reserved_nof_bits) - 1) << pmf_reserved_start_bit);
        pmf_in_lif_profile = old_in_lif_profile & pmf_bits_mask;
    }

    /*
     * write ingress part: if we have is_first = 1, then ingress attributes were changed, need to write to ingress HW
     */
    if (is_first)
    {
        /*
         * when we add a new LIF profile, we should duplicate the profile in all the indexes that can be accessed
         * with the original in_lif_profile/eth_rif_profile and the reserved bits of PMF/Egress, we duplicate according to intf_type and nof reserved bits.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_PROFILE_TABLE, &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RPF_MODE, INST_SINGLE, rpf_mode);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CS_IN_LIF_VTT_PROFILE,
                                     INST_SINGLE, cs_in_lif_profile_value);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CS_IN_LIF_FWD_PROFILE,
                                     INST_SINGLE, cs_in_lif_fwd_profile_value);

        if (dnx_data_trap.ingress.protocol_traps_init_mode_get(unit) == DNX_RX_TRAP_PROTOCOL_MODE_IN_LIF)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L_2_CP_TRAP_HANDLING_PROFILE,
                                         INST_SINGLE,
                                         in_lif_profile_info->ingress_fields.protocol_trap_profiles.l2cp_trap_profile);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ICMPV4_TRAP_HANDLING_PROFILE,
                                         INST_SINGLE,
                                         in_lif_profile_info->ingress_fields.
                                         protocol_trap_profiles.icmpv4_trap_profile);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ICMPV6_TRAP_HANDLING_PROFILE, INST_SINGLE,
                                         in_lif_profile_info->ingress_fields.
                                         protocol_trap_profiles.icmpv6_trap_profile);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ARP_TRAP_HANDLING_PROFILE, INST_SINGLE,
                                         in_lif_profile_info->ingress_fields.protocol_trap_profiles.arp_trap_profile);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGMP_TRAP_HANDLING_PROFILE, INST_SINGLE,
                                         in_lif_profile_info->ingress_fields.protocol_trap_profiles.igmp_trap_profile);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DHCP_TRAP_HANDLING_PROFILE, INST_SINGLE,
                                         in_lif_profile_info->ingress_fields.protocol_trap_profiles.dhcp_trap_profile);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NON_AUTH_8021X_TRAP_HANDLING_PROFILE,
                                         INST_SINGLE,
                                         in_lif_profile_info->ingress_fields.
                                         protocol_trap_profiles.non_auth_8021x_trap_profile);
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DA_NOT_FOUND_DESTINATION_PROFILE,
                                     INST_SINGLE, da_not_found_dst_profile);

        max_counter_value = 1 << (pmf_reserved_nof_bits + egress_reserved_nof_bits);

        /*
         * loop over the possible values of the reserved bits, and duplicate HW value to all the possible keys
         * the key is built by shifting the reserved bits value left and append new_in_lif_profile
         */
        for (hw_write_counter = 0; hw_write_counter < max_counter_value; hw_write_counter++)
        {
            if (intf_type == ETH_RIF)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE,
                                           CALCULATE_IN_LIF_PROFILE_FROM_ETH_RIF_PROFILE
                                           (IN_LIF_PROFILE_TABLE_MSB_PREFIX,
                                            (*new_in_lif_profile |
                                             (hw_write_counter <<
                                              (NOF_ETH_RIF_PROFILE_BITS -
                                               (pmf_reserved_nof_bits + egress_reserved_nof_bits))))));
            }
            else
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE,
                                           *new_in_lif_profile | (hw_write_counter <<
                                                                  (NOF_IN_LIF_PROFILE_BITS -
                                                                   (pmf_reserved_nof_bits +
                                                                    egress_reserved_nof_bits))));
            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_PROFILE_TABLE, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROUTING_ENABLERS_PROFILE, INST_SINGLE,
                                     routing_enablers_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE_IVL_SVL, INST_SINGLE,
                                     in_lif_profile_info->ingress_fields.mact_mode);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE_L2_MC_FWD_TYPE, INST_SINGLE,
                                     in_lif_profile_info->ingress_fields.l2_mc_fwd_type);

        /*
         * loop over the possible values of the reserved bits, and duplicate HW value to all the possible keys
         * the key is built by shifting the reserved bits value left and append new_in_lif_profile
         */
        for (hw_write_counter = 0; hw_write_counter < max_counter_value; hw_write_counter++)
        {
            if (intf_type == ETH_RIF)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE,
                                           CALCULATE_VSI_PROFILE_FROM_ETH_RIF_PROFILE(VSI_MSB_PREFIX,
                                                                                      (*new_in_lif_profile |
                                                                                       (hw_write_counter <<
                                                                                        (NOF_ETH_RIF_PROFILE_BITS -
                                                                                         (pmf_reserved_nof_bits +
                                                                                          egress_reserved_nof_bits))))));
            }
            else
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI_PROFILE,
                                           *new_in_lif_profile | (hw_write_counter <<
                                                                  (NOF_IN_LIF_PROFILE_BITS -
                                                                   (pmf_reserved_nof_bits +
                                                                    egress_reserved_nof_bits))));
            }
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

    /*
     * append the egress in_lif_profile bits to the output
     */
    if (intf_type == ETH_RIF)
    {
        *new_in_lif_profile =
            pmf_in_lif_profile | (egress_in_lif_profile << ETH_RIF_PROFILE_EGRESS_RESERVED_START_BIT) |
            *new_in_lif_profile;
    }
    else
    {
        *new_in_lif_profile =
            pmf_in_lif_profile | (egress_in_lif_profile << in_lif_profile_egress_reserved_start_bit) |
            *new_in_lif_profile;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief adds ingress flag to flags if the corresponding bit is on in the routing enablers vector
 * routing_enablers_vector - the input routing enablers vector
 * flags - the output ingress flags
 * ingress_flag - the input ingress flag to check, if the corresponding bit in the routing enablers vector is on then we add this flag to flags
 * enabler_offset - the offset of the corresponding enabler in the routing enablers vector
 */
static shr_error_e
dnx_in_lif_profile_routing_enablers_restore_flag(
    int unit,
    uint32 *routing_enablers_vector,
    uint32 *flags,
    int ingress_flag,
    int enabler_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(routing_enablers_vector, _SHR_E_PARAM, "NULL input - routing_enablers_vector");

    if (enabler_offset < 32)
    {
        if (~(routing_enablers_vector[0]) & SAL_BIT(enabler_offset))
        {
            *flags |= ingress_flag;
        }
    }
    else
    {
        if (~(routing_enablers_vector[1]) & (SAL_BIT(enabler_offset - 32)))
        {
            *flags |= ingress_flag;
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   in_lif_profile.h file
 */
void
in_lif_profile_info_t_init(
    int unit,
    in_lif_profile_info_t * in_lif_profile_info)
{
    int system_headers_mode;
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    /*
     * init ingress in_lif_profile fields
     */
    in_lif_profile_info->ingress_fields.l3_ingress_flags = 0;
    in_lif_profile_info->ingress_fields.pwe_nof_eth_tags = ZERO_TAGS;
    in_lif_profile_info->ingress_fields.urpf_mode = bcmL3IngressUrpfDisable;
    sal_memset(&(in_lif_profile_info->ingress_fields.protocol_trap_profiles), 0,
               sizeof(in_lif_profile_info->ingress_fields.protocol_trap_profiles));
    in_lif_profile_info->ingress_fields.cs_in_lif_profile_flags = 0;
    in_lif_profile_info->ingress_fields.mact_mode = DEFAULT_VSI_PROFILE_IVL_SVL_MODE;
    in_lif_profile_info->ingress_fields.l2_mc_fwd_type = DEFAULT_VSI_PROFILE_L2_MC_FWD_TYPE_MODE;
    /** Init default_frwrd_dst as add offset(0) to vsi.default-destination*/
    sal_memset(in_lif_profile_info->ingress_fields.default_frwrd_dst, 0,
               sizeof(in_lif_profile_info->ingress_fields.default_frwrd_dst));
    in_lif_profile_info->ingress_fields.default_frwrd_dst[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_MC].add_vsi_dst =
        TRUE;
    in_lif_profile_info->ingress_fields.default_frwrd_dst[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_BC].add_vsi_dst =
        TRUE;
    in_lif_profile_info->ingress_fields.default_frwrd_dst[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_UC].add_vsi_dst =
        TRUE;

    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        /*
         * init egress in_lif_profile fields
         */
        in_lif_profile_info->egress_fields.in_lif_orientation = DEFAULT_IN_LIF_ORIENTATION;
        in_lif_profile_info->egress_fields.ive_inner_qos_dp_profile = 0;
        in_lif_profile_info->egress_fields.ive_outer_qos_dp_profile = 0;
        in_lif_profile_info->egress_fields.lif_same_interface_mode = DEVICE_SCOPE;
    }
    else
    {
        /*
         * init egress in_lif_profile fields in jr mode
         */
        in_lif_profile_info->egress_fields.in_lif_orientation = DEFAULT_IN_LIF_ORIENTATION;
        in_lif_profile_info->egress_fields.ive_inner_qos_dp_profile = 0;
        in_lif_profile_info->egress_fields.ive_outer_qos_dp_profile = 0;
        in_lif_profile_info->egress_fields.lif_same_interface_mode = DEVICE_SCOPE;
    }
}

/**
 * \see
 *   in_lif_profile.h file
 */
shr_error_e
dnx_in_lif_profile_get_data(
    int unit,
    int in_lif_profile,
    in_lif_profile_info_t * in_lif_profile_info,
    in_lif_intf_type_e intf_type)
{
    in_lif_profile_info_egress_t egress_in_lif_profile_info;
    in_lif_template_data_t in_lif_profile_template_data;
    uint32 enablers_vector_template_data[2];
    int enablers_vector_ref_count, ref_count, system_headers_mode, in_lif_profile_jr_mode,
        in_lif_profile_exclude_orientation, in_lif_orientation;
    uint32 flags;
    dnx_default_frwrd_dst_t default_frwrd_dst[DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    flags = 0;
    in_lif_profile_jr_mode = 0;
    in_lif_profile_exclude_orientation = 0;
    in_lif_orientation = 0;
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    SHR_NULL_CHECK(in_lif_profile_info, _SHR_E_PARAM, "NULL input - in_lif_profile_info");
    in_lif_profile_info_t_init(unit, in_lif_profile_info);
    /*
     * Get in_lif_profile template data
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_template_data_get
                    (unit, in_lif_profile, intf_type, &in_lif_profile_template_data));

    /*
     * assign RPF mode
     */
    switch (in_lif_profile_template_data.rpf_mode)
    {
        case DBAL_ENUM_FVAL_RPF_MODE_LOOSE:
            in_lif_profile_info->ingress_fields.urpf_mode = bcmL3IngressUrpfLoose;
            break;
        case DBAL_ENUM_FVAL_RPF_MODE_STRICT:
            in_lif_profile_info->ingress_fields.urpf_mode = bcmL3IngressUrpfStrict;
            break;
        case DBAL_ENUM_FVAL_RPF_MODE_DISABLE:
            in_lif_profile_info->ingress_fields.urpf_mode = bcmL3IngressUrpfDisable;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "urpf_mode invalid : %d\n", in_lif_profile_template_data.rpf_mode);
            break;
    }

    /*
     * get routing enablers vector
     */
    SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_profile_l3_enablers.profile_data_get
                    (unit, _SHR_CORE_ALL, in_lif_profile_template_data.routing_enablers_profile,
                     &enablers_vector_ref_count, enablers_vector_template_data));

    /*
     * assign l3_ingress_flags
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_translate_to_l3_ingress_flags
                    (unit, in_lif_profile_template_data, &(in_lif_profile_info->ingress_fields.l3_ingress_flags)));

    /*
     * assign cs_in_lif_profile_flags
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_translate_to_cs_in_lif_profile_flags
                    (unit, in_lif_profile_template_data,
                     &(in_lif_profile_info->ingress_fields.cs_in_lif_profile_flags)));

    /*
     * assign nof ethernet tags
     */
    if (in_lif_profile_template_data.lif_type == PWE)
    {
        /*
         * LSB indicates LIF/NWK scope and bits 2 and 3 LSB indicate the nof_tags
         */
        in_lif_profile_info->ingress_fields.pwe_nof_eth_tags =
            ((in_lif_profile_template_data.cs_in_lif_profile >> 1) -
             DBAL_ENUM_FVAL_LIF_VTT4_CSP_2BIT_PWE_NOF_SD_TAGS_0_SD_TAGS) & 0x3;
    }

    dnx_in_lif_profile_routing_enablers_restore_flag(unit, enablers_vector_template_data, &flags,
                                                     BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST, IPV4_ENABLER_OFFSET);
    dnx_in_lif_profile_routing_enablers_restore_flag(unit, enablers_vector_template_data, &flags,
                                                     BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST, IPV4_MC_ENABLER_OFFSET);
    dnx_in_lif_profile_routing_enablers_restore_flag(unit, enablers_vector_template_data, &flags,
                                                     BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST, IPV6_ENABLER_OFFSET);
    dnx_in_lif_profile_routing_enablers_restore_flag(unit, enablers_vector_template_data, &flags,
                                                     BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST, IPV6_MC_ENABLER_OFFSET);
    dnx_in_lif_profile_routing_enablers_restore_flag(unit, enablers_vector_template_data, &flags,
                                                     BCM_L3_INGRESS_ROUTE_DISABLE_MPLS, MPLS_ENABLER_OFFSET);

    in_lif_profile_info->ingress_fields.l3_ingress_flags |= flags;

    /*
     * assign trap profiling
     */
    in_lif_profile_info->ingress_fields.protocol_trap_profiles = in_lif_profile_template_data.protocol_trap_profiles;
    /*
     * assign mact mode
     */
    in_lif_profile_info->ingress_fields.mact_mode = in_lif_profile_template_data.mact_mode;
    /*
     * assign l2 mc forward type
     */
    in_lif_profile_info->ingress_fields.l2_mc_fwd_type = in_lif_profile_template_data.l2_mc_fwd_type;
    /*
     * assign egress in_lif_profile fields in jr2 mode
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_algo_extract_egress_profile
                    (unit, in_lif_profile, &egress_in_lif_profile_info, &in_lif_profile_jr_mode,
                     &in_lif_profile_exclude_orientation, &in_lif_orientation, intf_type));
    in_lif_profile_info->egress_fields.in_lif_orientation = egress_in_lif_profile_info.in_lif_orientation;
    in_lif_profile_info->egress_fields.ive_outer_qos_dp_profile = egress_in_lif_profile_info.ive_outer_qos_dp_profile;
    in_lif_profile_info->egress_fields.ive_inner_qos_dp_profile = egress_in_lif_profile_info.ive_inner_qos_dp_profile;
    in_lif_profile_info->egress_fields.lif_same_interface_mode = egress_in_lif_profile_info.lif_same_interface_mode;

    /*
     * assign in_lif default forward destinations
     */
    sal_memset(default_frwrd_dst, 0, sizeof(default_frwrd_dst));
    SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_da_not_found_destination.profile_data_get
                    (unit, _SHR_CORE_ALL, in_lif_profile_template_data.da_not_found_dst_profile, &ref_count,
                     default_frwrd_dst));
    sal_memcpy(in_lif_profile_info->ingress_fields.default_frwrd_dst, default_frwrd_dst, sizeof(default_frwrd_dst));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   in_lif_profile.h file
 */
shr_error_e
dnx_in_lif_profile_get_ref_count(
    int unit,
    int in_lif_profile,
    int *ref_count,
    in_lif_intf_type_e intf_type)
{
    in_lif_template_data_t in_lif_profile_template_data;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * determine the template manager to look in according to intf_type
     */
    if (intf_type == ETH_RIF)
    {
        in_lif_profile = in_lif_profile & ETH_RIF_PROFILE_USED_BITS_MASK;
        SHR_IF_ERR_EXIT(algo_lif_profile_db.eth_rif_profile.profile_data_get
                        (unit, _SHR_CORE_ALL, in_lif_profile, ref_count, &in_lif_profile_template_data));
    }
    else if (intf_type == LIF)
    {
        in_lif_profile = in_lif_profile & IN_LIF_PROFILE_USED_BITS_MASK;
        SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_profile.profile_data_get
                        (unit, _SHR_CORE_ALL, in_lif_profile, ref_count, &in_lif_profile_template_data));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "invalid intf_type\n");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   in_lif_profile.h file
 */
shr_error_e
dnx_in_lif_profile_set(
    int unit,
    bcm_gport_t port,
    int new_in_lif_profile)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dbal_tables_e dbal_table_id;
    int local_in_lif;
    uint32 entry_handle_id, res_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    local_in_lif = gport_hw_resources.local_in_lif;
    dbal_table_id = gport_hw_resources.inlif_dbal_table_id;
    res_type = gport_hw_resources.inlif_dbal_result_type;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, res_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, new_in_lif_profile);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   in_lif_profile.h file
 */
shr_error_e
dnx_in_lif_profile_get(
    int unit,
    bcm_gport_t port,
    int *in_lif_profile_p)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dbal_tables_e dbal_table_id;
    int local_in_lif, result_type;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(in_lif_profile_p, _SHR_E_PARAM, "in_lif_profile_p");

    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    local_in_lif = gport_hw_resources.local_in_lif;
    dbal_table_id = gport_hw_resources.inlif_dbal_table_id;
    result_type = gport_hw_resources.inlif_dbal_result_type;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /** read entry InLIF profile */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) in_lif_profile_p));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

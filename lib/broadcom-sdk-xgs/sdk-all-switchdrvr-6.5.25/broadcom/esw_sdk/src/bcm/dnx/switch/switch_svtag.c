/**
 * \file switch_svtag.c
 *
 *
 * Purpose:
 *   Holds all the SVTAG related configuration.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
/*
 *  Exported functions have their descriptions in the switch_svtag.h file.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH
/*
 * Includes
 * {
 */
#include <soc/dnx/mdb.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bslenum.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/switch/switch_svtag.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm/error.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/port/port_match.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/port/port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm/trunk.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
/*
 * }
 */
/*
 * Defines
 * {
 */
/*
 * The switch SVTAG egress supported flags.
 */
#define SWITCH_SVTAG_EGRESS_SUPPORTED_FLAGS         (BCM_SWITCH_SVTAG_EGRESS_REPLACE | BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY)

/*
 * The NOF UIINT32s required to hold the SVTAG field in the HW
 */
#define SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32                  2
/*
 * The number of DBAL SVTAG tables
 */
#define SWITCH_SVTAG_NOF_SVTAG_DBAL_TABLES                    2
/*
 * The size of a field which is used for boolean indication
 */
#define SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE               1
/*
 * The types of LIFs that can enable SVTAG
 */
#define SWITCH_SVTAG_SUPPORTED_LIF_TYPES(gport)               (BCM_GPORT_IS_TUNNEL(gport) || BCM_GPORT_IS_VLAN_PORT(gport))
/*
 * The types of LIFs that support accumulation in SVTAG configuration
 */
#define SWITCH_SVTAG_IS_GPORT_SUPPORT_ACCUMULATION(gport)     (BCM_GPORT_IS_TUNNEL(gport))
/*
 * }
 */

/*
 * Macros
 * {
 */
/*
 * Add a field value into the SVTAG at the designated position.
 */
#define SWITCH_SVTAG_UPDATE_SVTAG_FIELD(svtag,field,position) svtag |= (field << position)
/*
 * Extract a field from the SVTAG
 */
#define SWITCH_SVTAG_EXTRACT_FIELD_FROM_SVTAG(svtag,pos,size) ((svtag >> pos) & (UTILEX_BITS_MASK(size-1,0)))

/*
 * }
 */

/*
 * Functions
 * {
 */

/**
 * \brief
 * This function updates the SVTAG DBAL table with the key field and the result type according to the gport type.
 * \param [in] unit            - The unit number.
 * \param [in] gport           - The gport
 * \param [in] entry_handle_id - the DBAL table handle which is allocated by the calling function.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 */
shr_error_e
dnx_switch_svtag_dabl_table_field_set(
    int unit,
    bcm_gport_t gport,
    uint32 entry_handle_id)
{

    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_core_t core;
    uint32 out_tm_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (SWITCH_SVTAG_SUPPORTED_LIF_TYPES(gport))
    {
        /*
         * Tunnel
         *
         * In case this is a tunnel, find the matching local LIF which is the table key
         */
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

        uint32 lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESEM_TUNNEL_SVTAG, entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_ESEM_TUNNEL_SVTAG_ETPS_SVTAG);

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_LOCAL_OUT_LIF,
                                   gport_hw_resources.local_out_lif);
    }
    else
    {
        /*
         * Get the local port
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY,
                                                        &gport_info));

        /*
         * Convert the local port to a TM port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_port_get(unit, gport_info.local_port, &core, &out_tm_port));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESEM_PORT_SVTAG, entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_ESEM_PORT_SVTAG_ETPS_SVTAG);

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_TM_PORT, out_tm_port);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Extract all the SVTAG fields from the HW field value in the SVTAG table
 * \param [in] unit           - The unit number.
 * \param [in] svtag_hw_entry - the HW SVTAG table entry value
 * \param [out] svtag_info    - The SVTAG fields values
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 */
void
dnx_switch_svtag_info_from_svtag_hw_entry_get(
    int unit,
    uint32 *svtag_hw_entry,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    uint32 accumulation = 0;
    uint32 svtag = 0;

    SHR_BITCOPY_RANGE(&svtag, 0, svtag_hw_entry, dnx_data_switch.svtag.egress_svtag_hw_field_position_bits_get(unit),
                      dnx_data_switch.svtag.svtag_label_size_bits_get(unit));
    SHR_BITCOPY_RANGE(&accumulation, 0, svtag_hw_entry,
                      dnx_data_switch.svtag.egress_svtag_accumulation_indication_hw_field_position_bits_get(unit),
                      SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE);

    svtag_info->accumulation = accumulation & UTILEX_BITS_MASK(SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE - 1, 0);

    svtag_info->sci =
        SWITCH_SVTAG_EXTRACT_FIELD_FROM_SVTAG(svtag, dnx_data_switch.svtag.egress_svtag_sci_position_bits_get(unit),
                                              dnx_data_switch.svtag.egress_svtag_sci_size_bits_get(unit));
    svtag_info->pkt_type =
        SWITCH_SVTAG_EXTRACT_FIELD_FROM_SVTAG(svtag,
                                              dnx_data_switch.svtag.egress_svtag_pkt_type_position_bits_get(unit),
                                              dnx_data_switch.svtag.egress_svtag_pkt_type_size_bits_get(unit));
    svtag_info->ipv6_indication =
        SWITCH_SVTAG_EXTRACT_FIELD_FROM_SVTAG(svtag,
                                              dnx_data_switch.
                                              svtag.egress_svtag_ipv6_indication_position_bits_get(unit),
                                              SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE);
    svtag_info->offset_addr =
        SWITCH_SVTAG_EXTRACT_FIELD_FROM_SVTAG(svtag,
                                              dnx_data_switch.svtag.egress_svtag_offset_addr_position_bits_get(unit),
                                              dnx_data_switch.svtag.egress_svtag_offset_addr_size_bits_get(unit));
    svtag_info->signature =
        SWITCH_SVTAG_EXTRACT_FIELD_FROM_SVTAG(svtag,
                                              dnx_data_switch.svtag.egress_svtag_signature_position_bits_get(unit),
                                              dnx_data_switch.svtag.egress_svtag_signature_size_bits_get(unit));
}

/**
 * see .h file
 */
shr_error_e
dnx_switch_svtag_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Set the IRE swap command to place the SVTAG at the start of the packet.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SIT_INGRESS_GENERAL_CONFIGURATION, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_SWAP_TPID_OFFSET, INST_SINGLE,
                                 dnx_data_switch.svtag.ingress_svtag_position_bytes_get(unit));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_SWAP_TPID_SIZE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_SIT_TAG_SWAP_SIZE_4_BYTES);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Common verification that is used by several SVTAG APIs
 * \param [in] unit - The unit number.
 * \param [in] flags - API flags BCM_SWITCH_SVTAG_EGRESS_XXX.
 * \param [in] gport - gport
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 */
shr_error_e
dnx_switch_svtag_egress_entry_common_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport)
{
    uint8 is_physical_port;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_switch.svtag.supported_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "SVTAG isn't supported in this device.");
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {
        if (gport != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "If the default entry flag BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY was used the gport value must be 0.");
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, gport, &is_physical_port));

        if (BCM_GPORT_IS_TRUNK(gport) || (!SWITCH_SVTAG_SUPPORTED_LIF_TYPES(gport) && !is_physical_port))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport type.");
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * The bcm_dnx_switch_svtag_egress_entry_get input verification
 * \param [in] unit  - The unit number.
 * \param [in] flags - The SVTAG API flags.
 * \param [in] gport - gport
 * \param [in] svtag_info - The SVTAG fields structure
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 */
shr_error_e
dnx_switch_svtag_egress_entry_get_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_common_verify(unit, flags, gport));

    SHR_NULL_CHECK(svtag_info, _SHR_E_PARAM, "bcm_switch_svtag_egress_info_t");

    if (_SHR_IS_FLAG_SET(flags, ~BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The only supported flag for this API is the %d flag.",
                     BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The bcm_dnx_switch_svtag_egress_entry_traverse input verification
 * \param [in] unit - The unit number.
 * \param [in] flags - traverse flags
 * \param [in] trav_fn - the traverse call back function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 */
shr_error_e
dnx_switch_svtag_egress_entry_traverse_verify(
    int unit,
    uint32 flags,
    bcm_switch_svtag_egress_traverse_cb trav_fn)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(flags, ~BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The only supported flag for this API is the %d flag.",
                     BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL);
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL) && (trav_fn != NULL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "If the BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL flag is set the callback function must be null.");
    }

    if (!_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL))
    {
        SHR_NULL_CHECK(trav_fn, _SHR_E_PARAM, "bcm_switch_svtag_egress_traverse_cb");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The bcm_dnx_switch_svtag_egress_entry_delete input verification
 * \param [in] unit  - The unit number.
 * \param [in] flags - API flags BCM_SWITCH_SVTAG_EGRESS_XXX.
 * \param [in] gport - gport
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 */
shr_error_e
dnx_switch_svtag_egress_entry_delete_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_common_verify(unit, flags, gport));

    if (_SHR_IS_FLAG_SET(flags, ~BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The only supported flag for this API is the %d flag.",
                     BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The bcm_dnx_switch_svtag_egress_entry_add input verification
 * \param [in] unit - The unit number.
 * \param [in] flags - The SVTAG API flags.
 * \param [in] gport - gport
 * \param [in] svtag_info - The SVTAG fields values
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 */
shr_error_e
dnx_switch_svtag_egress_entry_add_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    int table_capacity = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Common verification for the SVTAG APIs
     */
    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_common_verify(unit, flags, gport));
    /** Verify capacity for physical table */
    SHR_IF_ERR_EXIT(mdb_db_infos.
                    capacity.get(unit, dnx_data_switch.svtag.svtag_physical_db_get(unit), &table_capacity));

    if (table_capacity == 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "SVTAG table is not supported for the current MDB profile.");
    }
    SHR_NULL_CHECK(svtag_info, _SHR_E_PARAM, "bcm_switch_svtag_egress_info_t");

    if (svtag_info->sci >= (1 << dnx_data_switch.svtag.egress_svtag_sci_size_bits_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SCI field %d exceeds the MAX size of %d.", svtag_info->sci,
                     (1 << dnx_data_switch.svtag.egress_svtag_sci_size_bits_get(unit)) - 1);
    }

    if (svtag_info->signature >= (1 << dnx_data_switch.svtag.egress_svtag_signature_size_bits_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "signature field %d exceeds the MAX size of %d.", svtag_info->signature,
                     (1 << dnx_data_switch.svtag.egress_svtag_signature_size_bits_get(unit)) - 1);
    }

    if (svtag_info->offset_addr >= (1 << dnx_data_switch.svtag.egress_svtag_offset_addr_size_bits_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "offset addr field %d exceeds the MAX size of %d.", svtag_info->offset_addr,
                     (1 << dnx_data_switch.svtag.egress_svtag_offset_addr_size_bits_get(unit)) - 1);
    }

    /*
     * In accumulation mode for MACSEC (not IPSEC) the full value of the SECTSG offset should derive directly from the stack size.
     * Alingend with the fact that the minimum value for svtag_info->offset_addr is > 0 we need to configure the
     *  offset_addr to be the negative to the value of the sectag_offset in the secure channel that we use
     * remark-
     * IPSEC offset can get a value of zero and therefore this is not relevant for IPSEC SVTAG configuration
     */
    if ((svtag_info->accumulation) &&
        (svtag_info->offset_addr > ((1 << dnx_data_switch.svtag.egress_svtag_offset_addr_size_bits_get(unit))
                                    - dnx_data_macsec.general.sectag_offset_min_value_get(unit))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "offset addr (%d) must be set to be smaller than the the complement of the SECTAG minimum value when the accumulation is set. (value smaller than %d)",
                     svtag_info->offset_addr, ((1 << dnx_data_switch.svtag.egress_svtag_offset_addr_size_bits_get(unit))
                                               - dnx_data_macsec.general.sectag_offset_min_value_get(unit)));
    }

    if ((svtag_info->pkt_type > bcmSvtagPktTypeKayMgmt) || (svtag_info->pkt_type < bcmSvtagPktTypeNonMacsec))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported option for SVTAG packet type %d.", svtag_info->pkt_type);
    }

    if (_SHR_IS_FLAG_SET(flags, ~SWITCH_SVTAG_EGRESS_SUPPORTED_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "One or more of the used flags 0x%x aren't supported (supported flags are 0x%x).",
                     flags, SWITCH_SVTAG_EGRESS_SUPPORTED_FLAGS);
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY) && svtag_info->accumulation != FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The accumulation field must be set to 0 in case the BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY is used");
    }

    if (!SWITCH_SVTAG_IS_GPORT_SUPPORT_ACCUMULATION(gport) && svtag_info->accumulation)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The accumulation mode field can be set to 1 only for the case where the key is a tunnel.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Adds an SVTAG entry
 * \param [in] unit - The unit number.
 * \param [in] flags - The SVTAG API flags.
 * \param [in] gport - gport
 * \param [in] svtag_info - The SVTAG fields values
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 */
int
bcm_dnx_switch_svtag_egress_entry_add(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    uint32 entry_handle_id;
    uint32 svtag_hw_entry[SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32];
    uint32 svtag = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_add_verify(unit, flags, gport, svtag_info));

    /*
     * Update the SVTAG field with all of the SVTAG structure fields.
     */
    SWITCH_SVTAG_UPDATE_SVTAG_FIELD(svtag, svtag_info->sci,
                                    dnx_data_switch.svtag.egress_svtag_sci_position_bits_get(unit));
    SWITCH_SVTAG_UPDATE_SVTAG_FIELD(svtag, svtag_info->pkt_type,
                                    dnx_data_switch.svtag.egress_svtag_pkt_type_position_bits_get(unit));
    SWITCH_SVTAG_UPDATE_SVTAG_FIELD(svtag, svtag_info->ipv6_indication,
                                    dnx_data_switch.svtag.egress_svtag_ipv6_indication_position_bits_get(unit));
    SWITCH_SVTAG_UPDATE_SVTAG_FIELD(svtag, svtag_info->offset_addr,
                                    dnx_data_switch.svtag.egress_svtag_offset_addr_position_bits_get(unit));
    SWITCH_SVTAG_UPDATE_SVTAG_FIELD(svtag, svtag_info->signature,
                                    dnx_data_switch.svtag.egress_svtag_signature_position_bits_get(unit));

    /*
     * In case the default entry flag is set, update the default entry
     */
    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SVTAG_DEFAULT, &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SVTAG_DEFAULT_VALUE, INST_SINGLE, svtag);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        dbal_entry_action_flags_e dbal_action_flag = DBAL_COMMIT_UPDATE;
        uint32 accumulation = svtag_info->accumulation;
        sal_memset(svtag_hw_entry, 0, sizeof(uint32) * SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32);

        SHR_BITCOPY_RANGE(svtag_hw_entry, dnx_data_switch.svtag.egress_svtag_hw_field_position_bits_get(unit), &svtag,
                          0, dnx_data_switch.svtag.svtag_label_size_bits_get(unit));

        SHR_BITCOPY_RANGE(svtag_hw_entry,
                          dnx_data_switch.svtag.egress_svtag_accumulation_indication_hw_field_position_bits_get(unit),
                          &accumulation, 0, SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE);

        /*
         * Update the DBAL table with the SVTAG entry.
         *
         * Although the TM port table handle is taken, it won't be necessarily used, as this determine in the
         * dnx_switch_svtag_dabl_table_field_set function but the handle must be taken in the calling function
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_SVTAG, &entry_handle_id));

        SHR_IF_ERR_EXIT(dnx_switch_svtag_dabl_table_field_set(unit, gport, entry_handle_id));

        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_DESTINATION, INST_SINGLE,
                                         svtag_hw_entry);
        if (!_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_REPLACE))
        {
            dbal_action_flag = DBAL_COMMIT;
            /*
             * In case this is a physical port (gport can only be a tunnel or a port in this case)
             * add matching ESEM CMD to it in order invoke the SVTAG lookup
             */
            if (!SWITCH_SVTAG_SUPPORTED_LIF_TYPES(gport))
            {
                SHR_IF_ERR_EXIT(dnx_port_match_svtag_esem_cmd_set(unit, gport, TRUE));
            }
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_action_flag));

    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Gets and SVTAG entry
 * \param [in] unit  - The unit number.
 * \param [in] flags - The SVTAG API flags.
 * \param [in] gport - gport
 * \param [out] svtag_info - The SVTAG fields values
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 */
int
bcm_dnx_switch_svtag_egress_entry_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    uint32 entry_handle_id;
    uint32 svtag_hw_entry[SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32];

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_get_verify(unit, flags, gport, svtag_info));

    sal_memset(svtag_hw_entry, 0, sizeof(uint32) * SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32);
    /*
     * In case the default entry flag is set, return the default entry
     */
    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {
        uint32 svtag = 0;
        uint32 accumulation = 0;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SVTAG_DEFAULT, &entry_handle_id));

        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SVTAG_DEFAULT_VALUE, INST_SINGLE, &svtag);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Update the SVTAG value and the accumulation (0) into the HW field to extract the SVTAG fields values into the
         * svtag_info structure using a common function.
         */
        SHR_BITCOPY_RANGE(svtag_hw_entry, dnx_data_switch.svtag.egress_svtag_hw_field_position_bits_get(unit), &svtag,
                          0, dnx_data_switch.svtag.svtag_label_size_bits_get(unit));

        SHR_BITCOPY_RANGE(svtag_hw_entry,
                          dnx_data_switch.svtag.egress_svtag_accumulation_indication_hw_field_position_bits_get(unit),
                          &accumulation, 0, SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE);

    }
    else
    {

        /*
         * Although the TM port table handle is taken, it won't be necessarily used, as this will be  determine in the
         * dnx_switch_svtag_dabl_table_field_set function but the handle must be taken in the calling function
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_SVTAG, &entry_handle_id));

        SHR_IF_ERR_EXIT(dnx_switch_svtag_dabl_table_field_set(unit, gport, entry_handle_id));

        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_ENCAP_DESTINATION, INST_SINGLE,
                                       svtag_hw_entry);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

    dnx_switch_svtag_info_from_svtag_hw_entry_get(unit, svtag_hw_entry, svtag_info);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete and SVTAG entry
 * \param [in] unit  - The unit number.
 * \param [in] flags - API flags BCM_SWITCH_SVTAG_EGRESS_XXX.
 * \param [in] gport - gport
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 */
int
bcm_dnx_switch_svtag_egress_entry_delete(
    int unit,
    uint32 flags,
    bcm_gport_t gport)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_delete_verify(unit, flags, gport));
    /*
     * In case the default entry flag is set, clear the default entry
     */
    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {
        /*
         * Set the default SVTAG value into 0 which also include setting the packet type into "no TAG action in the NIF"
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SVTAG_DEFAULT, &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SVTAG_DEFAULT_VALUE, INST_SINGLE, 0);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /*
         * Although the TM port table handle is taken, it won't be necessarily used, as this determine in the
         * dnx_switch_svtag_dabl_table_field_set function but the handle must be taken in the calling function
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_SVTAG, &entry_handle_id));

        SHR_IF_ERR_EXIT(dnx_switch_svtag_dabl_table_field_set(unit, gport, entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, (DBAL_COMMIT)));
        /*
         * In case this is a physical port (gport can only be a tunnel or a port in this case)
         * remove the matching ESEM CMD that invokes the SVTAG lookup as the entry is removed
         */
        if (!BCM_GPORT_IS_TUNNEL(gport))
        {
            SHR_IF_ERR_EXIT(dnx_port_match_svtag_esem_cmd_set(unit, gport, FALSE));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse over the SVTAG entries
 * \param [in] unit - The unit number.
 * \param [in] flags - traverse flags BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_XXX
 * \param [in] trav_fn - the traverse call back function
 * \param [in] user_data - user data that used by the CB function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 */
int
bcm_dnx_switch_svtag_egress_entry_traverse(
    int unit,
    uint32 flags,
    bcm_switch_svtag_egress_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    int table_iter;
    int is_end;
    dbal_tables_e svtag_tables[SWITCH_SVTAG_NOF_SVTAG_DBAL_TABLES] =
        { DBAL_TABLE_ESEM_PORT_SVTAG, DBAL_TABLE_ESEM_TUNNEL_SVTAG };
    dbal_fields_e svtag_keys[SWITCH_SVTAG_NOF_SVTAG_DBAL_TABLES] =
        { DBAL_FIELD_OUT_TM_PORT, DBAL_FIELD_ESEM_LOCAL_OUT_LIF };
    uint32 svtag_hw_entry[SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32];
    bcm_switch_svtag_egress_info_t svtag_info;
    bcm_gport_t gport;
    bcm_port_t logical_port;
    int out_tm_valid;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_traverse_verify(unit, flags, trav_fn));

    /** traverse over all the SVTAG tables */
    for (table_iter = 0; table_iter < SWITCH_SVTAG_NOF_SVTAG_DBAL_TABLES; table_iter++)
    {
        if ((_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL)))
        {
            /*
             * Before clearing all the entries of DBAL_TABLE_ESEM_PORT_SVTAG,
             * iterate over the ports defined in the table and remove the SVTAG command from their ESEM CMD.
             * (this is done in order to achieve synchronization with the data in the ESEM CMD table)
             */
            if (svtag_tables[table_iter] == DBAL_TABLE_ESEM_PORT_SVTAG)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_SVTAG, &entry_handle_id));
                SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
                /*
                 * Receive first entry in table.
                 */
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                while (!is_end)
                {
                    uint32 port;
                    /** Get TM port */
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_OUT_TM_PORT, &port));

                    /** Convert TM port to logical port */
                    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_port_ref_count_get(unit, core_idx, port, &out_tm_valid));
                        if (out_tm_valid)
                        {
                            /** Matching logical port was found on this core */
                            SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_to_logical_get(unit, core_idx, port, &logical_port));
                            break;
                        }
                    }

                    /** Disable the SVTAG setting in ESEM CMD attached to the port */
                    SHR_IF_ERR_EXIT(dnx_port_match_svtag_esem_cmd_set(unit, logical_port, FALSE));
                    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                }
            }
            SHR_IF_ERR_EXIT(dbal_table_clear(unit, svtag_tables[table_iter]));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, svtag_tables[table_iter], &entry_handle_id));
            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
            /*
             * Receive first entry in table.
             */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            while (!is_end)
            {
                uint32 svtag_key;

                sal_memset(&svtag_info, 0, sizeof(bcm_switch_svtag_egress_info_t));

                /*
                 * Receive key and value fields of the entry.
                 */
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, svtag_keys[table_iter], &svtag_key));

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_ENCAP_DESTINATION, INST_SINGLE, svtag_hw_entry));

                dnx_switch_svtag_info_from_svtag_hw_entry_get(unit, svtag_hw_entry, &svtag_info);

                if (svtag_keys[table_iter] == DBAL_FIELD_ESEM_LOCAL_OUT_LIF)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                    (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, _SHR_CORE_ALL,
                                     svtag_key, &gport));
                }
                else
                {
                    /** Convert TM port to logical port */
                    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_port_ref_count_get
                                        (unit, core_idx, svtag_key, &out_tm_valid));
                        if (out_tm_valid)
                        {
                            /** Matching logical port was found on this core */
                            SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_to_logical_get
                                            (unit, core_idx, svtag_key, &logical_port));
                            break;
                        }
                    }

                    BCM_GPORT_LOCAL_SET(gport, logical_port);
                }

                /*
                 * Invoke callback function
                 */
                SHR_IF_ERR_EXIT((*trav_fn) (unit, 0, gport, &svtag_info, user_data));

                /*
                 * Receive next entry in table.
                 */
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_switch_svtag_encapsulation_add(
    int unit,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_svtag_encapsulation_get(
    int unit,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_svtag_encapsulation_delete(
    int unit,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_svtag_encapsulation_traverse(
    int unit,
    uint32 flags,
    bcm_switch_svtag_encapsulation_traverse_cb trav_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

/*
 * See .h file
 */
shr_error_e
dnx_switch_svtag_is_present_in_esem_cmd(
    int unit,
    uint32 esem_cmd_profile,
    int *app_db_id,
    uint8 *found)
{
    dnx_esem_access_if_t esem_if;
    dbal_enum_value_field_esem_offset_e esem_entry_offset;
    dnx_esem_cmd_data_t esem_cmd_data;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_sw_get(unit, esem_cmd_profile, &esem_cmd_data, NULL));

    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_info_get
                    (unit, 0, ESEM_ACCESS_TYPE_SVTAG, 1, &esem_if, &esem_entry_offset, NULL));

    if (esem_cmd_data.esem[esem_if].valid
        && ((esem_cmd_data.esem[esem_if].app_db_id == DBAL_ENUM_FVAL_ESEM_APP_DB_ID_OUT_LIF)
            || (esem_cmd_data.esem[esem_if].app_db_id == DBAL_ENUM_FVAL_ESEM_APP_DB_ID_OUT_TM_PORT))
        && (esem_cmd_data.esem[esem_if].designated_offset == esem_entry_offset))
    {
        *found = TRUE;
        *app_db_id = esem_cmd_data.esem[esem_if].app_db_id;
    }
    else
    {
        *found = FALSE;
        *app_db_id = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set the IRE swap state for SVTAG
 * (In case the SVTAG is enabled an IRE swap is required for placng the SVTAG at the start of the packet)
 * \param [in] unit - The unit number.
 * \param [in] ptc - the PTC.
 * \param [in] core_id - the core ID.
 * \param [in] enable - Bool flag that specify if the swap should be enabled or disabled
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected
 */
static shr_error_e
dnx_switch_svtag_swap_set(
    int unit,
    uint32 ptc,
    int core_id,
    uint8 enable)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC, ptc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAG_SWAP_ENABLE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAG_SWAP_OP_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_GLOBAL_TRIGGER);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Configure local port enable/disable MACSEC
 * \param [in] unit - The unit number.
 * \param [in] local_port - local port
 * \param [in] enable - Bool flag that specify if the swap should be enabled or disabled
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected
 */
static shr_error_e
dnx_switch_svtag_local_port_enable(
    int unit,
    bcm_port_t local_port,
    uint8 enable)
{
    uint32 ptc;
    int core_id;
    uint32 entry_handle_id, nof_bytes_to_skip_first_header;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, local_port, &ptc, &core_id));

    /*
     * Update the incoming port to allow IPSEC contexts
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PTC, ptc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT2, INST_SINGLE,
                                 enable ? DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT2_IPSEC_MACSEC :
                                 DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT2_DEFAULT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT3, INST_SINGLE,
                                 enable ? DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT3_IPSEC_MACSEC :
                                 DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT3_DEFAULT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT4, INST_SINGLE,
                                 enable ? DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT4_IPSEC_MACSEC :
                                 DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT4_DEFAULT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT5, INST_SINGLE,
                                 enable ? DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT5_IPSEC_MACSEC :
                                 DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT5_DEFAULT);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Set the incoming port to swap the SVTAG to the begging of the packet
     */
    SHR_IF_ERR_EXIT(dnx_switch_svtag_swap_set(unit, ptc, core_id, enable));
    /*
     * Configure the first header offset to skip for port,
     * The offset will be set to SVTAG size (bytes) if enable else to zero
     */
    nof_bytes_to_skip_first_header =
        (enable) ? UTILEX_TRANSLATE_BITS_TO_BYTES(dnx_data_switch.svtag.svtag_label_size_bits_get(unit)) : 0;
    /*
     * The dnx_port_first_header_size_to_skip_set supports trunks or ports
     * so in case of a local port the encoding should be removed.
     */
    SHR_IF_ERR_EXIT(dnx_port_first_header_size_to_skip_set(unit, local_port, nof_bytes_to_skip_first_header));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * verifies the input for dnx_switch_svtag_port_set function
 * the above input is received from the bcm_dnx_switch_control_port_set function
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - gport or local port.
 * \param [in] enable -  Bool flag for enable or disable svtag.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 */
shr_error_e
dnx_switch_svtag_port_set_verify(
    int unit,
    bcm_gport_t gport,
    uint8 enable)
{
    bcm_port_t logical_port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_SET(gport))
    {
        if (dnx_algo_port_valid_verify(unit, gport) != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid local port (0x%x).\n", gport);
        }
    }
    else if (!BCM_GPORT_IS_TRUNK(gport) && !BCM_GPORT_IS_LOCAL(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport (0x%x) type.\n", gport);
    }

    if (!BCM_GPORT_IS_TRUNK(gport))
    {
        if (BCM_GPORT_IS_LOCAL(gport))
        {
            logical_port = BCM_GPORT_LOCAL_GET(gport);
        }
        else
        {
            logical_port = gport;
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

        if (!DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Expected ingress TM port (%d)\n", logical_port);

        }
        if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Expected egress TM port (%d)\n", logical_port);
        }
    }
    /** Enable verification */
    SHR_BOOL_VERIFY(enable, _SHR_E_PARAM, "\nenable");

exit:
    SHR_FUNC_EXIT;
}
/** See header file */
shr_error_e
dnx_switch_svtag_macsec_set(
    int unit,
    bcm_gport_t gport,
    int enable)
{

    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    if (dnx_data_switch.svtag.feature_get(unit, dnx_data_switch_svtag_egress_svtag_enable_by_context))
    {
        uint32 pp_port_index;

        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            dbal_enum_value_field_etpp_forward_port_cs_var_e forward_cs;
            dbal_enum_value_field_etpp_enc1_port_cs_var_e enc_1_cpp;
            dbal_enum_value_field_etpp_enc5_port_cs_var_e enc_5_cpp;
            if (enable)
            {
                forward_cs = DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_MACSEC;
                enc_1_cpp = DBAL_ENUM_FVAL_ETPP_ENC1_PORT_CS_VAR_MACSEC;
                enc_5_cpp = DBAL_ENUM_FVAL_ETPP_ENC5_PORT_CS_VAR_MACSEC;
            }
            else
            {
                forward_cs = DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_ETHERNET_TYPICAL;
                enc_1_cpp = DBAL_ENUM_FVAL_ETPP_ENC1_PORT_CS_VAR_ETHERNET_TYPICAL;
                enc_5_cpp = DBAL_ENUM_FVAL_ETPP_ENC5_PORT_CS_VAR_ETHERNET_TYPICAL;
            }

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                       gport_info.internal_port_pp_info.pp_port[pp_port_index]);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                       gport_info.internal_port_pp_info.core_id[pp_port_index]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_CONTEXT_PORT_PROFILE, INST_SINGLE,
                                         forward_cs);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENC_1_CONTEXT_PORT_PROFILE, INST_SINGLE,
                                         enc_1_cpp);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENC_5_CONTEXT_PORT_PROFILE, INST_SINGLE,
                                         enc_5_cpp);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_switch_svtag_port_set(
    int unit,
    bcm_gport_t gport,
    uint8 enable)
{
    bcm_port_t local_port;
    SHR_FUNC_INIT_VARS(unit);

    /** verify input */
    SHR_IF_ERR_EXIT(dnx_switch_svtag_port_set_verify(unit, gport, enable));

    SHR_IF_ERR_EXIT(dnx_switch_svtag_macsec_set(unit, gport, enable));
    
    if (dnx_data_macsec.general.is_macsec_enabled_get(unit))
    {
        /** Disable or enable the svtag swap */
        if (BCM_GPORT_IS_TRUNK(gport))
        {
            /** If this is a trunk each member PTC should be updated */
            int core;
            int trunk_id = BCM_GPORT_TRUNK_GET(gport);
            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
            {
                bcm_pbmp_t local_port_pbmp_per_core;
                BCM_PBMP_CLEAR(local_port_pbmp_per_core);
                SHR_IF_ERR_EXIT(dnx_trunk_local_port_bmp_get(unit, trunk_id, core, &local_port_pbmp_per_core));
                BCM_PBMP_ITER(local_port_pbmp_per_core, local_port)
                {
                    SHR_IF_ERR_EXIT(dnx_switch_svtag_local_port_enable(unit, local_port, enable));

                }
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_switch_svtag_local_port_enable
                            (unit, (BCM_GPORT_IS_LOCAL(gport) ? BCM_GPORT_LOCAL_GET(gport) : gport), enable));
        }

        /** add the svtag size to the estimation of the editing size */
        SHR_IF_ERR_EXIT(dnx_port_pp_estimate_bta_set(unit, gport,
                                                     ((enable ==
                                                       TRUE) ? BITS2BYTES(dnx_data_switch.
                                                                          svtag.svtag_label_size_bits_get(unit)) : 0)));
    }
exit:
    SHR_FUNC_EXIT;

}
/*
 * }
 */

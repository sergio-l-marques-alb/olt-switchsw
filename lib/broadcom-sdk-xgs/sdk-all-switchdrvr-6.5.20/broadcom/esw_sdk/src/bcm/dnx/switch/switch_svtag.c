/**
 * \file switch_svtag.c
 *
 *
 * Purpose:
 *   Holds all the SVTAG related configuration.
 */
/*
 * $Copyright: (c) 2020 Broadcom.
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
#include <shared/bslenum.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/switch/switch_svtag.h>
#include <shared/shrextend/shrextend_error.h>
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

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_TUNNEL(gport))
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
         * This is a TM port
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESEM_PORT_SVTAG, entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_ESEM_PORT_SVTAG_ETPS_SVTAG);

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, gport);
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

        if (!BCM_GPORT_IS_TUNNEL(gport) && !is_physical_port)
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
    uint8 is_physical_port;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Common verification for the SVTAG APIs
     */
    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_common_verify(unit, flags, gport));

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

    if ((svtag_info->offset_addr != 0) && (svtag_info->accumulation))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "offset addr (%d) must be set to 0 when the accumulation is set.",
                     svtag_info->offset_addr);
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

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, gport, &is_physical_port));

    if (is_physical_port && svtag_info->accumulation != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The accumulation field must be set to 0 in case a physical port is used as a key.");
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
        sal_memset(svtag_hw_entry, 0, sizeof(uint32) * SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32);

        SHR_BITCOPY_RANGE(svtag_hw_entry, dnx_data_switch.svtag.egress_svtag_hw_field_position_bits_get(unit), &svtag,
                          0, dnx_data_switch.svtag.svtag_label_size_bits_get(unit));

        SHR_BITCOPY_RANGE(svtag_hw_entry,
                          dnx_data_switch.svtag.egress_svtag_accumulation_indication_hw_field_position_bits_get(unit),
                          (uint32 *) &(svtag_info->accumulation), 0, SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE);

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
        if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_REPLACE))
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            /*
             * In case this is a physical port (gport can only be a tunnel or a port in this case)
             * add matching ESEM CMD to it in order invoke the SVTAG lookup
             */
            if (!BCM_GPORT_IS_TUNNEL(gport))
            {
                SHR_IF_ERR_EXIT(dnx_port_match_svtag_esem_cmd_set(unit, gport, TRUE));
            }
        }
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
        { DBAL_FIELD_TM_PORT, DBAL_FIELD_ESEM_LOCAL_OUT_LIF };
    uint32 svtag_hw_entry[SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32];
    bcm_switch_svtag_egress_info_t svtag_info;
    bcm_gport_t gport;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_traverse_verify(unit, flags, trav_fn));

    /** traverse over all the SVTAG ttables */
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
                    /** Get port */
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_TM_PORT, &port));
                    /** Disable the SVTAG setting in ESEM CMD attached to the port */
                    SHR_IF_ERR_EXIT(dnx_port_match_svtag_esem_cmd_set(unit, port, FALSE));
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
                    gport = svtag_key;
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

/*
 * See .h file
 */
shr_error_e
dnx_switch_svtag_is_present_in_esem_cmd(
    int unit,
    dnx_esem_cmd_data_t * esem_cmd_data,
    uint8 *found)
{
    dnx_esem_access_if_t esem_if;
    dbal_enum_value_field_esem_offset_e esem_entry_offset;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_info_get
                    (unit, 0, ESEM_ACCESS_TYPE_SVTAG, &esem_if, &esem_entry_offset, NULL));

    if (esem_cmd_data->esem[esem_if].valid
        && ((esem_cmd_data->esem[esem_if].app_db_id == DBAL_ENUM_FVAL_ESEM_APP_DB_ID_OUT_LIF)
            || (esem_cmd_data->esem[esem_if].app_db_id == DBAL_ENUM_FVAL_ESEM_APP_DB_ID_OUT_TM_PORT))
        && (esem_cmd_data->esem[esem_if].designated_offset == esem_entry_offset))
    {
        *found = TRUE;
    }
    else
    {
        *found = FALSE;
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
 * verifies the input for dnx_switch_svtag_port_set function
 * the above input is received from the bcm_dnx_switch_control_port_set function
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - gport
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
    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_TRUNK(gport) && !BCM_GPORT_IS_LOCAL(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport (0x%x) type. \n", gport);
    }

    if (BCM_GPORT_IS_LOCAL(gport))
    {
        /** Verify port in range */
        SHR_MAX_VERIFY(BCM_GPORT_LOCAL_GET(gport), dnx_data_port.general.nof_tm_ports_get(unit), _SHR_E_PARAM,
                       "local port %d, is out of range: %d", BCM_GPORT_LOCAL_GET(gport),
                       dnx_data_port.general.nof_tm_ports_get(unit));
    }
    /** Enable verification */
    SHR_BOOL_VERIFY(enable, _SHR_E_PARAM, "\nenable");

exit:
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
    uint32 pp_port_index;
    uint32 ptc;
    uint32 nof_bytes_to_skip_first_header;
    int core_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    
    if (!BCM_GPORT_IS_TRUNK(gport))
    {
        BCM_GPORT_LOCAL_SET(gport, gport);
    }
    /** verify input */
    SHR_IF_ERR_EXIT(dnx_switch_svtag_port_set_verify(unit, gport, enable));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(dnx_port_pp_macsec_set(unit, gport_info.internal_port_pp_info.core_id[pp_port_index],
                                               gport_info.internal_port_pp_info.pp_port[pp_port_index], enable));

    }

    /** Disable or enable the svtag swap */
    if (BCM_GPORT_IS_TRUNK(gport))
    {
        /** If this is a trunk each member PTC should be updated */
        int core;
        int trunk_id = BCM_GPORT_TRUNK_GET(gport);
        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); ++core)
        {
            bcm_pbmp_t local_port_pbmp_per_core;

            BCM_PBMP_CLEAR(local_port_pbmp_per_core);
            SHR_IF_ERR_EXIT(dnx_trunk_local_port_bmp_get(unit, trunk_id, core, &local_port_pbmp_per_core));
            BCM_PBMP_ITER(local_port_pbmp_per_core, local_port)
            {
                SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, local_port, &ptc, &core_id));
                SHR_IF_ERR_EXIT(dnx_switch_svtag_swap_set(unit, ptc, core_id, enable));
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, BCM_GPORT_LOCAL_GET(gport), &ptc, &core_id));
        SHR_IF_ERR_EXIT(dnx_switch_svtag_swap_set(unit, ptc, core_id, enable));
    }

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
    SHR_IF_ERR_EXIT(dnx_port_first_header_size_to_skip_set
                    (unit, BCM_GPORT_IS_TRUNK(gport) ? gport : BCM_GPORT_LOCAL_GET(gport),
                     nof_bytes_to_skip_first_header));

exit:
    SHR_FUNC_EXIT;

}
/*
 * }
 */

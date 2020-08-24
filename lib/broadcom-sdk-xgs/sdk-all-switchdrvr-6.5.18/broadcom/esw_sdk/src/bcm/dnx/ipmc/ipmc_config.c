/** \file ipmc_config.c
 *
 * Handles the control over the IPv6 cascaded multicast entries.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_IPMC

/*
 * Include files
 * {
 */
#include <bcm/ipmc.h>
#include <bcm/types.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/ipmc/ipmc.h>
/*
 * }
 */

#define IPMC_CONFIG_SUPPORTED_FLAGS                  (BCM_IPMC_L2 | BCM_IPMC_REPLACE)
#define IPMC_CONFIG_SUPPORTED_TRAVERSE_FLAGS         (BCM_IPMC_CONFIG_TRAVERSE_DELETE_ALL | BCM_IPMC_L2)

/**
 * \brief
 *   Verify the ipmc (Internet protocol multicast) configuration input.
 * \param [in] unit -
 *   The unit number.
 * \param [in] config -
 *   contains the required information for validating the entry.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_ipmc_config_add_verify
 *  * bcm_dnx_ipmc_config_find
 *  * bcm_dnx_ipmc_config_remove
 */
static shr_error_e
dnx_ipmc_config_verify(
    int unit,
    bcm_ipmc_addr_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    /** Verify that the IPv6 source address is not multicast */
    if (IPMC_IS_IPV6_ADDRESS_MULTICAST(config->s_ip6_addr, config->s_ip6_mask))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "IPv6 source address is multicast - expecting the first byte not to be 0xFF");
    }
    /** Verify that the ETH-RIF value is valid. */
    if (config->vid >= dnx_data_l3.rif.nof_rifs_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal ETH-RIF (vid) value %d, should be positive and less than %d.",
                     config->vid, dnx_data_l3.rif.nof_rifs_get(unit));
    }
    /** Verify supported flags */
    if (_SHR_IS_FLAG_SET(config->flags, ~IPMC_CONFIG_SUPPORTED_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flag/s is provided to the bcm_ipmc_config_* API\n");
    }
    /** Verify that if L2 entry is specified, VID field is not set. */
    if (_SHR_IS_FLAG_SET(config->flags, BCM_IPMC_L2) && config->vid != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_IPMC_L2 flag is provided, indicating compressed SIP entry, but VID field is not 0.\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify the ipmc (Internet protocol multicast) configuration input.
 * \param [in] unit -
 *   The unit number.
 * \param [in] config -
 *   contains the required information for validating the entry.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_ipmc_config_add
 */
static shr_error_e
dnx_ipmc_config_add_verify(
    int unit,
    bcm_ipmc_addr_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the compressed SIP value */
    if (config->s_ip_addr > dnx_data_l3.fwd.nof_compressed_svl_sip_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Compressed SIP value is out of range. Valid range is between 0 and %d\n",
                     dnx_data_l3.fwd.nof_compressed_svl_sip_get(unit));
    }
    SHR_IF_ERR_EXIT(dnx_ipmc_config_verify(unit, config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Create a new IPMC configuration entry
 * \param [in] unit -
 *   The unit number.
 * \param [in] config -
 *   structure that contains the information for the new entry.
 *      * s_ip6_addr - Source IP address used in cascaded lookup
 *      * s_ip6_mask - Mask of the source IP address
 *      * vid - Ingress router interface value
 *      * s_ip_addr - Compressed Source/Source_and_Interface value.
 *      * flags - control flags
 *              * BCM_IPMC_L2 - indicates a cascaded entry used in MC bridge.
 *              If the flag is set a compressed SIP entry will be created, otherwise a compressed Source_and_Interface.
 *              * BCM_IPMC_REPLACE - indicates that an existing entry will be updated.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_ipmc_config_add(
    int unit,
    bcm_ipmc_addr_t * config)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table;
    dbal_entry_action_flags_e dbal_action;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ipmc_config_add_verify(unit, config));

    /** Choose DBAL table to which the entry will be added */
    if (!_SHR_IS_FLAG_SET(config->flags, BCM_IPMC_L2))
    {
        dbal_table = DBAL_TABLE_IPV6_MULTICAST_SOURCE_AND_INTERFACE;
    }
    else
    {
        dbal_table = DBAL_TABLE_IPV6_MULTICAST_SOURCE;
    }
    if (_SHR_IS_FLAG_SET(config->flags, BCM_IPMC_REPLACE))
    {
        dbal_action = DBAL_COMMIT_UPDATE;
    }
    else
    {
        dbal_action = DBAL_COMMIT;
    }

    /** Create an entry handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6, config->s_ip6_addr,
                                         config->s_ip6_mask);
    if (config->vid != 0 && dbal_table == DBAL_TABLE_IPV6_MULTICAST_SOURCE_AND_INTERFACE)
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, config->vid,
                                          IPMC_GET_IN_LIF_MASK(config->vid));
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT, INST_SINGLE, config->s_ip_addr);
    /** Commit entry to the table. */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_action));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Find an existing IPMC configuration entry
 * \param [in] unit -
 *   The unit number.
 * \param [in,out] config -
 *   structure that contains the information for the entry.
 *      * s_ip6_addr - Source IP address used in cascaded lookup
 *      * s_ip6_mask - Mask of the source IP address
 *      * vid - Ingress router interface value
 *      * s_ip_addr - Compressed Source/Source_and_Interface value, returned by the API.
 *      * flags - control flags
 *              * BCM_IPMC_L2 - indicates a cascaded entry used in MC bridge.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_ipmc_config_find(
    int unit,
    bcm_ipmc_addr_t * config)
{
    dbal_tables_e dbal_table;
    uint32 entry_handle_id;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_ipmc_config_verify(unit, config));
    /** Choose a dbal table in which the entry is going to be searched for. */
    if (!_SHR_IS_FLAG_SET(config->flags, BCM_IPMC_L2))
    {
        dbal_table = DBAL_TABLE_IPV6_MULTICAST_SOURCE_AND_INTERFACE;
    }
    else
    {
        dbal_table = DBAL_TABLE_IPV6_MULTICAST_SOURCE;
    }
    /** Create an entry handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6, config->s_ip6_addr,
                                         config->s_ip6_mask);
    if (config->vid != 0 && dbal_table == DBAL_TABLE_IPV6_MULTICAST_SOURCE_AND_INTERFACE)
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, config->vid,
                                          IPMC_GET_IN_LIF_MASK(config->vid));
    }
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT, INST_SINGLE, &config->s_ip_addr);
    /** Get the result value of the entry */
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Delete an existing IPMC configuration entry
 * \param [in] unit -
 *   The unit number.
 * \param [in] config -
 *   structure that contains the information for the entry.
 *      * s_ip6_addr - Source IP address used in cascaded lookup
 *      * s_ip6_mask - Mask of the source IP address
 *      * vid - Ingress router interface value
 *      * flags - control flags
 *              * BCM_IPMC_L2 - indicates a cascaded entry used in MC bridge.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_ipmc_config_remove(
    int unit,
    bcm_ipmc_addr_t * config)
{
    dbal_tables_e dbal_table;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_ipmc_config_verify(unit, config));
    /** Choose DBAL table from which the entry is going to be deleted */
    if (!_SHR_IS_FLAG_SET(config->flags, BCM_IPMC_L2))
    {
        dbal_table = DBAL_TABLE_IPV6_MULTICAST_SOURCE_AND_INTERFACE;
    }
    else
    {
        dbal_table = DBAL_TABLE_IPV6_MULTICAST_SOURCE;
    }
    /** Create an entry handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, DBAL_FIELD_IPV6, config->s_ip6_addr,
                                         config->s_ip6_mask);
    if (config->vid != 0 && dbal_table == DBAL_TABLE_IPV6_MULTICAST_SOURCE_AND_INTERFACE)
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, config->vid,
                                          IPMC_GET_IN_LIF_MASK(config->vid));
    }
    /** clearing the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Traverse all IPMC configuration entries
 * \param [in] unit -
 *   The unit number.
 * \param [in] flags - control flags
 *      * BCM_IPMC_CONFIG_TRAVERSE_DELETE_ALL - indicates that all tables will be cleared
 * \param [in] cb - reference to the callback function
 * \param [in] user_data - User data passed to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
int
bcm_dnx_ipmc_config_traverse(
    int unit,
    uint32 flags,
    bcm_ipmc_config_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 dbal_table;
    uint32 field_value;
    bcm_ipmc_addr_t dbal_entry;
    int is_end;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);
    if (_SHR_IS_FLAG_SET(flags, ~IPMC_CONFIG_SUPPORTED_TRAVERSE_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flag/s is provided to the bcm_ipmc_config_traverse API\n");
    }
    /** Choose DBAL table which is going to be cleared or traversed */
    if (!_SHR_IS_FLAG_SET(flags, BCM_IPMC_L2))
    {
        dbal_table = DBAL_TABLE_IPV6_MULTICAST_SOURCE_AND_INTERFACE;
    }
    else
    {
        dbal_table = DBAL_TABLE_IPV6_MULTICAST_SOURCE;
    }

    /** If BCM_IPMC_CONFIG_TRAVERSE_DELETE_ALL flag is provided, then table will be cleared instead of traversed */
    if (_SHR_IS_FLAG_SET(flags, BCM_IPMC_CONFIG_TRAVERSE_DELETE_ALL))
    {
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, dbal_table));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

        /** Receive first entry in table. */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            bcm_ipmc_addr_t_init(&dbal_entry);
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_masked_get
                            (unit, entry_handle_id, DBAL_FIELD_IPV6, dbal_entry.s_ip6_addr, dbal_entry.s_ip6_mask));

            if (dbal_table == DBAL_TABLE_IPV6_MULTICAST_SOURCE_AND_INTERFACE)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, &field_value));
                dbal_entry.vid = field_value;
            }
            else
            {
                dbal_entry.flags |= BCM_IPMC_L2;
            }
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT,
                                                                    INST_SINGLE, &dbal_entry.s_ip_addr));
            /** Invoke callback function */
            SHR_IF_ERR_EXIT((*cb) (unit, &dbal_entry, user_data));
            /** Receive next entry in table. */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

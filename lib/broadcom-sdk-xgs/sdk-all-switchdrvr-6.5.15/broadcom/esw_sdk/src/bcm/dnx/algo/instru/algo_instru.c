/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/** \file algo_instru.c
 *
 * Wrapper functions for utilex_multi_set.
 *
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/util.h>
#include <bcm_int/dnx/algo/instru/algo_instru.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_instru.h>
#include <soc/dnx/swstate/auto_generated/access/instru_access.h>

/**
 * }
 */

#define DNX_ALGO_INSTRU_UDP_TUNNEL_PROFILE "sFlow UDP tunnel profile"

void
dnx_algo_instru_sflow_destination_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    sflow_destination_key_t *destination_key = (sflow_destination_key_t *) data;

    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "UDP tunnel", destination_key->udp_tunnel,
                                        NULL, "0x%x");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "Sub agent id", destination_key->sub_agent_id,
                                        NULL, "0x%x");

    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
}

/**
 * \brief - 
 * Create Template instru UDP tunnel. Each profile only uses
 *  
 * \param [in] unit - unit id
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_instru_sflow_destination_profile_create(
    int unit)
{
    sw_state_algo_template_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set a template for vsi profiles
     */
    sal_memset(&data, 0, sizeof(data));

    data.data_size = sizeof(sflow_destination_key_t);
    data.first_profile = 0;
    data.flags = 0;
    /*
     * At most one profile per encap, at most all encaps on one profile.
     */
    data.max_references = dnx_data_instru.sflow.max_nof_sflow_encaps_get(unit);
    data.nof_profiles = dnx_data_instru.sflow.max_nof_sflow_encaps_get(unit);

    sal_strncpy(data.name, DNX_ALGO_INSTRU_UDP_TUNNEL_PROFILE, SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_destination_profile.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_instru_encap_to_sflow_destination_profile_init(
    int unit)
{
    sw_state_htbl_init_info_t htb_init_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Map from encap to UDP tunnel profile.
     */
    sal_memset(&htb_init_info, 0, sizeof(sw_state_htbl_init_info_t));
    htb_init_info.max_nof_elements = dnx_data_instru.sflow.max_nof_sflow_encaps_get(unit);
    htb_init_info.expected_nof_elements = dnx_data_instru.sflow.max_nof_sflow_encaps_get(unit);
    htb_init_info.hash_function = NULL;

    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_encap_to_sflow_destination_profile.create(unit, &htb_init_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_instru_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init instru sw state 
     */
    SHR_IF_ERR_EXIT(instru.init(unit));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.alloc(unit));

    /*
     * Initialize the UDP tunnel profile template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_instru_sflow_destination_profile_create(unit));

    /*
     * Initialize the encap to UDP tunnel profile hash.
     */
    SHR_IF_ERR_EXIT(dnx_algo_instru_encap_to_sflow_destination_profile_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_instru_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

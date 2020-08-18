/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/** \file algo_out_lif_profile.c
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
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <bcm_int/dnx/algo/lif_mngr/algo_out_lif_profile.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_profile_access.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>

/**
 * \see
 *   algo_out_lif_profile.h file
 */
void
dnx_algo_out_lif_profile_print_entry_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    out_lif_template_data_t *template_data = (out_lif_template_data_t *) data;

    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "out lif orientation",
                                        template_data->out_lif_orientation, NULL, "0x%08x");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "split horizon enable",
                                        template_data->etpp_split_horizon_enable, NULL, "0x%08x");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "oam lif profile",
                                        template_data->oam_lif_profile, NULL, "0x%08x");
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
}

/**
 * \brief
 *  Create Template Manager for out_lif_profile, the template is created with the following properties:
 *  entries : each entry represents OUT-LIF properties, more than one out_lif_profile can point to it.
 *  key : the key represents OUT_LIF_PROFILE
 *  the template has a default profile which will always exist in index 0.
 *  max references per profile > number of keys,this is in order to always keep the default profile.
 *  this way no matter how many template exchanges we will do, the default profile won't reach 0 references.
 *  according to the user's flags the profile will be decided and allocated
 *  (in case it doesnt exist already).
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_out_lif_profile_template_create(
    int unit)
{

    out_lif_template_data_orientation_t default_template_data_orientation;
    out_lif_template_data_oam_profile_t default_template_data_oam_profile;
    out_lif_template_data_t default_template_data;
    sw_state_algo_template_create_data_t data;
    uint32 ip_orientation_max_val = 0;
    uint32 ip_orientation_min_val = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_EEDB_IPV4_TUNNEL,
                                                          DBAL_FIELD_OUT_LIF_PROFILE,
                                                          FALSE,
                                                          DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL,
                                                          0, DBAL_PREDEF_VAL_MAX_VALUE, &ip_orientation_max_val));
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_EEDB_IPV4_TUNNEL,
                                                          DBAL_FIELD_OUT_LIF_PROFILE,
                                                          FALSE,
                                                          DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL,
                                                          0, DBAL_PREDEF_VAL_MIN_VALUE, &ip_orientation_min_val));
    /*
     * initialize the default template data with default values for orientation
     */
    sal_memset(&default_template_data_orientation, 0, sizeof(default_template_data_orientation));
    default_template_data_orientation.out_lif_orientation = DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION;
    sal_memset(&data, 0, sizeof(sw_state_algo_template_create_data_t));
    data.data_size = sizeof(default_template_data_orientation);
    data.default_data = &default_template_data_orientation;
    data.default_profile = ip_orientation_min_val;
    data.first_profile = ip_orientation_min_val;
    data.flags = SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data.nof_profiles = (ip_orientation_max_val - ip_orientation_min_val) + 1;
    data.max_references = dnx_data_lif.out_lif.nof_local_out_lifs_get(unit);
    sal_strncpy(data.name, DNX_ALGO_OUT_LIF_PROFILE, SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_lif_profile_db.out_lif_profile_orientation.create(unit, &data, NULL));
    /*
     * initialize the default template data with default values for oam profile
     */
    sal_memset(&default_template_data_oam_profile, 0, sizeof(default_template_data_oam_profile));
    default_template_data_oam_profile.oam_lif_profile = DNX_OUT_LIF_PROFILE_OAM_PROFILE;
    sal_memset(&data, 0, sizeof(sw_state_algo_template_create_data_t));
    data.data_size = sizeof(default_template_data_oam_profile);
    data.default_data = &default_template_data_oam_profile;
    data.default_profile = 0;
    data.first_profile = 0;
    data.flags = SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data.nof_profiles = 4;
    data.max_references = dnx_data_lif.out_lif.nof_local_out_lifs_get(unit);
    sal_strncpy(data.name, DNX_ALGO_OUT_LIF_PROFILE, SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_lif_profile_db.out_lif_profile_oam_profile.create(unit, &data, NULL));
    /*
     * initialize the default template data with default values
     */
    sal_memset(&default_template_data, 0, sizeof(default_template_data));
    default_template_data.out_lif_orientation = DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION;
    default_template_data.etpp_split_horizon_enable = DNX_OUT_LIF_PROFILE_SPLIT_HORIZON_ENABLE;
    default_template_data.oam_lif_profile = DNX_OUT_LIF_PROFILE_OAM_PROFILE;
    /*
     * initialize the templates data/properties
     */
    sal_memset(&data, 0, sizeof(sw_state_algo_template_create_data_t));
    data.data_size = sizeof(default_template_data);
    data.default_data = &default_template_data;
    data.default_profile = 0;
    data.first_profile = 0;
    data.flags = SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data.nof_profiles = 55;
    data.max_references = dnx_data_lif.out_lif.nof_local_out_lifs_get(unit);
    sal_strncpy(data.name, DNX_ALGO_OUT_LIF_PROFILE, SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_lif_profile_db.out_lif_profile.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   algo_out_lif_profile.h file
 */
shr_error_e
dnx_algo_out_lif_profile_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize out_lif_profile template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_out_lif_profile_template_create(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   algo_out_lif_profile.h file
 */
shr_error_e
dnx_algo_out_lif_profile_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 */

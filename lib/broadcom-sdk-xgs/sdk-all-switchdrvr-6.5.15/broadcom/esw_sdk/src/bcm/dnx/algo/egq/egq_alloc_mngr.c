/**
 * \file algo/egq/egq_alloc_mngr.c
 *
 * Implementation for advanced egq resource manager.
 *
 * Manage flows allocation.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/**
* Include files:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_legacy_am_access.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <bcm_int/dnx/algo/res_mngr/simple_bitmap_wrap.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/algo/egq/egq_alloc_mngr.h>

/** 
 * } 
 */

/**
* Structs:
* {
*/

/** 
 * } 
 */

/**
* Static Functions:
* {
*/

/** 
 * } 
 */

/**
* Functions:
* {
*/
/*
 * Cosq Egress Queue Mapping - Start
 * {
 */
int
dnx_am_template_egress_queue_mapping_create(
    int unit,
    int template_init_id,
    dnx_cosq_egress_queue_mapping_info_t * mapping_profile)
{

    sw_state_algo_template_create_data_t create_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize the entire legacy alloc manager module.
     */
    SHR_IF_ERR_EXIT(dnx_legacy_am_db.init(unit));
    /*
     * Create template manager insance - handle: DNX_ALGO_TEMPLATE_EGR_QUEUE_MAPPING
     */
    sal_memset(&create_info, 0, sizeof(sw_state_algo_template_create_data_t));
    create_info.flags =
        (SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE | SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE);
    create_info.first_profile = DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_LOW_ID;
    create_info.nof_profiles = DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_COUNT;
    create_info.max_references = DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_MAX_ENTITIES(unit);
    create_info.default_profile = template_init_id;
    create_info.data_size = DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_SIZE;
    create_info.default_data = mapping_profile;
    sal_strncpy(create_info.name, DNX_ALGO_TEMPLATE_EGR_TC_DP_MAPPING, SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_legacy_am_db.egr_tc_dp_mapping.create(unit, &create_info, NULL));

exit:
    SHR_FUNC_EXIT;
}

int
dnx_am_template_egress_queue_mapping_data_get(
    int unit,
    uint32 tm_port,
    int core,
    dnx_cosq_egress_queue_mapping_info_t * mapping_profile)
{
    int index, ref_count;
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    rc = dnx_egr_q_profile_map_get(unit, core, tm_port, (uint32 *) &index);
    SHR_IF_ERR_EXIT(rc);

    if (index < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Failed to get the old profile");
    }

    rc = dnx_legacy_am_db.egr_tc_dp_mapping.profile_data_get(unit, core, index, &ref_count, mapping_profile);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}
int
dnx_am_template_egress_queue_mapping_exchange(
    int unit,
    uint32 tm_port,
    int core,
    const dnx_cosq_egress_queue_mapping_info_t * mapping_profile,
    int *old_profile,
    int *is_last,
    int *new_profile,
    int *is_allocated)
{
    int rc = _SHR_E_NONE;
    int tmp_old_profile;
    uint8 tmp_is_last = 0;
    uint8 tmp_is_alloc = 0;

    SHR_FUNC_INIT_VARS(unit);

    rc = dnx_egr_q_profile_map_get(unit, core, tm_port, (uint32 *) &tmp_old_profile);
    SHR_IF_ERR_EXIT(rc);

    rc = dnx_legacy_am_db.egr_tc_dp_mapping.exchange
        (unit, core, 0, mapping_profile, tmp_old_profile, NULL, new_profile, &tmp_is_alloc, &tmp_is_last);
    SHR_IF_ERR_EXIT(rc);

    if (old_profile != NULL)
    {
        *old_profile = tmp_old_profile;
    }

    if (is_last != NULL)
    {
        *is_last = tmp_is_last ? 1 : 0;
    }

    if (is_allocated != NULL)
    {
        *is_allocated = tmp_is_alloc ? 1 : 0;
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_alloc_mngr.h file.
 */
void
dnx_algo_egr_tc_dp_mapping_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_cosq_egress_queue_mapping_info_t *template_data = (dnx_cosq_egress_queue_mapping_info_t *) data;
    uint32 i, j, k;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    for (i = 0; i < DNX_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAPPING_TYPES; i++)
    {
        DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "multicast", i, NULL, "%u");
        for (j = 0; j < DNX_DEVICE_COSQ_ING_NOF_TC; j++)
        {
            DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "tc", j, NULL, "%u");
            for (k = 0; k < DNX_DEVICE_COSQ_ING_NOF_DP; k++)
            {
                DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "dp", k, NULL, "%u");
                DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "tc_map",
                                                    template_data->queue_mapping[i][j][k].tc, NULL, "%u");
                DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "dp_map",
                                                    template_data->queue_mapping[i][j][k].dp, NULL, "%u");
            }
        }
    }
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);

    return;
}

/*
 * Cosq Egress Queue Mapping - End
 * }
 */
/**
 * }
 */

/** \file cosq_latency.c
 * $Id$
 *
 * General cosq latency functionality \n
 * relevant for ingress and egress (end to end) latency
 * 
 * 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

/*
 * Include files.
 * {
 */
#include <bcm/cosq.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/cosq/cosq_latency.h>
#include <include/soc/dnx/swstate/auto_generated/access/cosq_latency_access.h>
#include <bcm_int/dnx/algo/res_mngr/simple_bitmap_wrap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/stat/stat_pp.h>

/*
 * }
 */

/*
 * Defines
 * {
 */
#define DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION (4)

/*
 * }
 */

/** See .h file */
void
dnx_cosq_latency_queue_template_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_cosq_latency_queue_template_t *profile_info = (dnx_cosq_latency_queue_template_t *) data;

    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_STRING, NULL, "latency queue profile info:", NULL,
                                        NULL);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "latency_map", profile_info->latency_map, NULL,
                                        "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "tc_map", profile_info->tc_map, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
    return;
}

/**
* \brief
* verification function for latency profile create API
*/
static shr_error_e
dnx_cosq_latency_profile_create_verify(
    int unit,
    uint32 flags,
    int *gport_profile)
{
    uint32 legal_flags =
        BCM_COSQ_LATENCY_PROFILE_WITH_ID | BCM_COSQ_LATENCY_PROFILE_ID_EXTENSION | BCM_COSQ_LATENCY_END_TO_END;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(gport_profile, _SHR_E_PARAM, "gport_profile");
    SHR_MASK_VERIFY(flags, legal_flags, _SHR_E_PARAM, "some of the flags are not supported.\n");

    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_PROFILE_WITH_ID))
    {
        if ((BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(*gport_profile) == FALSE) &&
            (BCM_GPORT_IS_END_TO_END_LATENCY_PROFILE(*gport_profile) == FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport_profile %d", *gport_profile);
        }
        if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_END_TO_END) &&
            BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(*gport_profile))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "flag BCM_COSQ_LATENCY_END_TO_END, but gport (=%d) is decoded as ingress latency",
                         *gport_profile);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  create a ingress latency profile or end to end latency profile.
* \param [in] unit -unit id
* \param [in] flags - BCM_COSQ_LATENCY_PROFILE_WITH_ID, BCM_COSQ_LATENCY_PROFILE_ID_EXTENSION
* \param [out] profile_gport - gport of the latency profile
* \return
*    shr_error_e
* \remark
*   None
* \see
*   NONE
*/
int
bcm_dnx_cosq_latency_profile_create(
    int unit,
    int flags,
    bcm_gport_t * profile_gport)
{
    uint32 res_flags = 0;
    int profile_id = 0;
    simple_bitmap_wrap_alloc_info_t alloc_info;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_latency_profile_create_verify(unit, flags, profile_gport));

    if (!_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_END_TO_END))
    {
        sal_memset(&alloc_info, 0x0, sizeof(simple_bitmap_wrap_alloc_info_t));
        if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_PROFILE_WITH_ID))
        {
            profile_id = BCM_GPORT_INGRESS_LATENCY_PROFILE_GET(*profile_gport);
            res_flags |= SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;
        }
        res_flags |= DNX_ALGO_RES_SIMPLE_BITMAP_ALWAYS_CHECK_TAG;

        /** if allocate can be made from extension range, try it first, and if it is full, try the not extension range */
        if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_PROFILE_ID_EXTENSION))
        {
            alloc_info.tag = 1;
            rv = dnx_cosq_latency_db.ingress_latency_res.allocate_single(unit, res_flags, (void *) &alloc_info,
                                                                         &profile_id);
            if (_SHR_E_NONE == rv)
            {
                BCM_GPORT_INGRESS_LATENCY_PROFILE_SET((*profile_gport), profile_id);
                SHR_EXIT();
            }
            else if (_SHR_E_RESOURCE != rv)
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        alloc_info.tag = 0;
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.ingress_latency_res.allocate_single
                        (unit, res_flags, (void *) &alloc_info, &profile_id));
        BCM_GPORT_INGRESS_LATENCY_PROFILE_SET((*profile_gport), profile_id);

    }
    else
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_PROFILE_WITH_ID))
        {
            profile_id = BCM_GPORT_END_TO_END_LATENCY_PROFILE_GET(*profile_gport);
            res_flags |= SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;
        }
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.end_to_end_latency_res.allocate_single(unit, res_flags, NULL, &profile_id));
        BCM_GPORT_END_TO_END_LATENCY_PROFILE_SET((*profile_gport), profile_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  destroy a ingress latency profile or end to end latency profile.
* \param [in] unit -unit id
* \param [in] flags - NONE
* \param [in] profile_gport - gport of the latency profile
* \return
*    shr_error_e
* \remark
*   None
* \see
*   NONE
*/
int
bcm_dnx_cosq_latency_profile_destroy(
    int unit,
    int flags,
    bcm_gport_t profile_gport)
{
    int profile_id;
    SHR_FUNC_INIT_VARS(unit);

    /** free from resource manager the profile */
    if (BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(profile_gport))
    {
        profile_id = BCM_GPORT_INGRESS_LATENCY_PROFILE_GET(profile_gport);
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.ingress_latency_res.free_single(unit, profile_id));
    }
    else if (BCM_GPORT_IS_END_TO_END_LATENCY_PROFILE(profile_gport))
    {
        profile_id = BCM_GPORT_END_TO_END_LATENCY_PROFILE_GET(profile_gport);
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.end_to_end_latency_res.free_single(unit, profile_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport_profile %d", profile_gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 *  init resource manager for latency measurements
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_latency_res_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    dbal_table_field_info_t field_info;
    simple_bitmap_wrap_create_info_t extra_info;
    simple_bitmap_wrap_tag_info_t tag_info;
    bcm_gport_t gport;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&extra_info, 0x0, sizeof(simple_bitmap_wrap_create_info_t));
    sal_memset(&tag_info, 0x0, sizeof(simple_bitmap_wrap_tag_info_t));

    /** create ingress latency res mngr */
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_LATENCY_INGRESS_PROFILE, DBAL_FIELD_PROFILE_ID,
                                               TRUE, 0, 0, &field_info));
    sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));
    data.first_element = 0;
    data.nof_elements = field_info.max_value + 1;
    data.flags = 0;
    sal_strncpy(data.name, DNX_COSQ_LATENCY_INGRESS_RESOURCE, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    /** create tag for half of the entries. */
    /** pmf-flow-profile can be mapped just to the first half. queues can be mapped to all range. */
    extra_info.grain_size = data.nof_elements / 2;
    extra_info.max_tag_value = 1;
    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.ingress_latency_res.create(unit, &data, (void *) &extra_info));
    tag_info.element = 0;
    tag_info.tag = 0;
    tag_info.nof_elements = extra_info.grain_size;
    tag_info.force_tag = FALSE;
    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.ingress_latency_res.advanced_algorithm_info_set(unit, (void *) &tag_info));
    tag_info.element = extra_info.grain_size;
    tag_info.nof_elements = extra_info.grain_size;
    tag_info.tag = 1;
    tag_info.force_tag = FALSE;
    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.ingress_latency_res.advanced_algorithm_info_set(unit, (void *) &tag_info));

    /** capture the first profile as default profile that do not perform any latency action */
    BCM_GPORT_INGRESS_LATENCY_PROFILE_SET(gport, 0);
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_latency_profile_create(unit, BCM_COSQ_LATENCY_PROFILE_WITH_ID, &gport));

    /** create end to end latency res mngr */
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_LATENCY_EGRESS_PROFILE, DBAL_FIELD_PROFILE_ID,
                                               TRUE, 0, 0, &field_info));
    sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));
    data.first_element = 0;
    data.nof_elements = field_info.max_value + 1;
    data.flags = 0;
    sal_strncpy(data.name, DNX_COSQ_LATENCY_END_TO_END_RESOURCE, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.end_to_end_latency_res.create(unit, &data, NULL));
    /** capture the first profile as default profile that do not perform any latency action */
    BCM_GPORT_END_TO_END_LATENCY_PROFILE_SET(gport, 0);
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_latency_profile_create
                    (unit, (BCM_COSQ_LATENCY_PROFILE_WITH_ID | BCM_COSQ_LATENCY_END_TO_END), &gport));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_latency_queue_tm_init(
    int unit)
{
    sw_state_algo_template_create_data_t data;
    int nof_profiles;
    dbal_table_field_info_t field_info;
    dnx_cosq_latency_queue_template_t default_data = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /** get data on the profiles size etc.. from HW dbal tables */
    /** note - ingress and egress has the same hw configuration in HW. we take the data from ingress, but it belong also to egress */
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_LATENCY_VOQ_QUARTET_PROFILE, DBAL_FIELD_PROFILE_ID,
                                               FALSE, 0, 0, &field_info));
    nof_profiles = field_info.max_value + 1;

    /** Add all entities to template init id with given data */
    sal_memset(&data, 0, sizeof(sw_state_algo_template_create_data_t));
    data.flags = SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE |
        SW_STATE_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE;
    data.first_profile = 0;
    data.nof_profiles = nof_profiles;
    data.max_references = dnx_data_ipq.queues.nof_queues_get(unit) / DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION; /** quartet queues profile */
    data.default_profile = 0;
    data.data_size = sizeof(dnx_cosq_latency_queue_template_t);
    data.default_data = (void *) &default_data;

    /** create template manager */
    sal_strncpy(data.name, DNX_COSQ_LATENCY_QUARTET_QUEUES_TEMPLATE_NAME,
                SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.queue_template.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_cosq_latency_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.init(unit));

    /** init resource manager latency */
    SHR_IF_ERR_EXIT(dnx_cosq_latency_res_init(unit));
    /** init template manager queues latency */
    SHR_IF_ERR_EXIT(dnx_cosq_latency_queue_tm_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_cosq_latency_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit 
     */

    SHR_FUNC_EXIT;
}

/**
* \brief
*     get a report that contain the packets with the max ingress latency.
* \param [in] unit -unit id
* \param [in] gport - indicates on the core id.
* \param [in] max_count - max entries to fill - indicates the size of array "max_latency_pkts"
* \param [out] max_latency_pkts - array of information of the max latency packets.
* \param [out] actual_count - actual size filled by the driver into max_latency_pkts.
*
* \return
*    shr_error_e
* \remark
*   None
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_latency_ingress_max_get(
    int unit,
    bcm_gport_t gport,
    int max_count,
    bcm_cosq_max_latency_pkts_t * max_latency_pkts,
    int *actual_count)
{
    int index;
    uint32 entry_handle_id;
    dbal_table_field_info_t field_info;
    uint32 tc;
    uint32 tm_port;
    uint32 modid;
    uint32 lat_val;
    uint32 flow_id;
    bcm_core_t core_id = _SHR_COSQ_GPORT_CORE_GET(gport);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_LATENCY_INGRESS_MAX_REPORT, DBAL_FIELD_INDEX,
                                               TRUE, 0, 0, &field_info));
    *actual_count = 0;

    /** Table always sorted. Read entire table. HW freeze table when reading entry zero and un-freeze when reading entry 7 */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_INGRESS_MAX_REPORT, &entry_handle_id));

    /** go over all max latency table, read the content and fill the structure */
    for (index = 0; index <= field_info.max_value; index++)
    {
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_VALUE, INST_SINGLE, &lat_val);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_FLOW_ID, INST_SINGLE, &flow_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_TRF_CLS, INST_SINGLE, &tc);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_PORT_ID, INST_SINGLE, &tm_port);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_MODID, INST_SINGLE, &modid);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if ((*actual_count) < max_count)
        {
            max_latency_pkts[(*actual_count)].latency = lat_val;
            max_latency_pkts[(*actual_count)].latency_flow = flow_id;
            max_latency_pkts[(*actual_count)].cosq = (int) tc;
            BCM_GPORT_MODPORT_SET(max_latency_pkts[(*actual_count)].dest_gport, modid, tm_port);
            /** fill the array only if latency > 0. otherwise, it is consider as empty entry */
            if (max_latency_pkts[(*actual_count)].latency > 0)
            {
                (*actual_count)++;
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*     get a report that contain the packets with the max end to end latency. the report is taken from Egress pipe.
* \param [in] unit -unit id
* \param [in] gport - indicates on the core id.
* \param [in] max_count - max entries to fill - indicates the size of array "max_latency_pkts"
* \param [out] max_latency_pkts - array of information of the max latency packets.
* \param [out] actual_count - actual size filled by the driver into max_latency_pkts.
*
* \return
*    shr_error_e
* \remark
*   None
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_latency_egress_max_get(
    int unit,
    bcm_gport_t gport,
    int max_count,
    bcm_cosq_max_latency_pkts_t * max_latency_pkts,
    int *actual_count)
{
    int index;
    uint32 entry_handle_id;
    dbal_table_field_info_t field_info;
    uint32 lat_val;
    uint32 tc;
    uint32 tm_port;
    uint32 mc;
    uint32 flow_id;
    bcm_core_t core_id = _SHR_COSQ_GPORT_CORE_GET(gport);
    uint32 modid;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_LATENCY_EGRESS_MAX_REPORT, DBAL_FIELD_INDEX,
                                               TRUE, 0, 0, &field_info));
    *actual_count = 0;
    SHR_IF_ERR_EXIT(dnx_stk_sys_modid_get(unit, core_id, &modid));

    /** Table always sorted. Read entire table. HW freeze table when reading entry zero and un-freeze when reading entry 7 */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_MAX_REPORT, &entry_handle_id));

    /** go over all max latency table, read the content and fill the structure */
    for (index = 0; index <= field_info.max_value; index++)
    {
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_VALUE, INST_SINGLE, &lat_val);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_FLOW_ID, INST_SINGLE, &flow_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_TC, INST_SINGLE, &tc);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_TM_PORT_ID, INST_SINGLE, &tm_port);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_MC, INST_SINGLE, &mc);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if ((*actual_count) < max_count)
        {
            max_latency_pkts[(*actual_count)].latency = lat_val;
            max_latency_pkts[(*actual_count)].latency_flow = flow_id;
            max_latency_pkts[(*actual_count)].cosq = (int) tc;
            BCM_GPORT_MODPORT_SET(max_latency_pkts[(*actual_count)].dest_gport, modid, tm_port);
            /** fill the array only if latency > 0. otherwise, it is consider as empty entry */
            if (max_latency_pkts[(*actual_count)].latency > 0)
            {
                (*actual_count)++;
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* verification function for max latecy get API
*/
static shr_error_e
dnx_cosq_latency_max_pkts_get_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    int max_count,
    bcm_cosq_max_latency_pkts_t * max_latency_pkts,
    int *actual_count)
{
    uint32 legal_flags = BCM_COSQ_LATENCY_END_TO_END;
    int core_id;
    dbal_table_field_info_t field_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(actual_count, _SHR_E_PARAM, "actual_count");
    SHR_NULL_CHECK(max_latency_pkts, _SHR_E_PARAM, "max_latency_pkts");

    SHR_MASK_VERIFY(flags, legal_flags, _SHR_E_PARAM, "some of the flags are not supported.\n");

    if (_SHR_COSQ_GPORT_IS_CORE(gport))
    {
        core_id = _SHR_COSQ_GPORT_CORE_GET(gport);
        /** not support specific core */
        DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport %d", gport);
    }
    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_END_TO_END))
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_LATENCY_INGRESS_MAX_REPORT, DBAL_FIELD_INDEX,
                                                   TRUE, 0, 0, &field_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_LATENCY_EGRESS_MAX_REPORT, DBAL_FIELD_INDEX,
                                                   TRUE, 0, 0, &field_info));
    }
    if (max_count > (field_info.max_value + 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "max_count=%d, allow up to %d", max_count, (field_info.max_value + 1));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     get a report that contain the packets with the max latency.
*     user select one of the follwoing report: ingress latency, end to end latency.
* \param [in] unit -unit id
* \param [in] gport - indicates on the core id.
* \param [in] flags - iBCM_COSQ_LATENCY_END_TO_END: ndicates if the get the report from ing or egr (end to end) table
* \param [in] max_count - max entries to fill - indicates the size of array "max_latency_pkts"
* \param [out] max_latency_pkts - array of information of the max latency packets.
* \param [out] actual_count - actual size filled by the driver into max_latency_pkts.
*
* \return
*    shr_error_e
* \remark
*   None
* \see
*   NONE
*/
int
bcm_dnx_cosq_max_latency_pkts_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    int max_count,
    bcm_cosq_max_latency_pkts_t * max_latency_pkts,
    int *actual_count)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_latency_max_pkts_get_verify
                          (unit, gport, flags, max_count, max_latency_pkts, actual_count));
    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_END_TO_END))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_egress_max_get(unit, gport, max_count, max_latency_pkts, actual_count));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_max_get(unit, gport, max_count, max_latency_pkts, actual_count));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  function set a single property of the ingress latency profile. 
* \param [in] unit -unit id
* \param [in] profile_id - profile_id
* \param [in] cosq - offset/index  of the object.relevant just for type bin
* \param [in] type - property type of the profile
* \param [in] arg  - the value of the property
* \return
*    shr_error_e
* \remark
*   All threshold are in nano second units, same as the latency units.
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_latency_ingress_profile_set(
    int unit,
    int profile_id,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (type == bcmCosqControlLatencyBinThreshold)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_INGRESS_PROFILE_BINS, &entry_handle_id));
        /** set the table key */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BIN_ID, cosq);
    }
    else
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_INGRESS_PROFILE, &entry_handle_id));
    }
    /** set the common tables key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    switch (type)
    {
        case bcmCosqControlLatencyCounterEnable:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_STAT_ENABLE, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyDropEnable:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LATENCY_DROP_ENABLE, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyEcnEnable:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_ENABLE, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyDropThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_DROP_THRESHOLD, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyEcnThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_THRESHOLD, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyTrackDropPacket:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_TRACK_DROP_ENABLE, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyTrack:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_TRACK_ENABLE, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyBinThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_BIN_THRESHOLD, INST_SINGLE, arg);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for ingress latency profile \n", type);
            break;
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  function set a single property of the end to end latency profile. 
* \param [in] unit -unit id
* \param [in] profile_id - profile_id
* \param [in] cosq - offset/index  of the object.relevant just for type bin
* \param [in] type - property type of the profile
* \param [in] arg  - the value of the property
* \return
*    shr_error_e
* \remark
*   All threshold are in nano second units, same as the latency units.
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_latency_end_to_end_profile_set(
    int unit,
    int profile_id,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (type == bcmCosqControlLatencyBinThreshold)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_PROFILE_BINS, &entry_handle_id));
        /** set the table key */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BIN_ID, cosq);
    }
    else
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_PROFILE, &entry_handle_id));
    }
    /** set the common tables key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    switch (type)
    {
        case bcmCosqControlLatencyCounterEnable:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_STAT_ENABLE, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyDropEnable:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LATENCY_DROP_ENABLE, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyEcnEnable:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_ENABLE, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyDropThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_DROP_THRESHOLD, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyEcnThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_THRESHOLD, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyEgressCounterCommand:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_STAT_COMMAND, INST_SINGLE,
                                         STAT_PP_PROFILE_ID_GET(arg));
            break;
        case bcmCosqControlLatencyTrack:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_TRACK_ENABLE, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyBinThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_BIN_THRESHOLD, INST_SINGLE, arg);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end latency profile \n", type);
            break;
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_latency_profile_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_profile_set
                        (unit, BCM_GPORT_INGRESS_LATENCY_PROFILE_GET(port), cosq, type, arg));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_profile_set
                        (unit, BCM_GPORT_END_TO_END_LATENCY_PROFILE_GET(port), cosq, type, arg));
    }
exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*  function get a single property value of the ingress latency profile. 
* \param [in] unit -unit id
* \param [in] profile_id - profile_id
* \param [in] cosq - offset/index  of the object.relevant just for type bin
* \param [in] type - property type of the profile
* \param [out] arg  - the value of the property
* \return
*    shr_error_e
* \remark
*   All threshold are in nano second units, same as the latency units.
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_latency_ingress_profile_get(
    int unit,
    int profile_id,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (type == bcmCosqControlLatencyBinThreshold)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_INGRESS_PROFILE_BINS, &entry_handle_id));
        /** set the table key */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BIN_ID, cosq);
    }
    else
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_INGRESS_PROFILE, &entry_handle_id));
    }
    /** set the common tables key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    switch (type)
    {
        case bcmCosqControlLatencyCounterEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_STAT_ENABLE, INST_SINGLE, (uint32 *) arg);
            break;
        case bcmCosqControlLatencyDropEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LATENCY_DROP_ENABLE, INST_SINGLE,
                                       (uint32 *) arg);
            break;
        case bcmCosqControlLatencyEcnEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_ENABLE, INST_SINGLE, (uint32 *) arg);
            break;
        case bcmCosqControlLatencyDropThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_DROP_THRESHOLD, INST_SINGLE,
                                       (uint32 *) arg);
            break;
        case bcmCosqControlLatencyEcnThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_THRESHOLD, INST_SINGLE,
                                       (uint32 *) arg);
            break;
        case bcmCosqControlLatencyTrackDropPacket:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_TRACK_DROP_ENABLE, INST_SINGLE,
                                       (uint32 *) arg);
            break;
        case bcmCosqControlLatencyTrack:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_TRACK_ENABLE, INST_SINGLE, (uint32 *) arg);
            break;
        case bcmCosqControlLatencyBinThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_BIN_THRESHOLD, INST_SINGLE,
                                       (uint32 *) arg);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for ingress latency profile \n", type);
            break;
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  function get a single property value of the end to end latency profile. 
* \param [in] unit -unit id
* \param [in] profile_id - profile_id
* \param [in] cosq - offset/index  of the object.relevant just for type bin
* \param [in] type - property type of the profile
* \param [out] arg  - the value of the property
* \return
*    shr_error_e
* \remark
*   All threshold are in nano second units, same as the latency units.
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_latency_end_to_end_profile_get(
    int unit,
    int profile_id,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (type == bcmCosqControlLatencyBinThreshold)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_PROFILE_BINS, &entry_handle_id));
        /** set the table key */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BIN_ID, cosq);
    }
    else
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_PROFILE, &entry_handle_id));
    }
    /** set the common tables key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    switch (type)
    {
        case bcmCosqControlLatencyCounterEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_STAT_ENABLE, INST_SINGLE, (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyDropEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LATENCY_DROP_ENABLE, INST_SINGLE,
                                       (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyEcnEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_ENABLE, INST_SINGLE, (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyDropThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_DROP_THRESHOLD, INST_SINGLE,
                                       (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyEcnThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_THRESHOLD, INST_SINGLE,
                                       (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyEgressCounterCommand:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_STAT_COMMAND, INST_SINGLE, (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            STAT_PP_ENGINE_PROFILE_SET(*arg, *arg, bcmStatCounterInterfaceEgressTransmitPp);
            break;
        case bcmCosqControlLatencyTrack:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_TRACK_ENABLE, INST_SINGLE, (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyBinThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_BIN_THRESHOLD, INST_SINGLE,
                                       (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end latency profile \n", type);
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_latency_profile_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_profile_get
                        (unit, BCM_GPORT_INGRESS_LATENCY_PROFILE_GET(port), cosq, type, arg));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_profile_get
                        (unit, BCM_GPORT_END_TO_END_LATENCY_PROFILE_GET(port), cosq, type, arg));
    }
exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*  get from HW the quartet profile per queue
* \param [in] unit -unit id
* \param [in] qid - queue id
* \param [out] profile_id - queue quartet profile id
* \return
*    shr_error_e
* \remark
*   None
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_latency_queue_quartet_profile_id_get(
    int unit,
    int qid,
    uint32 *profile_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get the quartet profile id */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_QUARTET_PROFILE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUARTET_QUEUE,
                               (qid / DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_latency_ingress_queue_map_verify(
    int unit,
    int flags,
    int lat_profile_id,
    int qid)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify that the profile_id is quartet base */
    if (lat_profile_id % DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid lat_profile_id (=%d)", lat_profile_id);
    }
    /** verify that the queue-id is quartet base */
    if (qid % DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid queue_id (=%d)", qid);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_latency_ingress_queue_profile_set(
    int unit,
    int profile,
    dnx_cosq_latency_queue_template_t * info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_QUARTET_PROFILE_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUARTET_PROFILE, profile);
    /** Setting values  */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_FLOW_PROFILE, INST_SINGLE, info->latency_map);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_MAP, INST_SINGLE, info->tc_map);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_latency_ingress_queue_profile_info_get(
    int unit,
    int profile,
    dnx_cosq_latency_queue_template_t * info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_QUARTET_PROFILE_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUARTET_PROFILE, profile);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_FLOW_PROFILE, INST_SINGLE,
                               (uint32 *) &info->latency_map);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TC_MAP, INST_SINGLE, (uint32 *) &info->tc_map);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_latency_ingress_queue_profile_map(
    int unit,
    int qid,
    int profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_QUARTET_PROFILE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUARTET_QUEUE,
                               (qid / DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION));
    /** Setting values  */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, profile);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_latency_ingress_queue_map_set(
    int unit,
    int flags,
    int qid,
    int lat_profile_id)
{
    uint32 quartet_profile_id;
    dnx_cosq_latency_queue_template_t profile_info;
    int new_profile;
    uint8 first_reference, last_reference;
    SHR_FUNC_INIT_VARS(unit);

    /** verify inputs params */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_latency_ingress_queue_map_verify(unit, flags, lat_profile_id, qid));

    /** get the quartet_profile_id and tc_map for the qid */
    SHR_IF_ERR_EXIT(dnx_cosq_latency_queue_quartet_profile_id_get(unit, qid, &quartet_profile_id));
    /** get the profile info */
    SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_queue_profile_info_get(unit, quartet_profile_id, &profile_info));
    /** modify the latency_map of the profile for template manager usage */
    profile_info.latency_map = lat_profile_id / DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION;

    /** exchange the profile */
    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.queue_template.exchange(unit, _SHR_CORE_ALL, 0, &profile_info,
                                                                quartet_profile_id, NULL, &new_profile,
                                                                &first_reference, &last_reference));
    /** update profile in HW */
    if (first_reference == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_queue_profile_set(unit, new_profile, &profile_info));
    }
    /** map the queue quartet to the new profile */
    SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_queue_profile_map(unit, qid, new_profile));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_latency_ingress_queue_map_get(
    int unit,
    int flags,
    int qid,
    int *lat_profile_id)
{
    uint32 quartet_profile_id;
    dnx_cosq_latency_queue_template_t info;
    SHR_FUNC_INIT_VARS(unit);

    /** get the quartet_profile_id and tc_map for the qid */
    SHR_IF_ERR_EXIT(dnx_cosq_latency_queue_quartet_profile_id_get(unit, qid, &quartet_profile_id));

    SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_queue_profile_info_get(unit, quartet_profile_id, &info));
    *lat_profile_id = info.latency_map * DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION;

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_latency_end_to_end_per_port_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_END_TO_END_CONFIG, &entry_handle_id));
    /** Setting values  */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_PER_FLOW_ENABLE, INST_SINGLE, (enable == FALSE));
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_latency_end_to_end_per_port_get(
    int unit,
    int *enable)
{
    uint32 entry_handle_id;
    uint32 flow_enable;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_END_TO_END_CONFIG, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_PER_FLOW_ENABLE, INST_SINGLE, &flow_enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *enable = (flow_enable == FALSE);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_latency_end_to_end_port_map_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport_to_map,
    int profile_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flags (=%d)", flags);
    }
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_latency_end_to_end_port_map_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport_to_map,
    int profile_id)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** verify inputs params */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_latency_end_to_end_port_map_verify(unit, flags, gport_to_map, profile_id));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport_to_map, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
        /** Set key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        /** Set profile */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LATENCY_FLOW_PROFILE, INST_SINGLE, profile_id);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_latency_end_to_end_port_map_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport_to_map,
    int *profile_id)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport_to_map, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /** take the profile from the first pp port in the array */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    /** Set key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);
    /** Get profile */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LATENCY_FLOW_PROFILE, INST_SINGLE,
                               (uint32 *) profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

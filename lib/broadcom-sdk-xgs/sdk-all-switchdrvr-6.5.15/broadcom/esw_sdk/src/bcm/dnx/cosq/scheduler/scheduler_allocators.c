
/** \file scheduler_allocators.c
 * $Id$
 *
 * e2e scheduler functionality for DNX --  allocations
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

#include <bcm/types.h>
#include <shared/shrextend/shrextend_error.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_sch_alloc_mngr_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <bcm_int/dnx/algo/res_mngr/simple_bitmap_wrap.h>

#include "scheduler_flow_alloc.h"
#include "scheduler_dbal.h"

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>

#define DNX_SCHEDULER_INTERFACE_NON_CHANNELIZED_TAG 2
#define DNX_SCHEDULER_INTERFACE_BIG_TAG 1
#define DNX_SCHEDULER_INTERFACE_REGULAR_TAG 0

/**
 * \brief -
 * initialize CL class template manager
 */
static shr_error_e
dnx_scheduler_allocators_cl_class_tmpl_mngr_create(
    int unit)
{
    sw_state_algo_template_create_data_t create_info;
    dnx_sch_cl_info_t cl_template_data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&create_info, 0, sizeof(sw_state_algo_template_create_data_t));
    sal_memset(&cl_template_data, 0, sizeof(dnx_sch_cl_info_t));

    create_info.flags = SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE;
    create_info.first_profile = 0;
    create_info.nof_profiles = dnx_data_sch.se.cl_class_profile_nof_get(unit);
    /** one additional dummy reference for default profile */
    create_info.max_references = dnx_data_sch.flow.nof_cl_get(unit) + 1;
    create_info.default_profile = 0;
    create_info.data_size = sizeof(dnx_sch_cl_info_t);
    create_info.default_data = &cl_template_data;
    sal_strncpy(create_info.name, "TEMPLATE_COSQ_SCHED_CLASS", SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(sch_alloc_db.cl_class_alloc_mngr.create(unit, &create_info, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
void
dnx_sch_cl_class_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_sch_cl_info_t *profile_data;

    if (data)
    {
        DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

        profile_data = (dnx_sch_cl_info_t *) data;

        DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_STRING, NULL, "CL config", NULL, NULL);
        {
            DNX_ALGO_TEMPLATE_PRINT_CB_SUB_STRUCT_START();
            DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "cl_mode", profile_data->config.cl_mode,
                                                NULL, NULL);
            DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "wfq_weight_mode",
                                                profile_data->config.wfq_weight_mode, NULL, NULL);
            DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "enhanced_mode",
                                                profile_data->config.enhanced_mode, NULL, NULL);
            DNX_ALGO_TEMPLATE_PRINT_CB_SUB_STRUCT_END();
        }

        DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_STRING, NULL, "CL weight", NULL, NULL);
        DNX_ALGO_TEMPLATE_PRINT_CB_SUB_STRUCT_START();
        {
            DNX_ALGO_TEMPLATE_PRINT_CB_ADD_ARR(TEMPLATE_MNGR_PRINT_TYPE_INT, "weight", profile_data->weight.weight,
                                               NULL, NULL, DNX_SCH_CL_WEIGHTS_NOF);
        }
        DNX_ALGO_TEMPLATE_PRINT_CB_SUB_STRUCT_END();

        DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
    }
    else
    {
        LOG_CLI((BSL_META_U(unit, "invalid profile data\n")));
    }

    return;
}

/**
 * \brief - allocate and configure CL class profile
 * 
 */
shr_error_e
dnx_scheduler_allocators_cl_profile_alloc(
    int unit,
    int core_id,
    uint32 flags,
    dnx_sch_cl_info_t * profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference)
{

    SHR_FUNC_INIT_VARS(unit);

    /** allocate profile */
    SHR_IF_ERR_EXIT(sch_alloc_db.cl_class_alloc_mngr.allocate_single(unit, core_id, flags, profile_data,
                                                                     extra_arguments, profile, first_reference));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - deallocate cl profile 
 */
shr_error_e
dnx_scheduler_allocators_cl_profile_dealloc(
    int unit,
    int core_id,
    int profile,
    uint8 *last_reference)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sch_alloc_db.cl_class_alloc_mngr.free_single(unit, core_id, profile, last_reference));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get cl profile template data
 */
shr_error_e
dnx_scheduler_allocators_cl_profile_data_get(
    int unit,
    int core_id,
    int profile,
    int *ref_count,
    dnx_sch_cl_info_t * profile_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sch_alloc_db.cl_class_alloc_mngr.profile_data_get(unit, core_id, profile, ref_count, profile_data));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Exchange cl profile template data
 */
shr_error_e
dnx_scheduler_allocators_cl_profile_exchange(
    int unit,
    int core_id,
    uint32 flags,
    const dnx_sch_cl_info_t * profile_data,
    int old_profile,
    const void *extra_arguments,
    int *new_profile,
    uint8 *first_reference,
    uint8 *last_reference)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sch_alloc_db.cl_class_alloc_mngr.exchange(unit, core_id, flags, profile_data, old_profile,
                                                              extra_arguments, new_profile,
                                                              first_reference, last_reference));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - create scheduler interface allocation manager
 */
shr_error_e
dnx_scheduler_interface_allocator_create(
    int unit)
{
    simple_bitmap_wrap_create_info_t create_extra_info;
    sw_state_algo_res_create_data_t create_data;

    simple_bitmap_wrap_tag_info_t tag_info;

    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&create_data, 0, sizeof(create_data));
    sal_memset(&create_extra_info, 0, sizeof(create_extra_info));

    sal_memset(&tag_info, 0, sizeof(tag_info));

    /*
     * Set bitmap info
     */
    create_data.flags =
        SW_STATE_ALGO_RES_CREATE_DUPLICATE_PER_CORE | DNX_ALGO_RES_SIMPLE_BITMAP_CREATE_ALLOW_FORCING_TAG;
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_sch.interface.nof_sch_interfaces_get(unit);

    create_extra_info.grain_size = dnx_data_sch.interface.nof_big_calendars_get(unit);
    create_extra_info.max_tag_value = DNX_SCHEDULER_INTERFACE_NON_CHANNELIZED_TAG;

    sal_strncpy(create_data.name, "SCH interface allocator", SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /*
     * Create sch interface allocator
     */
    SHR_IF_ERR_EXIT(sch_alloc_db.interface.alloc(unit, SW_STATE_ALGO_RES_NOF_CORES_GET(unit)));
    for (core_id = 0; core_id < SW_STATE_ALGO_RES_NOF_CORES_GET(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(sch_alloc_db.interface.create(unit, core_id, &create_data, &create_extra_info));
    }

    for (core_id = 0; core_id < SW_STATE_ALGO_RES_NOF_CORES_GET(unit); core_id++)
    {
        /** big calendar tag */
        tag_info.element = 0;
        tag_info.tag = DNX_SCHEDULER_INTERFACE_BIG_TAG;
        tag_info.nof_elements = dnx_data_sch.interface.nof_big_calendars_get(unit);
        tag_info.force_tag = TRUE;
        SHR_IF_ERR_EXIT(sch_alloc_db.interface.advanced_algorithm_info_set(unit, core_id, (void *) &tag_info));

        /** regular calendar tag */
        tag_info.element = dnx_data_sch.interface.nof_big_calendars_get(unit);
        tag_info.tag = DNX_SCHEDULER_INTERFACE_REGULAR_TAG;
        tag_info.nof_elements = dnx_data_sch.interface.nof_sch_interfaces_get(unit)
            - dnx_data_sch.interface.nof_big_calendars_get(unit);
        tag_info.force_tag = TRUE;
        SHR_IF_ERR_EXIT(sch_alloc_db.interface.advanced_algorithm_info_set(unit, core_id, (void *) &tag_info));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * allocate new scheduler interface
 * if prefer_big = TRUE, big interface would be allocated if available.
 * if prefer_big = FALSE, regular interface would be allocated if available.
 */
shr_error_e
dnx_scheduler_allocators_interface_allocate(
    int unit,
    int core,
    int prefer_big,
    int *interface_id)
{
    shr_error_e res = _SHR_E_INTERNAL;
    int allocated_id = -1;

    simple_bitmap_wrap_alloc_info_t alloc_info;

    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&alloc_info, 0x0, sizeof(simple_bitmap_wrap_alloc_info_t));

    alloc_info.tag = (prefer_big ? DNX_SCHEDULER_INTERFACE_BIG_TAG : DNX_SCHEDULER_INTERFACE_REGULAR_TAG);

    /** check if can allocate */
    res = sch_alloc_db.interface.allocate_single(unit, core,
                                                 flags | SW_STATE_ALGO_RES_ALLOCATE_SIMULATION, (void *) &alloc_info,
                                                 &allocated_id);

    if (res == _SHR_E_RESOURCE)
    {
        /** can't allocate interface of the preferred type, try the other type */
        alloc_info.tag = (prefer_big ? DNX_SCHEDULER_INTERFACE_REGULAR_TAG : DNX_SCHEDULER_INTERFACE_BIG_TAG);
    }
    else
    {
        SHR_IF_ERR_EXIT(res);
    }

    /*
     * Now we actually can allocate
     */
    SHR_IF_ERR_EXIT(sch_alloc_db.interface.allocate_single(unit, core, flags, (void *) &alloc_info, &allocated_id));

    *interface_id = allocated_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * deallocate scheduler interface
 */
shr_error_e
dnx_scheduler_allocators_interface_deallocate(
    int unit,
    int core,
    int interface_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sch_alloc_db.interface.free_single(unit, core, interface_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file.
 */
shr_error_e
dnx_scheduler_allocators_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sch_alloc_db.init(unit));

    /** create allocator for scheduler flow */
    SHR_IF_ERR_EXIT(dnx_scheduler_flow_alloc_mngr_create(unit));

    /** create allocator for scheduler interface */
    SHR_IF_ERR_EXIT(dnx_scheduler_interface_allocator_create(unit));

    SHR_IF_ERR_EXIT(dnx_scheduler_allocators_cl_class_tmpl_mngr_create(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file.
 */
shr_error_e
dnx_scheduler_allocators_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit 
     */

    /*
     * Resource and template manager don't require deinitialization per instance.
     */

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

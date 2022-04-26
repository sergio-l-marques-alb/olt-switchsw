/* \file ctests_dnx_scheduling_elements_tests.c
 *
 * Tests for priority
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL
/************
 * INCLUDES  *
 *************/
#include "ctest_dnx_scheduling_elements_tests.h"

#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/dnx/diag_dnx_utils.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <src/bcm/dnx/cosq/scheduler/scheduler_flow_alloc.h>
#include <bcm_int/dnx/algo/sch/sch_alloc_mngr.h>

#include <sal/appl/sal.h>

 /************
 *  DEFINES   *
 *************/

#define DNX_SE_MAX_NUM_SOC_PROPERTIES  100

/** Scheduling elements types */
#define SE_TYPE_HR 0
#define SE_TYPE_CL 1
#define SE_TYPE_FQ 2

/** Scheduling element modes */
#define SE_MODE_SIMPLE 0
#define SE_MODE_COMPOSITE 1
#define SE_MODE_ENHANCED 2

/************
 * TYPEDEFS  *
 *************/

extern shr_error_e dnx_scheduler_flow_fill_alloc_info(
    int unit,
    int core,
    int num_cos,
    dnx_sch_alloc_flow_type_e flow_type,
    dnx_scheduler_shared_shaper_order_t shared_order,
    int region_index,
    dnx_algo_sch_alloc_info_t * alloc_info);

typedef struct
{
    ctest_soc_property_t ctest_soc_property[DNX_SE_MAX_NUM_SOC_PROPERTIES];
    int nof_soc_properties;
} dnx_se_soc_properties_t;

/************
* FUNCTIONS *
*************/

static sh_sand_enum_t sh_enum_table_se_types[] = {
    {"HR", SE_TYPE_HR, "Allocate all HR elements"},
    {"CL", SE_TYPE_CL, "Allocate all CL elements"},
    {"FQ", SE_TYPE_FQ, "Allocate all FQ elements"},
    {NULL}
};

static sh_sand_enum_t sh_enum_table_se_modes[] = {
    {"Simple", SE_MODE_SIMPLE, "Allocate simple elements"},
    {"Composite", SE_MODE_COMPOSITE, "Allocate composite elements"},
    {"Enhanced", SE_MODE_ENHANCED, "Allocate enhanced elements"},
    {NULL}
};

sh_sand_man_t dnx_scheduling_elements_alloc_man = {
    "Allocate all flow ID of a given type",
    "This test is trying to allocate all flow ID of a given type.\nThe test will try to delete the flow IDs, once allocated",
    "ctest tm scheduler scheduling_elements allocate",
    "type=HR mode=Simple"
};

/* *INDENT-OFF* */
sh_sand_option_t dnx_scheduling_elements_alloc_options[] = {
    /* Name         Type                 Description                    Default */
    {"TYpe",        SAL_FIELD_TYPE_ENUM, "Scheduling element type",     "HR",     sh_enum_table_se_types},
    {"MoDe",        SAL_FIELD_TYPE_ENUM, "Scheduling element mode",     "Simple", sh_enum_table_se_modes},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_invoke_t dnx_scheduling_elements_alloc_tests[] = {
    {"se_flex_quartet_alloc_hr_simple", "TYpe=HR MoDe=Simple", CTEST_POSTCOMMIT},
    {"se_flex_quartet_alloc_cl_simple", "TYpe=CL MoDe=Simple", CTEST_POSTCOMMIT},
    {"se_flex_quartet_alloc_fq_simple", "TYpe=FQ MoDe=Simple", CTEST_POSTCOMMIT},
    {"se_flex_quartet_alloc_hr_composite", "TYpe=HR MoDe=Composite", CTEST_POSTCOMMIT},
    {"se_flex_quartet_alloc_cl_composite", "TYpe=CL MoDe=Composite", CTEST_POSTCOMMIT},
    {"se_flex_quartet_alloc_fq_composite", "TYpe=FQ MoDe=Composite", CTEST_POSTCOMMIT},
    {"se_flex_quartet_alloc_cl_enhanced", "TYpe=CL MoDe=Enhanced", CTEST_POSTCOMMIT},
    {NULL}
};

/*
 * \brief
 * Load one SoC Property into soc_properties structure.
 */
shr_error_e
dnx_scheduling_elements_soc_property_set(
    int unit,
    dnx_se_soc_properties_t * soc_properties,
    char *property,
    char *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(property, _SHR_E_PARAM, "property");

    if (soc_properties->nof_soc_properties > DNX_SE_MAX_NUM_SOC_PROPERTIES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Reached the maximum number of SOC properties %d. \"%s\"=\"%s\"\n",
                     soc_properties->nof_soc_properties, property, value);
    }

    soc_properties->ctest_soc_property[soc_properties->nof_soc_properties].property = property;
    soc_properties->ctest_soc_property[soc_properties->nof_soc_properties].value = value;
    soc_properties->nof_soc_properties++;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_scheduling_elements_nof_multi_offset_regions_get(
    int unit,
    dnx_sch_alloc_flow_type_e flow_type,
    dnx_scheduler_shared_shaper_order_t shared_order,
    int *nof_regions)
{
    int region_idx, first_se_region, last_se_region;

    dnx_algo_sch_alloc_info_t alloc_info;

    SHR_FUNC_INIT_VARS(unit);

    *nof_regions = 0;

    first_se_region = dnx_data_sch.flow.nof_connectors_only_regions_get(unit);
    last_se_region = dnx_data_sch.flow.nof_regions_get(unit);

    for (region_idx = first_se_region; region_idx < last_se_region; region_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_flow_fill_alloc_info
                        (unit, 0, 1, flow_type, shared_order, region_idx, &alloc_info));
        if (alloc_info.nof_offsets > 1)
        {
            (*nof_regions)++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * Return the flow quartet order in the HR regions.
 */
static shr_error_e
dnx_scheduling_elements_hr_quartet_type_get(
    int unit,
    dnx_sch_flow_quartet_type_e * hr_quartet_type)
{
    int group_idx;
    SHR_FUNC_INIT_VARS(unit);

    /** Get quartet type from core 0, assuming symmetric configuration */
    group_idx = DNX_SCH_QUARTET_GROUP_INDEX_FROM_REGION_INDEX_GET(dnx_data_sch.flow.nof_regions_get(unit)) - 1;
    *hr_quartet_type = dnx_data_sch.flow.quartet_type_get(unit, 0, group_idx)->type;

    SHR_FUNC_EXIT;
}

/*
 * \brief
 * Return the number of composite elements in a region per quartet order.
 * The function returns the number of elements in a SE only region and in an HR region.
 */
static shr_error_e
dnx_scheduling_elements_nof_comp_elements_in_region_get(
    int unit,
    dnx_sch_flow_quartet_type_e quartet_type,
    dnx_sch_alloc_flow_type_e flow_type,
    int *nof_elements_in_se_region,
    int *nof_elements_in_hr_region)
{
    int region_size = dnx_data_sch.flow.region_size_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    switch (flow_type)
    {
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR:
        {
            /*
             * HRs are available on HR regions only.
             * There is one HR in a quartet.
             */
            *nof_elements_in_se_region = 0;
            *nof_elements_in_hr_region = region_size / 4;
            break;
        }

        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL:
        {
            switch (quartet_type)
            {
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_CL_FQ:
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_CL_FQ_FQ:
                    /*
                     * The types support two composite CLs in quartet in SE regions.
                     * The types support one composite CL in quartet in HR regions.
                     */
                    *nof_elements_in_se_region = region_size / 2;
                    *nof_elements_in_hr_region = region_size / 4;
                    break;
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_FQ_CL:
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_FQ_CL:
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_FQ_CL_CL:
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_CL_FQ:
                    /*
                     * The types support one composite CL in quartet in SE regions.
                     * The types support one composite CL in quartet in HR regions.
                     */
                    *nof_elements_in_se_region = region_size / 4;
                    *nof_elements_in_hr_region = region_size / 4;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected quartet type (%d). \n", quartet_type);
            }
            break;
        }

        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ:
        {
            switch (quartet_type)
            {
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_CL_FQ:
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_CL_FQ_FQ:
                    /*
                     * The types does not support composite FQ
                     */
                    *nof_elements_in_se_region = 0;
                    *nof_elements_in_hr_region = 0;
                    break;
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_FQ_CL:
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_FQ_CL:
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_FQ_CL_CL:
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_CL_FQ:
                    /*
                     * The types support composite FQ in SE regions.
                     */
                    *nof_elements_in_se_region = region_size / 4;
                    *nof_elements_in_hr_region = 0;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected quartet type (%d). \n", quartet_type);
            }
            break;
        }

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected flow type (%d). \n", flow_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * Return the total number of composite elements of a given flow type.
 */
static shr_error_e
dnx_scheduling_elements_nof_comp_elements_get(
    int unit,
    dnx_sch_alloc_flow_type_e flow_type,
    int *nof_elements)
{
    int first_se_region, last_se_region;
    int region_idx, group_idx;
    int nof_elements_in_se_region, nof_elements_in_hr_region;
    dnx_sch_flow_quartet_type_e quartet_type;
    dnx_sch_region_type_e region_type;

    SHR_FUNC_INIT_VARS(unit);

    first_se_region = dnx_data_sch.flow.nof_connectors_only_regions_get(unit);
    last_se_region = dnx_data_sch.flow.nof_regions_get(unit);

    *nof_elements = 0;
    for (region_idx = first_se_region; region_idx < last_se_region; region_idx++)
    {
        /** Get quartet AND region types from core 0, assuming symmetric configuration */
        group_idx = DNX_SCH_QUARTET_GROUP_INDEX_FROM_REGION_INDEX_GET(region_idx);
        quartet_type = dnx_data_sch.flow.quartet_type_get(unit, 0, group_idx)->type;
        region_type = dnx_data_sch.flow.region_type_get(unit, 0, region_idx)->type;

        SHR_IF_ERR_EXIT(dnx_scheduling_elements_nof_comp_elements_in_region_get
                        (unit, quartet_type, flow_type, &nof_elements_in_se_region, &nof_elements_in_hr_region));

        switch (region_type)
        {
            case DNX_SCH_REGION_TYPE_SE:
                *nof_elements += nof_elements_in_se_region;
                break;
            case DNX_SCH_REGION_TYPE_SE_HR:
                *nof_elements += nof_elements_in_hr_region;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal region type (%d). \n", region_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * Return the number of elements of a given type
 */
static shr_error_e
dnx_scheduling_elements_nof_elements_get(
    int unit,
    dnx_sch_alloc_flow_type_e flow_type,
    dnx_scheduler_shared_shaper_order_t shared_order,
    int *nof_elements)
{
    int nof_multi_offset_regions;
    dnx_sch_flow_quartet_type_e hr_quartet_type;

    SHR_FUNC_INIT_VARS(unit);

    switch (flow_type)
    {
        case DNX_SCH_ALLOC_FLOW_TYPE_HR:
            *nof_elements = dnx_data_sch.flow.nof_hr_get(unit);
            if (dnx_data_sch.flow.reserved_hr_get(unit) > 0)
            {
                /** decrease one element, in case of a reserved HR */
                *nof_elements = *nof_elements - 1;
            }
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_CL:
            *nof_elements = dnx_data_sch.flow.nof_cl_get(unit);
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_FQ:
            /** number of FQ = NOF SE - (NOF CL - NOF HR + 1). "+1" is for the reserved FQ element */
            *nof_elements =
                dnx_data_sch.flow.nof_se_get(unit) - (dnx_data_sch.flow.nof_cl_get(unit) +
                                                      dnx_data_sch.flow.nof_hr_get(unit) + 1);
            break;

        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR:
            SHR_IF_ERR_EXIT(dnx_scheduling_elements_nof_comp_elements_get(unit, flow_type, nof_elements));
            if (dnx_data_sch.flow.reserved_hr_get(unit) > 0)
            {
                /** decrease one element, in case of a reserved HR */
                (*nof_elements)--;
            }

            SHR_IF_ERR_EXIT(dnx_scheduling_elements_hr_quartet_type_get(unit, &hr_quartet_type));
            switch (hr_quartet_type)
            {
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_CL_FQ:
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_CL_FQ_FQ:
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_FQ_CL:
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_FQ_CL_CL:
                    /*
                     *  The composite HR for the following types is HR-FQ.
                     *  One element should be decreased because of the reserved FQ.
                     */
                    (*nof_elements)--;
                    break;
                default:
            /** Do Nothing */
                    break;
            }

            break;

        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL:
            SHR_IF_ERR_EXIT(dnx_scheduling_elements_nof_comp_elements_get(unit, flow_type, nof_elements));

            SHR_IF_ERR_EXIT(dnx_scheduling_elements_hr_quartet_type_get(unit, &hr_quartet_type));
            switch (hr_quartet_type)
            {
                case DNX_SCH_FLOW_QUARTET_TYPE_CL_FQ_FQ_CL:
                case DNX_SCH_FLOW_QUARTET_TYPE_FQ_CL_CL_FQ:
                    /*
                     *  The composite CL for the following types, in HR regions, is CL-FQ.
                     *  One element should be decreased because of the reserved FQ.
                     */
                    (*nof_elements)--;
                    break;
                default:
            /** Do Nothing */
                    break;
            }

            break;

        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ:
            SHR_IF_ERR_EXIT(dnx_scheduling_elements_nof_comp_elements_get(unit, flow_type, nof_elements));
            break;

        case DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL:
            *nof_elements = dnx_data_sch.flow.nof_cl_get(unit) - dnx_data_sch.flow.nof_hr_get(unit) - 1;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal type (%d). \n", flow_type);
            break;
    }

    
    SHR_IF_ERR_EXIT(dnx_scheduling_elements_nof_multi_offset_regions_get
                    (unit, flow_type, shared_order, &nof_multi_offset_regions));
    *nof_elements -= nof_multi_offset_regions;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_scheduling_elements_flow_type_get(
    int unit,
    int type,
    int mode,
    dnx_sch_alloc_flow_type_e * flow_type,
    dnx_scheduler_shared_shaper_order_t * shaper_order)
{
    SHR_FUNC_INIT_VARS(unit);

    *shaper_order = DNX_SCHEDULER_SHARED_SHAPER_ORDER_NONE;

    switch (type)
    {
        case SE_TYPE_HR:
        {
            switch (mode)
            {
                case SE_MODE_SIMPLE:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_HR;
                    break;
                case SE_MODE_COMPOSITE:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected HR mode %d\n", mode);
                    break;
            }
            break;
        }

        case SE_TYPE_CL:
        {
            switch (mode)
            {
                case SE_MODE_SIMPLE:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_CL;
                    break;
                case SE_MODE_COMPOSITE:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL;
                    break;
                case SE_MODE_ENHANCED:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected CL mode %d\n", mode);
                    break;
            }
            break;
        }

        case SE_TYPE_FQ:
        {
            switch (mode)
            {
                case SE_MODE_SIMPLE:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_FQ;
                    break;
                case SE_MODE_COMPOSITE:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected HR mode %d\n", mode);
                    break;
            }
            break;
        }

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected type %d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Test for allocating all flow IDs of a given type (without ID)
 *
 *   Test walkthrough:
 *   1) Allocate all flows of the specific type.
 *   2) Try allocate a flow after all flows were allocated - should fail.
 *   3) Delete all allocated flow.
 *   4) Repeat steps 1-3.
 *
 * Note:
 *   Test depends on queue bundle size and type.
 */
static shr_error_e
dnx_scheduling_elements_alloc(
    int unit,
    int type,
    int mode)
{
    int rv;
    int nof_elements = 0, element_idx = 0, core_idx;
    int *allocated_flow_ids = NULL;
    int flow_id;

    dnx_sch_alloc_flow_type_e flow_type;
    dnx_scheduler_shared_shaper_order_t shared_order;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduling_elements_flow_type_get(unit, type, mode, &flow_type, &shared_order));
    SHR_IF_ERR_EXIT(dnx_scheduling_elements_nof_elements_get(unit, flow_type, shared_order, &nof_elements));
    SHR_ALLOC(allocated_flow_ids, nof_elements * sizeof(int), "allocated flow ids", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    sal_memset(allocated_flow_ids, 0, nof_elements * sizeof(int));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
    {
        LOG_CLI((BSL_META("Allocate flows on core %d\n"), core_idx));
        /*
         * Allocate and delete two times
         */
        for (int i = 0; i < 2; i++)
        {
            /*
             * 1. Allocate all flows of the specific type
             */
            LOG_CLI((BSL_META("Allocate %d flows\n"), nof_elements));
            for (element_idx = 0; element_idx < nof_elements; element_idx++)
            {
                SHR_CLI_EXIT_IF_ERR(dnx_scheduler_flow_allocate
                                    (unit, core_idx, 1, 0, FALSE, FALSE, 1, flow_type, shared_order, -1, &flow_id),
                                    "Test FAIL. Failed to allocate element %d\n", element_idx);
                allocated_flow_ids[element_idx] = flow_id;
            }

            /*
             * 2. Try allocate a flow after all flows were allocated - should fail.
             */
            rv = dnx_scheduler_flow_allocate(unit, core_idx, 1, 0, FALSE, FALSE, 1, flow_type, shared_order, -1,
                                             &flow_id);
            if (SHR_SUCCESS(rv))
            {
                SHR_CLI_EXIT(rv, "Test FAIL. Allocated the %d element, which is more than expected.", flow_id);
            }

            /*
             * 3. Delete all allocated flows
             */
            LOG_CLI((BSL_META("Delete allocated flows\n")));
            for (element_idx = 0; element_idx < nof_elements; element_idx++)
            {
                flow_id = allocated_flow_ids[element_idx];
                SHR_CLI_EXIT_IF_ERR(dnx_scheduler_flow_deallocate(unit, core_idx, 1, flow_type, flow_id),
                                    "Test Fail. Failed to delete element %d with flow id %d\n", element_idx, flow_id);
            }
        }
    }

exit:
    SHR_FREE(allocated_flow_ids);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_scheduling_elements_alloc_soc_properties_load(
    int unit,
    dnx_se_soc_properties_t * soc_properties)
{
    int first_se_region, last_se_region;
    int region_idx, group_idx;
    int group_size;

    /*
     * Make this array static since it is used by the caller.
     */
    static char soc_property_name[DNX_DATA_MAX_SCH_FLOW_NOF_QUARTET_ORDER_GROUPS][RHNAME_MAX_SIZE];
    static char *flow_orders[] =
        { "CL_FQ_CL_FQ", "CL_FQ_FQ_CL", "CL_CL_FQ_FQ", "FQ_CL_FQ_CL", "FQ_CL_CL_FQ", "FQ_FQ_CL_CL" };
    int order_idx;
    int number_of_orders;

    SHR_FUNC_INIT_VARS(unit);

    number_of_orders = sizeof(flow_orders) / sizeof(flow_orders[0]);

    /** Init properties structure */
    sal_memset(soc_properties, 0, sizeof(dnx_se_soc_properties_t));

    /** Disable E2E application and remove ports */
    SHR_IF_ERR_EXIT(dnx_scheduling_elements_soc_property_set(unit, soc_properties, "appl_enable_e2e_scheme", "0"));
    SHR_IF_ERR_EXIT(dnx_scheduling_elements_soc_property_set(unit, soc_properties, "ucode_port*", NULL));
    SHR_IF_ERR_EXIT(dnx_scheduling_elements_soc_property_set
                    (unit, soc_properties, "dtm_flow_mapping_mode_region_*", NULL));
    SHR_IF_ERR_EXIT(dnx_scheduling_elements_soc_property_set(unit, soc_properties, "dtm_flow_mapping_mode", "3"));

    /** Set random flow order */
    first_se_region = dnx_data_sch.flow.nof_connectors_only_regions_get(unit) + 1;
    last_se_region = dnx_data_sch.flow.nof_regions_get(unit) + 1;
    group_size = dnx_data_sch.flow.nof_regions_in_quartet_order_group_get(unit);

    for (region_idx = first_se_region, group_idx = 0; region_idx < last_se_region;
         region_idx += group_size, group_idx++)
    {
        order_idx = sal_rand() % number_of_orders;
        sal_snprintf(soc_property_name[group_idx], RHNAME_MAX_SIZE, "%s%d.%d", "flow_quartet_order_base_region_",
                     region_idx, unit);
        SHR_IF_ERR_EXIT(dnx_scheduling_elements_soc_property_set
                        (unit, soc_properties, soc_property_name[group_idx], flow_orders[order_idx]));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * Set SoC properties and call to maximal flow ID allocation test
 */
static shr_error_e
dnx_scheduling_elements_alloc_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int type, mode;
    rhhandle_t ctest_soc_set_h = NULL;
    dnx_se_soc_properties_t soc_properties;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");

    SH_SAND_GET_ENUM("TYpe", type);
    SH_SAND_GET_ENUM("MoDe", mode);

    SHR_IF_ERR_EXIT(dnx_scheduling_elements_alloc_soc_properties_load(unit, &soc_properties));
    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, soc_properties.ctest_soc_property, &ctest_soc_set_h));

    SHR_IF_ERR_EXIT(dnx_scheduling_elements_alloc(unit, type, mode));

exit:
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_scheduling_elements_alloc_is_supported(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_sch.flow.feature_get(unit, dnx_data_sch_flow_flex_quartet_supported))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_tm_scheduling_elements_cmds[] = {
    {"allocate", dnx_scheduling_elements_alloc_cmd, NULL, dnx_scheduling_elements_alloc_options,
     &dnx_scheduling_elements_alloc_man, NULL, dnx_scheduling_elements_alloc_tests, SH_CMD_CONDITIONAL,
     dnx_scheduling_elements_alloc_is_supported}
    ,
    {NULL}
};

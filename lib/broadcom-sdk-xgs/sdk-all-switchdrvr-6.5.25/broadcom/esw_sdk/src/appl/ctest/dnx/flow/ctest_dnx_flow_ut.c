/** \file ctest_dnx_flow_ut.c
 *
 * per application tests
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

/*************
 * INCLUDES  *
 *************/
#include <sal/appl/sal.h>

#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <appl/diag/bslenable.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/utils/dnx_sbusdma_desc.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_stk_dispatch.h>
#include <sal/core/boot.h>
#include <bcm/flow.h>
#include <include/bcm_int/dnx/qos/qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/ctest/dnxc/ctest_dnxc_system.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <include/bcm_int/dnx/algo/rx/algo_rx.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_rx_access.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include "ctest_dnx_flow.h"
#include "src/appl/diag/dnx/flow/diag_dnx_flow.h"

/*************
 *  DEFINES  *
 *************/

/** test standrd logs - making line shorter */
#define LM              BSL_LOG_MODULE
#define U               BSL_META_U
#define LOGI            LOG_INFO

/** Entry priority that is used for TCAM test */
#define CTEST_FLOW_ENTRY_PRIORITY_VALUE      5

/** Maximum number of dbal fields a common field can be mapped to */
#define CTEST_FLOW_NOF_COMMON_TO_DBAL_FIELDS 2

/** Size of the common related debal fields - max size of dbal fields a single common field can be mapped plus 1 */
#define CTEST_FLOW_SIZE_OF_COMMON_RELATED_DBAL_FIELDS (CTEST_FLOW_NOF_COMMON_TO_DBAL_FIELDS + 1)

typedef struct
{
    flow_special_fields_e special_fields_added[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS];
} flow_traverse_data_t;

/***************
 * Parameters  *
 ***************/
bcm_gport_t gport_pwe_ingress_lif_scoped;

/***************
 *  Functions  *
 ***************/

shr_error_e ctest_flow_ut_run(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint32 keep_entry,
    bcm_gport_t * flow_id,
    int is_short_mode);

/** perform a set on the flow application (first clearing the structs) */
shr_error_e
ctest_flow_ut_application_lif_entry_create(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    void *app_data,
    bcm_flow_special_fields_t * special_fields)
{
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        rv = bcm_flow_terminator_info_create(unit, flow_handle_info, (bcm_flow_terminator_info_t *) app_data,
                                             special_fields);
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        rv = bcm_flow_initiator_info_create(unit, flow_handle_info, (bcm_flow_initiator_info_t *) app_data,
                                            special_fields);
    }

    if (rv != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FAILURE on flow app %s, type = %d\n", flow_app_info->app_name,
                     flow_app_info->flow_app_type);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e ctest_flow_ut_struct_special_fields_verify(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    uint8 update_valued,
    bcm_flow_special_fields_t * special_fields,
    flow_special_fields_e special_fields_added[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS]);

static int
flow_term_entry_verify_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *user_data)
{
    const dnx_flow_app_config_t *flow_app_info;
    flow_traverse_data_t *traverse_data;

    SHR_FUNC_INIT_VARS(unit);

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);

    traverse_data = user_data;

    SHR_IF_ERR_EXIT(ctest_flow_ut_struct_special_fields_verify(unit, flow_app_info, 0, FALSE,
                                                               special_fields, traverse_data->special_fields_added));

exit:
    SHR_FUNC_EXIT;
}

static int
flow_init_entry_verify_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    void *user_data)
{
    const dnx_flow_app_config_t *flow_app_info;
    flow_traverse_data_t *traverse_data;

    SHR_FUNC_INIT_VARS(unit);

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);

    traverse_data = user_data;

    SHR_IF_ERR_EXIT(ctest_flow_ut_struct_special_fields_verify(unit, flow_app_info, 0, FALSE,
                                                               special_fields, traverse_data->special_fields_added));

exit:
    SHR_FUNC_EXIT;
}

static int
flow_match_entry_verify_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * special_fields,
    void *user_data)
{
    const dnx_flow_app_config_t *flow_app_info;
    flow_traverse_data_t *traverse_data;

    SHR_FUNC_INIT_VARS(unit);

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);

    traverse_data = user_data;

    SHR_IF_ERR_EXIT(ctest_flow_ut_struct_special_fields_verify(unit, flow_app_info, 0, FALSE,
                                                               special_fields, traverse_data->special_fields_added));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ctest_flow_ut_application_traverse(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    flow_special_fields_e special_field_added[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS])
{
    flow_traverse_data_t traverse_data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memcpy(&(traverse_data.special_fields_added), special_field_added,
               sizeof(flow_special_fields_e) * BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS);

    switch (flow_app_info->flow_app_type)
    {
        case FLOW_APP_TYPE_TERM:
            SHR_IF_ERR_EXIT(bcm_flow_terminator_info_traverse
                            (unit, flow_handle_info, flow_term_entry_verify_cb, &traverse_data));
            break;
        case FLOW_APP_TYPE_INIT:
            SHR_IF_ERR_EXIT(bcm_flow_initiator_info_traverse
                            (unit, flow_handle_info, flow_init_entry_verify_cb, &traverse_data));
            break;
        case FLOW_APP_TYPE_TERM_MATCH:
        case FLOW_APP_TYPE_INIT_MATCH:

            SHR_IF_ERR_EXIT(bcm_flow_match_info_traverse
                            (unit, flow_handle_info, flow_match_entry_verify_cb, &traverse_data));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error, Unsupported app-type value (%d) for application %s",
                         flow_app_info->flow_app_type, flow_app_info->app_name);
    }

exit:
    SHR_FUNC_EXIT;
}

/** perform a get on the flow application (first clearing the structs) */
shr_error_e
ctest_flow_ut_lif_application_entry_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    void *app_data,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(special_fields, 0, sizeof(bcm_flow_special_fields_t));
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        sal_memset(app_data, 0, sizeof(bcm_flow_terminator_info_t));
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_get
                        (unit, flow_handle_info, (bcm_flow_terminator_info_t *) app_data, special_fields));
    }
    else
    {
        sal_memset(app_data, 0, sizeof(bcm_flow_initiator_info_t));
        SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get
                        (unit, flow_handle_info, (bcm_flow_initiator_info_t *) app_data, special_fields));
    }

exit:
    SHR_FUNC_EXIT;
}

/*************
* FUNCTIONS *
*************/
/** returning a payload LIF app for the match application, it is required in order to created the lif before
 *  calling the match application test */
static shr_error_e
ctest_flow_ut_match_payload_app_find(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_t * match_payload_flow_handle_id,
    dnx_flow_app_config_t ** payload_flow_app_info)
{
    int is_table_active;
    const dnx_flow_app_config_t *temp_payload_flow_app_info;
    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_info->match_payload_apps[0][0])
    {
        SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                        (unit, flow_app_info->match_payload_apps[0], match_payload_flow_handle_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "No match payload LIF application was found for application %s\n",
                     flow_app_info->app_name);
    }

    temp_payload_flow_app_info = dnx_flow_app_info_get(unit, (*match_payload_flow_handle_id));

    *payload_flow_app_info = (dnx_flow_app_config_t *) temp_payload_flow_app_info;

    if (FLOW_APP_IS_NOT_VALID((*payload_flow_app_info)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "LIF related APP %s is not valid \n", (*payload_flow_app_info)->app_name);
    }

    SHR_IF_ERR_EXIT(dbal_tables_is_table_active
                    (unit, FLOW_LIF_DBAL_TABLE_GET((*payload_flow_app_info)), &is_table_active));
    if (!is_table_active)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "For Application %s LIF DBAL table is not active \n",
                     (*payload_flow_app_info)->app_name);
    }

exit:
    SHR_FUNC_EXIT;
}

/** dynamic init of flow ut tests this function create dynamiclly a test for all existing applications   */
shr_error_e
ctest_flow_ut_dyn_tests_creation(
    int unit,
    rhlist_t * test_list)
{
    int app_idx, num_apps = dnx_flow_number_of_apps();

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Don't exit if deliberately including adapter unsupported tests
     */
    if (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_tests_supported)
        && !test_list->include_unsupported)
    {
        SHR_EXIT();
    }

    for (app_idx = FLOW_APP_FIRST_IDX; app_idx < num_apps; app_idx++)
    {
        int ut_flags = 0;
        const dnx_flow_app_config_t *flow_app_info;
        char ut_name[FLOW_STR_MAX * 2] = "";
                                                 /** consist the test name only */
        char ut_arg[FLOW_STR_MAX * 2] = "application=";
                                                         /** consist the full args to run the test name=xxx table=yyy*/

        flow_app_info = dnx_flow_app_info_get(unit, app_idx);

        /** add tests to pre-commit for spec applications */
        if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_GTP_TERMINATOR, FLOW_STR_MAX) ||
            !sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_GTP_INITIATOR, FLOW_STR_MAX) ||
            !sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_GTP_TERM_MATCH, FLOW_STR_MAX))
        {
            utilex_prime_flag_set(&ut_flags, CTEST_PRECOMMIT);
        }

        if (FLOW_APP_IS_NOT_VALID(flow_app_info))
        {
            continue;
        }

        sal_snprintf(ut_name, FLOW_STR_MAX * 2, "%s_%s ", ut_name, flow_app_info->app_name);
        sal_snprintf(ut_arg, FLOW_STR_MAX * 2, "%s%s ", ut_arg, flow_app_info->app_name);

        SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, ut_name, ut_arg, ut_flags), "Add test failed");
    }

exit:
    SHR_FUNC_EXIT;
}

/** translates common field with termination type to dbal field */
static shr_error_e
ctest_flow_common_field_type_term_to_dbal_field(
    int unit,
    int field_enabler,
    dbal_fields_e dbal_fields[CTEST_FLOW_SIZE_OF_COMMON_RELATED_DBAL_FIELDS])
{
    SHR_FUNC_INIT_VARS(unit);

    /** Since most have only 1 dbal field mapped, this cover most cases, and the rest of the cases will dealt set the end inside them */
    dbal_fields[1] = DBAL_FIELD_EMPTY;

    switch (field_enabler)
    {
        case BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID:
            dbal_fields[0] = DBAL_FIELD_VRF;
            dbal_fields[1] = DBAL_FIELD_FODO;
            dbal_fields[2] = DBAL_FIELD_EMPTY;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID:
            dbal_fields[0] = DBAL_FIELD_STAT_OBJECT_ID;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID:
            dbal_fields[0] = DBAL_FIELD_IN_LIF_PROFILE;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID:
            dbal_fields[0] = DBAL_FIELD_STAT_OBJECT_CMD;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID:
            dbal_fields[0] = DBAL_FIELD_QOS_PROFILE;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID:
            /** always DBAL_FIELD_PROPAGATION_PROFILE, if (ingress_ecn == bcmQosIngressEcnModelEligible) also DBAL_FIELD_ECN_MAPPING_PROFILE */
            dbal_fields[0] = DBAL_FIELD_PROPAGATION_PROFILE;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID:
            /** always DBAL_FIELD_DESTINATION and if (condition at line 1734) also DBAL_FIELD_GLOB_OUT_LIF */
            dbal_fields[0] = DBAL_FIELD_DESTINATION;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID:
            dbal_fields[0] = DBAL_FIELD_ACTION_PROFILE_IDX;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID:
            /** Should also have DBAL_FIELD_FODO, similar to VRF, but not added to avoid mapping of 2 common fields to a single dbal field */
            dbal_fields[0] = DBAL_FIELD_VSI;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID:
            dbal_fields[0] = DBAL_FIELD_IN_LIF_PROFILE;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_ID_VALID:
            dbal_fields[0] = DBAL_FIELD_PROTECTION_POINTER;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_STATE_VALID:
            dbal_fields[0] = DBAL_FIELD_PROTECTION_PATH;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID:
            dbal_fields[0] = DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID:
            dbal_fields[0] = DBAL_FIELD_LEARN_PAYLOAD_CONTEXT;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID:
            dbal_fields[0] = DBAL_FIELD_LEARN_ENABLE;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID:
            dbal_fields[0] = DBAL_FIELD_LIF_GENERIC_DATA_0;
            break;
        /** NOT implemented yet (line 3341) */
        case BCM_FLOW_TERMINATOR_ELEMENT_FIELD_CLASS_ID_VALID:
            dbal_fields[0] = DBAL_FIELD_EMPTY;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type input for this flow\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/** translates common field with initiator type to dbal field */
static shr_error_e
ctest_flow_common_field_type_init_to_dbal_field(
    int unit,
    int field_enabler,
    dbal_fields_e dbal_fields[CTEST_FLOW_SIZE_OF_COMMON_RELATED_DBAL_FIELDS])
{
    SHR_FUNC_INIT_VARS(unit);

    dbal_fields[0] = DBAL_FIELD_EMPTY;
    dbal_fields[1] = DBAL_FIELD_EMPTY;

    switch (field_enabler)
    {
        case BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID:
            dbal_fields[0] = DBAL_FIELD_STAT_OBJECT_ID;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID:
            dbal_fields[0] = DBAL_FIELD_STAT_OBJECT_CMD;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID:
            dbal_fields[0] = DBAL_FIELD_QOS_PROFILE;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID:
            /** BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID returns DBAL_FIELD_NEXT_OUTLIF_POINTER on condition (line 1857) */
            dbal_fields[0] = DBAL_FIELD_NEXT_OUTLIF_POINTER;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID:
            /** always set DBAL_FIELD_ENCAP_QOS_MODEL, DBAL_FIELD_TTL_MODE is conditioned on the flag valid_elements_set (line 2683) */
            dbal_fields[0] = DBAL_FIELD_ENCAP_QOS_MODEL;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID:
            dbal_fields[0] = DBAL_FIELD_ACTION_PROFILE;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_MTU_PROFILE_VALID:
            dbal_fields[0] = DBAL_FIELD_MTU_PROFILE;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID:
            dbal_fields[0] = DBAL_FIELD_OUT_LIF_PROFILE;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_ID_VALID:
            dbal_fields[0] = DBAL_FIELD_PROTECTION_POINTER;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_STATE_VALID:
            dbal_fields[0] = DBAL_FIELD_PROTECTION_PATH;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_TPID_CLASS_ID_VALID:
            dbal_fields[0] = DBAL_FIELD_EGRESS_LLVP_PROFILE;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_CLASS_ID_VALID:
            dbal_fields[0] = DBAL_FIELD_ESEM_NAME_SPACE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid input for this flow (0x%x)\n", field_enabler);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/** translates common field to dbal field */
static shr_error_e
ctest_flow_common_field_to_dbal_field(
    int unit,
    dnx_flow_app_type_e app_type,
    int field_enabler,
    dbal_fields_e dbal_fields[CTEST_FLOW_SIZE_OF_COMMON_RELATED_DBAL_FIELDS])
{
    SHR_FUNC_INIT_VARS(unit);

    switch (app_type)
    {
        case FLOW_APP_TYPE_TERM:
            SHR_IF_ERR_EXIT(ctest_flow_common_field_type_term_to_dbal_field(unit, field_enabler, dbal_fields));
            break;
        case FLOW_APP_TYPE_INIT:
            SHR_IF_ERR_EXIT(ctest_flow_common_field_type_init_to_dbal_field(unit, field_enabler, dbal_fields));
            break;
        case FLOW_APP_TYPE_TERM_MATCH:
        case FLOW_APP_TYPE_INIT_MATCH:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Common fields aren't supported for Match applications\n");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid application type input for this flow\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static void
ctest_flow_ut_common_field_l2_ingress_info_service_type_legal_value_get(
    int unit,
    uint8 result_type,
    int *value)
{

    switch (result_type)
    {
        case DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP:
        case DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE:
        case DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA:
        case DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD:
            if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled))
            {
                *value = bcmFlowServiceTypeMultiPoint;
            }
            else
            {
                *value = CTEST_FLOW_SKIP_FIELD;
            }
            break;
        /** the following result_types don't have service_type dbal field and do not exist when service_type is part of the inlif profile */
        case DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA_AND_PROTECTION:
        case DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_RCH_CONTROL_LIF:
            *value = CTEST_FLOW_SKIP_FIELD;
            break;
        default:
            *value = bcmFlowServiceTypeCrossConnect;
            break;
    }
}

/** get common field with terminator type with a legal value  */
static shr_error_e
ctest_flow_ut_common_field_type_term_legal_value_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    int field_enabler,
    int *legal_value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (field_enabler)
    {
        case BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID:
        case BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID:
        case BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID:
        case BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID:
        case BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID:
        case BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_STATE_VALID:
        case BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID:
        case BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID:
            *legal_value = 1;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID:
            if (sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, FLOW_STR_MAX))
            {
                *legal_value = bcmFlowServiceTypeCrossConnect;
            }
            else
            {
                /** for vlan_port_ll_terminator, value getting require logic */
                ctest_flow_ut_common_field_l2_ingress_info_service_type_legal_value_get(unit, result_type, legal_value);
            }
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID:
            if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_GTP_TERMINATOR, FLOW_STR_MAX))
            {
                *legal_value = 1;
            }
            else
            {
                *legal_value = ((1 << dnx_data_port.general.ingress_vlan_domain_size_get(unit)) - 2);
            }

            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_ID_VALID:
            BCM_FAILOVER_SET(*legal_value, 1, BCM_FAILOVER_TYPE_INGRESS);
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID:
            BCM_GPORT_FORWARD_PORT_SET(*legal_value, 0x0);
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID:
        case BCM_FLOW_TERMINATOR_ELEMENT_FIELD_CLASS_ID_VALID:
            *legal_value = CTEST_FLOW_SKIP_FIELD;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID:
            /** setting to value 0x10030001 - profile 1 (16lsb) and flags phb, remark and ingress (16msb) */
            /** setting to qos profile 1 */
            *legal_value = 1;
            /** setting the flags */
            DNX_QOS_PHB_MAP_SET(*legal_value);
            DNX_QOS_REMARK_MAP_SET(*legal_value);
            DNX_QOS_INGRESS_MAP_SET(*legal_value);
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID:
            /** setting to value 0x16000001 */
            BCM_GPORT_TRAP_SET(*legal_value, 0x1, 0, 0);
            if (!dnx_data_flow.trap.feature_get(unit, dnx_data_flow_trap_flow_in_use))
            {
             
                *legal_value = CTEST_FLOW_SKIP_FIELD;
            }
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID:
            BCM_GPORT_FORWARD_PORT_SET(*legal_value, 0x1);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type input for this flow\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/** get common field with initiator type with a legal value  */
static shr_error_e
ctest_flow_ut_common_field_type_init_legal_value_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    int field_enabler,
    int *legal_value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (field_enabler)
    {
        case BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID:
        case BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID:
        case BCM_FLOW_INITIATOR_ELEMENT_MTU_PROFILE_VALID:
        case BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_STATE_VALID:
        case BCM_FLOW_INITIATOR_ELEMENT_TPID_CLASS_ID_VALID:
        case BCM_FLOW_INITIATOR_ELEMENT_CLASS_ID_VALID:
            *legal_value = 1;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_ID_VALID:
            BCM_FAILOVER_SET(*legal_value, 1, BCM_FAILOVER_TYPE_ENCAP);
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID:
            /** setting to value 0x16000C01 */
            BCM_GPORT_TRAP_SET(*legal_value, 0xc01, 0, 0);
            if (!dnx_data_flow.trap.feature_get(unit, dnx_data_flow_trap_flow_in_use))
            {
             
                *legal_value = CTEST_FLOW_SKIP_FIELD;
            }
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID:
            if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_ARP_INITIATOR, FLOW_STR_MAX))
            {
                *legal_value = bcmQosEgressModelUniform;
            }
            else
            {
                *legal_value = bcmQosEgressModelPipeNextNameSpace;
            }
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID:
            /** DNX_OUT_LIF_PROFILE_DEFAULT = 0 */
            *legal_value = 0;
            if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
            {
                
                *legal_value = CTEST_FLOW_SKIP_FIELD;
            }
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID:
            *legal_value = 1;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID:
            *legal_value = 0;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type input for this flow\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/** get common field with a legal value */
shr_error_e
ctest_flow_ut_common_field_legal_value_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    int field_enabler,
    int *legal_value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (flow_app_info->flow_app_type)
    {
        case FLOW_APP_TYPE_TERM:
            SHR_IF_ERR_EXIT(ctest_flow_ut_common_field_type_term_legal_value_get(unit, flow_app_info,
                                                                                 result_type, field_enabler,
                                                                                 legal_value));
            break;
        case FLOW_APP_TYPE_INIT:
            SHR_IF_ERR_EXIT(ctest_flow_ut_common_field_type_init_legal_value_get(unit, flow_app_info,
                                                                                 result_type, field_enabler,
                                                                                 legal_value));
            break;
        case FLOW_APP_TYPE_TERM_MATCH:
        case FLOW_APP_TYPE_INIT_MATCH:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Common fields aren't supported for Match applications\n");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid application type input for this flow\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/** check if the common field can is valid for update update */
static shr_error_e
ctest_flow_ut_common_field_value_updating_valid(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    int enabler,
    uint8 *update_valid)
{
    shr_error_e rv;
    dbal_fields_e common_fields_related_dbal_fields[CTEST_FLOW_SIZE_OF_COMMON_RELATED_DBAL_FIELDS] = { 0 };
    uint8 dbal_field_idx;

    SHR_FUNC_INIT_VARS(unit);

    /** assume the update is valid unless meeting with future conditions */
    (*update_valid) = TRUE;

    if (FLOW_APP_IS_MATCH(flow_app_info->flow_app_type))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Common fields aren't supported for Match applications\n");
    }

    /** get relevant DBAL FIELD to set for the test */
    SHR_IF_ERR_EXIT(ctest_flow_common_field_to_dbal_field(unit, flow_app_info->flow_app_type,
                                                          enabler, common_fields_related_dbal_fields));

    for (dbal_field_idx = 0; dbal_field_idx < CTEST_FLOW_SIZE_OF_COMMON_RELATED_DBAL_FIELDS; dbal_field_idx++)
    {
        int field_size = 0;

        if (common_fields_related_dbal_fields[dbal_field_idx] == DBAL_FIELD_EMPTY)
        {
            break;
        }
        rv = dbal_tables_field_size_get(unit, flow_app_info->flow_table[0],
                                        common_fields_related_dbal_fields[dbal_field_idx],
                                        0, result_type, 0, &field_size);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

        if (rv == _SHR_E_NOT_FOUND)
        {
            continue;
        }

        /** cannot update value for 1bit sized fields */
        if (field_size == 1)
        {
            (*update_valid) = FALSE;
            break;
        }
    }

    /** pass on all special cases for them we cannot update the value */
        /** l3_intf require a more complex value and as of today we set it to 0 to disable it, so cannot change it */
    if (((flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
         && (enabler == BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID)) ||
        /** action_gport require a more complex value building, skip updating its value for now */
        ((flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
         && (enabler == BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID)) ||
        /** for arp_initiator, qos_egress_model value effect other fields so skip it */
        ((!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_ARP_INITIATOR, FLOW_STR_MAX)) &&
         (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
         && (enabler == BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID)))
    {
        (*update_valid) = FALSE;
    }

    /** cannot update value for 1bit sized fields */
    if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_GTP_TERMINATOR, FLOW_STR_MAX)
        && (enabler == BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID))
    {
        (*update_valid) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/** get common field with a legal value */
shr_error_e
ctest_flow_ut_common_field_value_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    uint8 update_value,
    int field_enabler,
    int *legal_value,
    uint8 *update_valid)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_flow_ut_common_field_legal_value_get(unit, flow_app_info, result_type,
                                                               field_enabler, legal_value));

    if (update_value)
    {
        SHR_IF_ERR_EXIT(ctest_flow_ut_common_field_value_updating_valid(unit, flow_app_info, result_type,
                                                                        field_enabler, update_valid));

        if ((*update_valid) == TRUE)
        {
            (*legal_value)++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static void
ctest_flow_ut_special_field_ipv6_tunnel_init_tunnel_type_legal_value_get(
    int unit,
    uint8 result_type,
    bcm_flow_special_field_t * value)
{
    if (result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_UDP)
    {
        value->symbol = bcmTunnelTypeUdp6;
    }
    else if (result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_RAW)
    {
        value->symbol = bcmTunnelTypeIpAnyIn6;
    }
    else if ((result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_GRE) ||
             (result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_GENEVE))
    {
        value->symbol = bcmTunnelTypeGreAnyIn6;
    }
    else if (result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_VXLAN)
    {
        value->symbol = bcmTunnelTypeVxlan6;
    }
    else        /* result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_VXLAN_ECN */
    {
        value->symbol = bcmTunnelTypeVxlan6Gpe;
    }
}

static void
ctest_flow_ut_special_field_ipv4_tunnel_init_tunnel_type_legal_value_get(
    int unit,
    uint8 result_type,
    bcm_flow_special_field_t * value)
{
    if (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_UDP)
    {
        value->symbol = bcmTunnelTypeUdp;
    }
    else if ((result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL) ||
             (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_COPY_INNER) ||
             (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_BFD_IPV4_EXTRA_SIP))
    {
        value->symbol = bcmTunnelTypeIpAnyIn4;
    }
    else if (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_GRE_GENEVE)
    {
        value->symbol = bcmTunnelTypeGreAnyIn4;
    }
    else if ((result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_VXLAN_GPE) ||
             (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN))
    {
        value->symbol = bcmTunnelTypeVxlan;
    }
    else        /* result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN */
    {
        value->symbol = bcmTunnelTypeVxlanGpe;
    }
}

static shr_error_e
ctest_flow_ut_special_field_legal_port_get(
    int unit,
    uint8 request_sys_port,
    uint32 *port)
{
    dnx_algo_port_info_s port_info_i;
    bcm_port_t port_i;
    bcm_pbmp_t logical_ports;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_PP, 0, &logical_ports));
    BCM_PBMP_ITER(logical_ports, port_i)
    {
        if (port_i == 0)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port_i, &port_info_i));
        if (port_info_i.port_type == DNX_ALGO_PORT_TYPE_NIF_ETH)
        {
            *port = port_i;
            break;
        }
    }

    if (request_sys_port)
    {
        SHR_IF_ERR_EXIT(bcm_stk_gport_sysport_get(unit, port_i, &port_i));
        *port = port_i;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Allocate MPLS gport
 * Note:
 * This is needed for the following match application (as the key is GLOB_IN_LIF):
 *    - VLAN_PORT_AC_NATIVE_LIF_SCOPED_0_VLAN_TERM_MATCH
 *    - VLAN_PORT_AC_NATIVE_LIF_SCOPED_1_VLAN_TERM_MATCH
 *    - VLAN_PORT_AC_NATIVE_LIF_SCOPED_2_VLAN_TERM_MATCH
 */
static shr_error_e
ctest_flow_ut_special_field_glob_in_lif_create(
    int unit,
    const dnx_flow_app_config_t * flow_app_info)
{
    int vpn_id, ii = 0, glob_in_lif_needed = FALSE;

    bcm_mpls_vpn_config_t vpn_info;
    bcm_mpls_port_t mpls_port;

    SHR_FUNC_INIT_VARS(unit);

    while (flow_app_info->special_fields[ii] != FLOW_S_F_EMPTY)
    {
        if (flow_app_info->special_fields[ii++] == FLOW_S_F_GLOB_IN_LIF)
        {
            glob_in_lif_needed = TRUE;
            break;
        }
    }

    if (!glob_in_lif_needed)
    {
        SHR_EXIT();
    }

    bcm_mpls_vpn_config_t_init(&vpn_info);
    bcm_mpls_port_t_init(&mpls_port);
    vpn_id = 100;

    vpn_info.vpn = vpn_id;
    vpn_info.flags = BCM_MPLS_VPN_VPLS | BCM_MPLS_VPN_WITH_ID;
    vpn_info.broadcast_group = vpn_id;
    vpn_info.unknown_multicast_group = vpn_id;
    vpn_info.unknown_unicast_group = vpn_id;

    SHR_IF_ERR_EXIT(bcm_mpls_vpn_id_create(unit, &vpn_info));

    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY;
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = 0x100;
    mpls_port.egress_tunnel_if = 0x2000CCCE;
    mpls_port.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    mpls_port.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    mpls_port.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;

    SHR_IF_ERR_EXIT(bcm_mpls_port_add(unit, vpn_id, &mpls_port));

    gport_pwe_ingress_lif_scoped = mpls_port.mpls_port_id;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_ut_special_field_glob_in_lif_delete(
    int unit,
    const dnx_flow_app_config_t * flow_app_info)
{
    int vpn_id, ii = 0, glob_in_lif_needed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    while (flow_app_info->special_fields[ii] != FLOW_S_F_EMPTY)
    {
        if (flow_app_info->special_fields[ii++] == FLOW_S_F_GLOB_IN_LIF)
        {
            glob_in_lif_needed = TRUE;
            break;
        }
    }

    if (!glob_in_lif_needed)
    {
        SHR_EXIT();
    }

    vpn_id = 100;

    SHR_IF_ERR_EXIT(bcm_mpls_port_delete(unit, vpn_id, gport_pwe_ingress_lif_scoped));

    SHR_IF_ERR_EXIT(bcm_mpls_vpn_id_destroy(unit, vpn_id));

exit:
    SHR_FUNC_EXIT;
}

/** get special field with a legal value */
shr_error_e
ctest_flow_ut_special_field_legal_value_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * value)
{
    uint8 idx;
    const flow_special_field_info_t *special_field_info;

    SHR_FUNC_INIT_VARS(unit);

    /** zero the value to insure no leftovers are there */
    sal_memset(value, 0x0, sizeof(bcm_flow_special_field_t));

    /** enabler special fields require no value */
    SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, special_field_id, &special_field_info));
    if (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_ENABLER)
    {
        SHR_EXIT();
    }

    switch (special_field_id)
    {
        case FLOW_S_F_TUNNEL_ENDPOINT_IDENTIFIER:
        case FLOW_S_F_MAPPED_PORT:
        case FLOW_S_F_VLAN_DOMAIN:
        case FLOW_S_F_VID:
        case FLOW_S_F_S_VID:
        case FLOW_S_F_S_VID_1:
        case FLOW_S_F_S_VID_2:
        case FLOW_S_F_C_VID:
        case FLOW_S_F_C_VID_1:
        case FLOW_S_F_C_VID_2:
        case FLOW_S_F_VID_OUTER_VLAN:
        case FLOW_S_F_VID_INNER_VLAN:
        case FLOW_S_F_PCP_DEI_OUTER_VLAN:
        case FLOW_S_F_VRF:
        case FLOW_S_F_VSI:
        case FLOW_S_F_MPLS_ENTROPY_LABEL:
        case FLOW_S_F_MPLS_INCLUSIVE_MULTICAST_LABEL:
        case FLOW_S_F_MPLS_EXPECT_BOS:
        case FLOW_S_F_MPLS_TRAP_TTL_0:
        case FLOW_S_F_MPLS_TRAP_TTL_1:
        case FLOW_S_F_BIER_BIFT_ID:
        case FLOW_S_F_SRC_UDP_PORT:
        case FLOW_S_F_DST_UDP_PORT:
        case FLOW_S_F_QOS_DSCP:
        case FLOW_S_F_QOS_EXP:
        case FLOW_S_F_QOS_PRI:
        case FLOW_S_F_DUAL_HOMING:
        case FLOW_S_F_MPLS_PLATFORM_NAMESPACE:
        case FLOW_S_F_NOF_OF_LAYERS_TO_TERMINATE:
        case FLOW_S_F_REFLECTOR_PROCESS_TYPE:
            value->shr_var_uint32 = 1;
            break;
        case FLOW_S_F_MPLS_LABEL:
        case FLOW_S_F_MPLS_LABEL_2:
        case FLOW_S_F_MATCH_LABEL:
            value->shr_var_uint32 = 20;
            break;
        case FLOW_S_F_PORT:
        case FLOW_S_F_IN_PORT:
            SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_legal_port_get(unit, FALSE, &(value->shr_var_uint32)));
            break;
        case FLOW_S_F_SYSTEM_PORT_MATCH:
            SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_legal_port_get(unit, TRUE, &(value->shr_var_uint32)));
            break;
        case FLOW_S_F_QOS_CFI:
        case FLOW_S_F_GTP_PDU_TYPE:
            /** This is because CIF values are only 0 and 1 (according to macros is bcm_int qos.h) */
            /** now GTP PDU type only support 0 and 1 */
            value->shr_var_uint32 = 0;
            break;
        case FLOW_S_F_MPLS_CONTROL_WORD_ENABLE:
            
            value->shr_var_uint32 = 0;
            break;
        case FLOW_S_F_IPV4_SIP:
        case FLOW_S_F_IPV4_DIP:
        case FLOW_S_F_IPV4:
            value->shr_var_uint32 = 0xA1000011;
            break;
        case FLOW_S_F_IPV4_SIP_MASK:
        case FLOW_S_F_IPV4_DIP_MASK:
            /*
             * note that the mask value is not identical to the sip/dip value this is done to validate that the mask
             * working
             */
            value->shr_var_uint32 = 0xA1F00011;
            break;
        case FLOW_S_F_VLAN_EDIT_PROFILE:
        case FLOW_S_F_VLAN_EDIT_VID_1:
        case FLOW_S_F_VLAN_EDIT_VID_2:
            value->shr_var_uint32 = 5;
            break;
        case FLOW_S_F_VLAN_EDIT_PCP_DEI_PROFILE:
            value->shr_var_uint32 = 5;
            DNX_QOS_VLAN_PCP_MAP_SET(value->shr_var_uint32);
            break;
        case FLOW_S_F_VSI_ASSIGNMENT_MODE:
        case FLOW_S_F_MPLS_ENTROPY_LABEL_INDICATION:
        case FLOW_S_F_NEXT_LAYER_NETWORK_DOMAIN:
            value->shr_var_uint32 = 0;
            break;
        case FLOW_S_F_BITSTR:
            for (idx = 0; idx < BCM_FLOW_SPECIAL_FIELD_UIN32_ARR_MAX_SIZE; idx++)
            {
                value->shr_var_uint32_arr[idx] = 1;
            }
            break;
        case FLOW_S_F_BIER_BSL:
        case FLOW_S_F_BIER_BFIR_ID:
        case FLOW_S_F_BIER_RSV:
        case FLOW_S_F_BIER_OAM:
            value->shr_var_uint32 = 0;
            break;
        case FLOW_S_F_BFR_BIT_STR:
            for (idx = 0; idx < BCM_FLOW_SPECIAL_FIELD_UIN32_ARR_MAX_SIZE; idx++)
            {
                value->shr_var_uint32_arr[idx] = 1;
            }
            break;
        case FLOW_S_F_IPV6_DIP:
            for (idx = 0; idx < BCM_FLOW_SPECIAL_FIELD_UIN8_ARR_MAX_SIZE; idx++)
            {
                value->shr_var_uint8_arr[idx] = 0x1;
            }
            break;
        case FLOW_S_F_IPV6_DIP_MASK:
        case FLOW_S_F_IPV6_MASK:
            for (idx = 0; idx < BCM_FLOW_SPECIAL_FIELD_UIN8_ARR_MAX_SIZE; idx++)
            {
                value->shr_var_uint8_arr[idx] = 0xff;
            }
            break;
        case FLOW_S_F_IPV6_SIP:
        case FLOW_S_F_IPV6:
            for (idx = 0; idx < BCM_FLOW_SPECIAL_FIELD_UIN8_ARR_MAX_SIZE; idx++)
            {
                value->shr_var_uint8_arr[idx] = 0x1;
            }
            break;
        case FLOW_S_F_IPV6_TUNNEL_TYPE:
            if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_IPV6_INITIATOR, FLOW_STR_MAX))
            {
                ctest_flow_ut_special_field_ipv6_tunnel_init_tunnel_type_legal_value_get(unit, result_type, value);
            }
            else
            {
                value->symbol = bcmTunnelTypeGreAnyIn6;
            }
            break;
        case FLOW_S_F_IPV4_TUNNEL_TYPE:
            if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_IPV4_INITIATOR, FLOW_STR_MAX))
            {
                ctest_flow_ut_special_field_ipv4_tunnel_init_tunnel_type_legal_value_get(unit, result_type, value);
            }
            else
            {
                value->symbol = bcmTunnelTypeIpAnyIn4;
            }
            break;
        case FLOW_S_F_LAYER_TYPE:
            value->symbol = 1;
            break;
        case FLOW_S_F_TUNNEL_CLASS:
            value->shr_var_uint32 = bcmTunnelTypeGreAnyIn6;
            break;
        case FLOW_S_F_DST_MAC_ADDRESS:
        case FLOW_S_F_REFLECTOR_SRC_MAC:
            for (idx = 0; idx < 6; idx++)
            {
                value->shr_var_uint8_arr[idx] = idx;
            }
            break;
        case FLOW_S_F_SRC_MAC_ADDRESS:
            for (idx = 0; idx < 6; idx++)
            {
                value->shr_var_uint8_arr[idx] = idx;
            }
            break;
        case FLOW_S_F_MPLS_VCCV_TYPE:
            value->symbol = bcmMplsPortControlChannelTtl;
            break;
        case FLOW_S_F_SRC_ENCAP_ID_MATCH:
            value->shr_var_uint32 = 0x50;
            break;
        case FLOW_S_F_QOS_TTL:
            /** currently being skipped as BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID is required to set two fields to check it */
            value->shr_var_uint32 = CTEST_FLOW_SKIP_FIELD;
            break;
        case FLOW_S_F_MPLS_LABEL_3:
        case FLOW_S_F_MPLS_LABEL_4:
        case FLOW_S_F_MPLS_LABEL_5:
        case FLOW_S_F_MPLS_LABEL_6:
        case FLOW_S_F_MPLS_LABEL_7:
        case FLOW_S_F_MPLS_LABEL_8:
            value->shr_var_uint32 = CTEST_FLOW_SKIP_FIELD;
            break;
        case FLOW_S_F_VSI_MATCH:
            value->shr_var_uint32 = 3;
            break;
        case FLOW_S_F_CLASS_ID_MATCH:
            value->shr_var_uint32 = 6;
            break;
        case FLOW_S_F_NAME_SPACE_MATCH:
        case FLOW_S_F_ESEM_NAME_SPACE:
            value->shr_var_uint32 = 1;
            break;
        case FLOW_S_F_GLOB_IN_LIF:

            value->shr_var_uint32 = gport_pwe_ingress_lif_scoped;
            break;
        case FLOW_S_F_VID_MATCH:
            value->shr_var_uint32 = 9;
            break;
        case FLOW_S_F_RCH_DESTINATION:
            BCM_GPORT_SYSTEM_PORT_ID_SET(value->shr_var_uint32, 13);
            break;
        case FLOW_S_F_RCH_CONTROL_VLAN_PORT:
            value->shr_var_uint32 = CTEST_FLOW_SKIP_FIELD;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "special field %s (%d) has no mapping\n",
                         dnx_flow_special_field_to_string(unit, special_field_id), special_field_id);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static void
ctest_flow_ut_common_fields_remove_ingress_info_duplication(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint32 *matching_common_fields_bitmap)
{
    /** checking if both l2_ingress info and l3_ingress info are set */
    if ((flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) &&
        ((*matching_common_fields_bitmap) & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) &&
        ((*matching_common_fields_bitmap) & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID))
    {
        /** since both l2_ingress and l3_ingress cannot be set - remove one of them */
        (*matching_common_fields_bitmap) &= (~BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID);
    }
}

/** this function gets the relevant params and updates the common field bitmap with fields which are mandatory to the flow app */
static void
ctest_flow_ut_app_mandatory_common_fields_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint32 *matching_common_fields_bitmap)
{
    if ((flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT) &&
        (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_MANDATORY_L3_INTF)))
    {
        /** add l3_intf */
        (*matching_common_fields_bitmap) |= BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID;
    }
}

/** this function gets the relevant params and creates an array of special fields which are mandatory to the flow app */
static shr_error_e
ctest_flow_ut_app_mandatory_and_unchangeable_special_fields_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    flow_special_fields_e mandatory_special_fields[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    uint32 *nof_special_fields)
{
    const flow_special_field_info_t *special_field_info;
    uint8 idx;

    SHR_FUNC_INIT_VARS(unit);

    (*nof_special_fields) = 0;

    /** pass on all fields and add to the list the field with mandatory indication */
    for (idx = 0; flow_app_info->special_fields[idx] != FLOW_S_F_EMPTY; idx++)
    {
        SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, flow_app_info->special_fields[idx], &special_field_info));
        /** if field is lif mandatory or unchangeable and flow app is lif, then it is mandatory */
        if (FLOW_APP_IS_LIF(flow_app_info->flow_app_type) &&
            (dnx_flow_special_field_is_ind_set(unit, special_field_info, FLOW_SPECIAL_FIELD_IND_LIF_MANDATORY) ||
             dnx_flow_special_field_is_ind_set(unit, special_field_info, FLOW_SPECIAL_FIELD_IND_LIF_UNCHANGEABLE)))
        {
            mandatory_special_fields[(*nof_special_fields)] = flow_app_info->special_fields[idx];
            (*nof_special_fields)++;
        }
        /** if field is match mandatory or unchangeable and flow app is match, then it is mandatory */
        if (FLOW_APP_IS_MATCH(flow_app_info->flow_app_type) &&
            (dnx_flow_special_field_is_ind_set(unit, special_field_info, FLOW_SPECIAL_FIELD_IND_MATCH_MANDATORY) ||
             dnx_flow_special_field_is_ind_set(unit, special_field_info, FLOW_SPECIAL_FIELD_IND_MATCH_UNCHANGEABLE)))
        {
            mandatory_special_fields[(*nof_special_fields)] = flow_app_info->special_fields[idx];
            (*nof_special_fields)++;
        }
    }

    if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_IPV4_INITIATOR, FLOW_STR_MAX))
    {
        /** in case of udp */
        if (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_UDP)
        {
            mandatory_special_fields[(*nof_special_fields)] = FLOW_S_F_SRC_UDP_PORT;
            mandatory_special_fields[(*nof_special_fields) + 1] = FLOW_S_F_DST_UDP_PORT;
            (*nof_special_fields) += 2;
        }
        /** in case of gre or geneve */
        if (result_type == DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL_GRE_GENEVE)
        {
            mandatory_special_fields[(*nof_special_fields)] = FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB;
            (*nof_special_fields)++;
        }
    }
    else if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_IPV6_INITIATOR, FLOW_STR_MAX))
    {
        /** in case of udp */
        if (result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_UDP)
        {
            mandatory_special_fields[(*nof_special_fields)] = FLOW_S_F_SRC_UDP_PORT;
            mandatory_special_fields[(*nof_special_fields) + 1] = FLOW_S_F_DST_UDP_PORT;
            (*nof_special_fields) += 2;
        }
        /** in case of gre or geneve */
        if ((result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_GRE) ||
            (result_type == DBAL_RESULT_TYPE_EEDB_IPV6_TUNNEL_ETPS_IPV6_TUNNEL_GENEVE))
        {
            mandatory_special_fields[(*nof_special_fields)] = FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB;
            (*nof_special_fields)++;
        }
    }
    else if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_MPLS_PORT_INITIATOR, FLOW_STR_MAX))
    {
        mandatory_special_fields[(*nof_special_fields)] = FLOW_S_F_MPLS_ENTROPY_LABEL;
        (*nof_special_fields)++;
    }
    else if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_MPLS_TUNNEL_INITIATOR, FLOW_STR_MAX) ||
             !sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_MPLS_EVPN_INITIATOR, FLOW_STR_MAX))
    {
        
        mandatory_special_fields[(*nof_special_fields)] = FLOW_S_F_MPLS_ENTROPY_LABEL;
        (*nof_special_fields)++;
        mandatory_special_fields[(*nof_special_fields)] = FLOW_S_F_MPLS_ENTROPY_LABEL_INDICATION;
        (*nof_special_fields)++;
    }

exit:
    SHR_FUNC_EXIT;
}

/** pass on all flow app special fields, and add if required the un-mapped fields */
static shr_error_e
ctest_flow_ut_app_unmapped_special_fields_add(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    uint32 matching_common_fields_bitmap,
    dbal_fields_e dbal_field_in_table[FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS + BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    uint32 nof_dbal_fields_in_table,
    flow_special_fields_e flow_special_fields_ids[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    uint32 *nof_special_fields)
{
    uint8 idx;
    uint8 qos_params_required = FALSE;
    const flow_special_field_info_t *special_field_info;

    SHR_FUNC_INIT_VARS(unit);

    /** check if qos should be added */
    if ((flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT) &&
        (matching_common_fields_bitmap & BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID))
    {
        for (idx = 0; idx < nof_dbal_fields_in_table; idx++)
        {
            if (dbal_field_in_table[idx] == DBAL_FIELD_NWK_QOS_IDX)
            {
                qos_params_required = TRUE;
                break;
            }
        }
    }

    for (idx = 0; flow_app_info->special_fields[idx] != FLOW_S_F_EMPTY; idx++)
    {
        SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, flow_app_info->special_fields[idx], &special_field_info));

        /** if special field is not mapped to empty - skip it */
        if (special_field_info->mapped_dbal_field != DBAL_FIELD_EMPTY)
        {
            continue;
        }

        /** decide whether to add special field or not */
        if ((flow_app_info->special_fields[idx] == FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB) ||
            (flow_app_info->special_fields[idx] == FLOW_S_F_IPVX_INIT_GRE_WITH_SN))
        {
            /*
             * currently FLOW_S_F_IPVX_INIT_GRE_WITH_SN is never added FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB is added by
             * the mandatory fields function 
             */
            continue;
        }
        else if (flow_app_info->special_fields[idx] == FLOW_S_F_QOS_TTL)
        {
            /** currently being skipped as BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID is required to set two fields to check it */
            continue;
        }

        /** do not add qos params if they are not required */
        else if ((qos_params_required == FALSE) &&
                 ((flow_app_info->special_fields[idx] == FLOW_S_F_QOS_DSCP) ||
                  (flow_app_info->special_fields[idx] == FLOW_S_F_QOS_EXP) ||
                  (flow_app_info->special_fields[idx] == FLOW_S_F_QOS_PRI) ||
                  (flow_app_info->special_fields[idx] == FLOW_S_F_QOS_CFI)))
        {
            continue;
        }

        /** most cases - always add the unmapped special field */
        else
        {
            flow_special_fields_ids[(*nof_special_fields)] = flow_app_info->special_fields[idx];
            (*nof_special_fields)++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_ut_src_addr_profile_special_field_add(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    dbal_fields_e dbal_field_in_table[FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS + BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    uint32 nof_dbal_fields_in_table,
    flow_special_fields_e flow_special_fields_ids[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    uint32 *nof_special_fields)
{
    uint8 fields_idx;
    const flow_special_field_info_t *special_field_info;

    SHR_FUNC_INIT_VARS(unit);

    /** check if DBAL_FIELD_SOURCE_IDX exists, if not - src_address_profile should NOT be set */
    for (fields_idx = 0; fields_idx < nof_dbal_fields_in_table; fields_idx++)
    {
        if (dbal_field_in_table[fields_idx] == DBAL_FIELD_SOURCE_IDX)
        {
            break;
        }
    }
    if (fields_idx == nof_dbal_fields_in_table)
    {
        SHR_EXIT();
    }

    /** search if there is a source address profile in the fields and if found - add it */
    for (fields_idx = 0; flow_app_info->special_fields[fields_idx] != FLOW_S_F_EMPTY; fields_idx++)
    {
        SHR_IF_ERR_EXIT(flow_special_field_info_get
                        (unit, flow_app_info->special_fields[fields_idx], &special_field_info));

        if (dnx_flow_special_field_is_ind_set
            (unit, special_field_info, FLOW_SPECIAL_FIELD_IND_SRC_ADDR_PROFILE_SUPPORTED))
        {
            flow_special_fields_ids[(*nof_special_fields)] = flow_app_info->special_fields[fields_idx];
            (*nof_special_fields)++;
            /** there can be only one src address profile field */
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_ut_dbal_fields_from_common_fields_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    dbal_fields_e app_related_dbal_fields[FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS + BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    uint32 common_field_idx[FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS],
    uint32 *nof_common_fields)
{
    uint32 dbal_field_idx, app_nof_dbal_fields = 0, app_field_idx = 0;
    dbal_fields_e common_fields_related_dbal_fields[CTEST_FLOW_SIZE_OF_COMMON_RELATED_DBAL_FIELDS] = { 0 };
    uint32 app_common_fields_bitmap = flow_app_info->valid_common_fields_bitmap;

    SHR_FUNC_INIT_VARS(unit);

    /** We don't support P2P with learning for now. The default value shouldn't be validated in this case. */
    if ((!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, FLOW_STR_MAX)) &&
        (result_type == DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE))
    {
        app_common_fields_bitmap &= (~BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID);
    }

    while (app_common_fields_bitmap != 0)
    {
        /** if the field is relevant*/
        if (app_common_fields_bitmap & 0x1)
        {
            /** get relevant DBAL FIELD to set for the test */
            SHR_IF_ERR_EXIT(ctest_flow_common_field_to_dbal_field
                            (unit, flow_app_info->flow_app_type, (1 << app_field_idx),
                             common_fields_related_dbal_fields));
            for (dbal_field_idx = 0; common_fields_related_dbal_fields[dbal_field_idx] != DBAL_FIELD_EMPTY;
                 dbal_field_idx++)
            {
                app_related_dbal_fields[app_nof_dbal_fields] = common_fields_related_dbal_fields[dbal_field_idx];

                /** save in the location of the common field in the bitmap */
                common_field_idx[app_nof_dbal_fields] = app_field_idx;

                app_nof_dbal_fields++;
            }
        }

        /** update the loop indexes */
        app_field_idx++;
        app_common_fields_bitmap = (app_common_fields_bitmap >> 1);
    }
    (*nof_common_fields) = app_nof_dbal_fields;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_ut_dbal_fields_from_special_fields_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint32 nof_common_fields,
    dbal_fields_e app_related_dbal_fields[FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS + BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    uint32 *app_nof_dbal_fields)
{
    int app_field_idx;
    const flow_special_field_info_t *special_field_info;

    SHR_FUNC_INIT_VARS(unit);

    /** setting the starting point for the array */
    (*app_nof_dbal_fields) = nof_common_fields;

    /** looping to extract all dbal fields relevant for special fields */
    for (app_field_idx = 0; flow_app_info->special_fields[app_field_idx] != FLOW_S_F_EMPTY; app_field_idx++)
    {
        SHR_IF_ERR_EXIT(flow_special_field_info_get
                        (unit, flow_app_info->special_fields[app_field_idx], &special_field_info));
        app_related_dbal_fields[(*app_nof_dbal_fields)] = special_field_info->mapped_dbal_field;
        (*app_nof_dbal_fields)++;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_ut_dbal_fields_from_table_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    dbal_fields_e dbal_field_in_table[FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS + BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    uint32 *nof_dbal_fields_in_table)
{
    dbal_fields_e next_field_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_next_field_get
                    (unit, FLOW_LIF_DBAL_TABLE_GET(flow_app_info), DBAL_FIELD_EMPTY, 0, result_type, &next_field_id));

    while (next_field_id != DBAL_FIELD_EMPTY)
    {
        dbal_field_in_table[(*nof_dbal_fields_in_table)] = next_field_id;
        (*nof_dbal_fields_in_table)++;
        SHR_IF_ERR_EXIT(dbal_tables_next_field_get
                        (unit, FLOW_LIF_DBAL_TABLE_GET(flow_app_info), next_field_id, 0, result_type, &next_field_id));
    }

exit:
    SHR_FUNC_EXIT;
}

static void
ctest_flow_ut_app_fields_select(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    dbal_fields_e app_related_dbal_fields[FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS + BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    uint32 app_nof_dbal_fields,
    dbal_fields_e dbal_field_in_table[FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS + BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    uint32 nof_dbal_fields_in_table,
    uint32 common_field_idx[FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS],
    uint32 nof_common_fields,
    uint32 *matching_common_fields_bitmap,
    flow_special_fields_e flow_special_fields_ids[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    uint32 *nof_selected_special_fields)
{
    uint32 app_field_idx, dbal_table_idx;

    *matching_common_fields_bitmap = 0;
    for (app_field_idx = 0; app_field_idx < app_nof_dbal_fields; app_field_idx++)
    {
        for (dbal_table_idx = 0; dbal_table_idx < nof_dbal_fields_in_table; dbal_table_idx++)
        {
            if (dbal_field_in_table[dbal_table_idx] == app_related_dbal_fields[app_field_idx])
            {
                /** if dbal field matches common field, update its bitmap index in the matching fields */
                if (app_field_idx < nof_common_fields)
                {
                    (*matching_common_fields_bitmap) |= (1 << common_field_idx[app_field_idx]);
                }
                else
                {
                    /** getting the special field matching to the selected app_related_dbal_fields */
                    flow_special_fields_ids[(*nof_selected_special_fields)] =
                        flow_app_info->special_fields[app_field_idx - nof_common_fields];
                    (*nof_selected_special_fields)++;
                }
                break;
            }
        }
    }
}

static shr_error_e
ctest_flow_ut_mandatory_and_unchangeable_fields_select(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    uint32 *matching_common_fields_bitmap,
    flow_special_fields_e flow_special_fields_ids[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    uint32 *nof_selected_special_fields)
{
    uint32 app_field_idx;
    uint32 nof_mandatory_special_fields;
    int mandatory_field_idx, added_mandatory_fields = 0;
    flow_special_fields_e mandatory_special_fields[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS];

    SHR_FUNC_INIT_VARS(unit);

    /** unique special fields addition */
    ctest_flow_ut_app_mandatory_common_fields_get(unit, flow_app_info, matching_common_fields_bitmap);

    /** unique special fields addition */
    SHR_IF_ERR_EXIT(ctest_flow_ut_app_mandatory_and_unchangeable_special_fields_get
                    (unit, flow_app_info, result_type, mandatory_special_fields, &nof_mandatory_special_fields));

    /** if there are mandatory special fields, check if they are not already selected */
    if (nof_mandatory_special_fields)
    {
        for (mandatory_field_idx = 0; mandatory_field_idx < nof_mandatory_special_fields; mandatory_field_idx++)
        {
            for (app_field_idx = 0; app_field_idx < (*nof_selected_special_fields); app_field_idx++)
            {
                if (flow_special_fields_ids[app_field_idx] == mandatory_special_fields[mandatory_field_idx])
                {
                    break;
                }
            }
            /** occurs only if mandatory_special_field is not already selected */
            if (app_field_idx == (*nof_selected_special_fields))
            {
                flow_special_fields_ids[(*nof_selected_special_fields) + added_mandatory_fields] =
                    mandatory_special_fields[mandatory_field_idx];
                added_mandatory_fields++;
            }
        }
        (*nof_selected_special_fields) += added_mandatory_fields;
    }

exit:
    SHR_FUNC_EXIT;
}

/** fill an array of dbal fields that corresponds between the app common and special fields to the app related dbal table fields */
static shr_error_e
ctest_flow_ut_matching_fields_find(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    uint32 *matching_common_fields_bitmap,
    flow_special_fields_e flow_special_fields_ids[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS])
{
    dbal_fields_e app_related_dbal_fields[FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS + BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS] =
        { 0 };
    dbal_fields_e dbal_fields_in_table[FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS + BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS] =
        { 0 };
    uint32 nof_selected_special_fields = 0;
    uint32 app_nof_dbal_fields = 0;
    uint32 nof_dbal_fields_in_table = 0;
    uint32 nof_common_fields = 0;
    uint32 common_field_idx[FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /*
     * PART 1 - extract all relevant data: dbal fields related to common and special fields and the dbal fields from the table
     */

    /** extracting the dbal fields matching the common fields */
    SHR_IF_ERR_EXIT(ctest_flow_ut_dbal_fields_from_common_fields_get
                    (unit, flow_app_info, result_type, app_related_dbal_fields, common_field_idx, &nof_common_fields));

    /** extracting the dbal fields matching the special fields */
    SHR_IF_ERR_EXIT(ctest_flow_ut_dbal_fields_from_special_fields_get(unit, flow_app_info, nof_common_fields,
                                                                      app_related_dbal_fields, &app_nof_dbal_fields));

    /** extracting the dbal fields matching the dbal table */
    SHR_IF_ERR_EXIT(ctest_flow_ut_dbal_fields_from_table_get(unit, flow_app_info, result_type, dbal_fields_in_table,
                                                             &nof_dbal_fields_in_table));

    /*
     * PART 2 - cross-check the fields from the two arrays - and select those who exists in both
     */

    /** cross-check fields which exist in both arrays */
    ctest_flow_ut_app_fields_select(unit, flow_app_info, app_related_dbal_fields, app_nof_dbal_fields,
                                    dbal_fields_in_table, nof_dbal_fields_in_table, common_field_idx, nof_common_fields,
                                    matching_common_fields_bitmap, flow_special_fields_ids,
                                    &nof_selected_special_fields);

    /*
     * PART 3 - treat with the special cases
     */

    /** unique common fields checks */
    ctest_flow_ut_common_fields_remove_ingress_info_duplication(unit, flow_app_info, matching_common_fields_bitmap);

    /** treat unmapped special fields */
    SHR_IF_ERR_EXIT(ctest_flow_ut_app_unmapped_special_fields_add
                    (unit, flow_app_info, result_type, *matching_common_fields_bitmap, dbal_fields_in_table,
                     nof_dbal_fields_in_table, flow_special_fields_ids, &nof_selected_special_fields));

    /** add required sip fields if required */
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE))
    {
        SHR_IF_ERR_EXIT(ctest_flow_ut_src_addr_profile_special_field_add(unit, flow_app_info, dbal_fields_in_table,
                                                                         nof_dbal_fields_in_table,
                                                                         flow_special_fields_ids,
                                                                         &nof_selected_special_fields));
    }

    /** treat the mandatory and unchangeable fields */
    SHR_IF_ERR_EXIT(ctest_flow_ut_mandatory_and_unchangeable_fields_select(unit, flow_app_info, result_type,
                                                                           matching_common_fields_bitmap,
                                                                           flow_special_fields_ids,
                                                                           &nof_selected_special_fields));

    /** marking the end of the fields */
    flow_special_fields_ids[nof_selected_special_fields] = FLOW_S_F_EMPTY;

exit:
    SHR_FUNC_EXIT;
}

/** check if the special field can is valid for update update */
static shr_error_e
ctest_flow_ut_special_field_value_updating_valid(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    flow_special_fields_e field_id,
    uint8 *update_valid)
{
    const flow_special_field_info_t *special_field_info;

    SHR_FUNC_INIT_VARS(unit);

    /** assume the update is valid unless meeting with future conditions */
    (*update_valid) = TRUE;

    SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, field_id, &special_field_info));

    /** pass on the conditions that mark this field as not valid for value updating */
    if (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_ENABLER)
    {
        (*update_valid) = FALSE;
    }
    else if ((special_field_info->payload_type == FLOW_PAYLOAD_TYPE_BCM_ENUM) ||
             (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_ENUM))
    {
        (*update_valid) = FALSE;
    }
    else if (FLOW_APP_IS_LIF(flow_app_info->flow_app_type) &&
             dnx_flow_special_field_is_ind_set(unit, special_field_info, FLOW_SPECIAL_FIELD_IND_LIF_UNCHANGEABLE))
    {
        (*update_valid) = FALSE;
    }
    else if (dnx_flow_special_field_is_ind_set(unit, special_field_info, FLOW_SPECIAL_FIELD_IND_IS_KEY))
    {
        (*update_valid) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/** check the special field value */
static shr_error_e
ctest_flow_ut_special_field_value_update(
    int unit,
    flow_special_fields_e field_id,
    bcm_flow_special_field_t * special_field_values)
{
    const flow_special_field_info_t *special_field_info;
    uint8 idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, field_id, &special_field_info));

    switch (special_field_info->payload_type)
    {
        /** for arrays update only the 1st two as they are always joint */
        case FLOW_PAYLOAD_TYPE_UINT32_ARR:
            for (idx = 0; idx < 1; idx++)
            {
                special_field_values->shr_var_uint32_arr[idx]++;
            }
            break;
        case FLOW_PAYLOAD_TYPE_UINT8_ARR:
            for (idx = 0; idx < 1; idx++)
            {
                special_field_values->shr_var_uint8_arr[idx]++;
            }
            break;
        case FLOW_PAYLOAD_TYPE_UINT32:
            special_field_values->shr_var_uint32++;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "payload type (%d) should not reach here\n", special_field_info->payload_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_ut_special_field_value_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    uint8 update_value,
    uint8 use_for_set,
    flow_special_fields_e field_id,
    bcm_flow_special_field_t * special_field_values)
{
    uint8 update_valid = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_legal_value_get(unit, flow_app_info, result_type,
                                                                field_id, special_field_values));

    if ((update_value == TRUE) && (special_field_values->shr_var_uint32 != CTEST_FLOW_SKIP_FIELD))
    {
        SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_value_updating_valid(unit, flow_app_info, field_id, &update_valid));

        if (update_valid == TRUE)
        {
            SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_value_update(unit, field_id, special_field_values));
        }
        else if (use_for_set)
        {
            special_field_values->shr_var_uint32 = CTEST_FLOW_SKIP_FIELD;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** set a legal value, if possible, to a common field */
static shr_error_e
ctest_flow_ut_common_field_legal_value_set(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    int enabler,
    uint8 update_value,
    void *app_data,
    uint8 *is_skipped)
{
    int legal_value;
    uint8 update_valid = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    /** set status to failure */
    (*is_skipped) = 1;

    SHR_IF_ERR_EXIT(ctest_flow_ut_common_field_value_get(unit, flow_app_info, result_type, update_value,
                                                         enabler, &legal_value, &update_valid));

    if (legal_value == CTEST_FLOW_SKIP_FIELD)
    {
        LOGI(LM, (U(unit, "\tCommon field %s (0x%x) was skipped\n"),
                  dnx_flow_common_field_enabler_to_string(unit, flow_app_info->flow_app_type, enabler), enabler));
    }
    else if (update_valid == TRUE)
    {
        SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler(unit, flow_app_info->flow_app_type, enabler, app_data, legal_value));
        LOGI(LM, (U(unit, "\tCommon field %s (0x%x) was set to 0x%x\n"),
                  dnx_flow_common_field_enabler_to_string(unit, flow_app_info->flow_app_type, enabler), enabler,
                  legal_value));
        *is_skipped = 0;
    }
    else        /* (update_valid == FALSE) */
    {
        LOGI(LM, (U(unit, "\tCommon field %s (0x%x) was not updated\n"),
                  dnx_flow_common_field_enabler_to_string(unit, flow_app_info->flow_app_type, enabler), enabler));
    }

exit:
    SHR_FUNC_EXIT;
}

/** filling the common fields that appear in the relevant dbal table to a default value */
static shr_error_e
ctest_flow_ut_common_fields_struct_fill(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    uint8 update_value,
    void *app_data,
    uint32 *matching_common_fields_bitmap)
{
    uint32 app_field_idx = 0;
    uint32 temp_matching_common_fields_bitmap = (*matching_common_fields_bitmap);
    uint8 is_skipped;

    SHR_FUNC_INIT_VARS(unit);

    /** zeroing the bitmap to set to actual set fields */
    *matching_common_fields_bitmap = 0;

    /** setting the relevant common fields */
    while (temp_matching_common_fields_bitmap != 0)
    {
        if (temp_matching_common_fields_bitmap & 0x1)
        {
            SHR_IF_ERR_EXIT(ctest_flow_ut_common_field_legal_value_set(unit, flow_app_info, result_type,
                                                                       (1 << app_field_idx), update_value,
                                                                       app_data, &is_skipped));
            if (!is_skipped)
            {
                *matching_common_fields_bitmap |= (1 << app_field_idx);
            }
        }
        app_field_idx++;
        temp_matching_common_fields_bitmap = (temp_matching_common_fields_bitmap >> 1);
    }

    /** For FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, Cannot set service_type or dest_info alone */
    if ((flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) &&
        ((!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled)) ||
         (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, FLOW_STR_MAX))))
    {
        if (_SHR_IS_FLAG_SET(*matching_common_fields_bitmap, BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID))
        {
            *matching_common_fields_bitmap &= ~(BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID);
        }
        if (_SHR_IS_FLAG_SET(*matching_common_fields_bitmap, BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID))
        {
            *matching_common_fields_bitmap &= ~(BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID);
        }
    }

    LOGI(LM, (U(unit, "\tCommon bitmap after final selection is 0x%x\n"), *matching_common_fields_bitmap));

exit:
    SHR_FUNC_EXIT;
}

/** filling the bcm special fields array with a hardcoded values per special field that is found in
 *  special_fields_to_add  */
static shr_error_e
ctest_flow_ut_special_fields_struct_fill(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    int result_type,
    uint8 update_value,
    uint8 update_special_fields_arr,
    flow_special_fields_e special_fields_to_add[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    bcm_flow_special_fields_t * special_fields)
{
    uint32 app_field_idx, last_field_idx = 0;
    bcm_flow_special_field_t special_field_values;
    const flow_special_field_info_t *special_field_info;

    SHR_FUNC_INIT_VARS(unit);

    /** setting the relevant special fields */
    for (app_field_idx = 0; special_fields_to_add[app_field_idx] != FLOW_S_F_EMPTY; app_field_idx++)
    {
        SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_value_get(unit, flow_app_info, result_type, update_value,
                                                              TRUE, special_fields_to_add[app_field_idx],
                                                              &special_field_values));

        if (special_field_values.shr_var_uint32 != CTEST_FLOW_SKIP_FIELD)
        {
            special_fields->special_fields[last_field_idx].field_id = special_fields_to_add[app_field_idx];

            SHR_IF_ERR_EXIT(flow_special_field_info_get
                            (unit, special_fields_to_add[app_field_idx], &special_field_info));

            switch (special_field_info->payload_type)
            {
                case FLOW_PAYLOAD_TYPE_UINT32_ARR:
                    sal_memcpy(special_fields->special_fields[last_field_idx].shr_var_uint32_arr,
                               special_field_values.shr_var_uint32_arr,
                               sizeof(special_fields->special_fields[last_field_idx].shr_var_uint32_arr));

                    LOGI(LM, (U(unit, "\tSpecial field %s was set to 32bit array\n"),
                              dnx_flow_special_field_to_string(unit, special_fields_to_add[app_field_idx])));
                    break;
                case FLOW_PAYLOAD_TYPE_UINT8_ARR:
                    sal_memcpy(special_fields->special_fields[last_field_idx].shr_var_uint8_arr,
                               special_field_values.shr_var_uint8_arr,
                               sizeof(special_fields->special_fields[last_field_idx].shr_var_uint8_arr));

                    LOGI(LM, (U(unit, "\tSpecial field %s was set to 8bit array\n"),
                              dnx_flow_special_field_to_string(unit, special_fields_to_add[app_field_idx])));
                    break;
                case FLOW_PAYLOAD_TYPE_UINT32:
                    special_fields->special_fields[last_field_idx].shr_var_uint32 = special_field_values.shr_var_uint32;

                    LOGI(LM, (U(unit, "\tSpecial field %s was set to 0x%x\n"),
                              dnx_flow_special_field_to_string(unit, special_fields_to_add[app_field_idx]),
                              special_fields->special_fields[last_field_idx].shr_var_uint32));
                    break;

                case FLOW_PAYLOAD_TYPE_ENUM:
                case FLOW_PAYLOAD_TYPE_BCM_ENUM:
                    special_fields->special_fields[last_field_idx].symbol = special_field_values.symbol;

                    LOGI(LM, (U(unit, "\tSpecial field %s was set to %d\n"),
                              dnx_flow_special_field_to_string(unit, special_fields_to_add[app_field_idx]),
                              special_fields->special_fields[last_field_idx].symbol));
                    break;
                case FLOW_PAYLOAD_TYPE_ENABLER:
                    LOGI(LM, (U(unit, "\tSpecial field %s was set (enabler field)\n"),
                              dnx_flow_special_field_to_string(unit, special_fields_to_add[app_field_idx])));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Special field %s has invalid payload type (%d)\n",
                                 dnx_flow_special_field_to_string(unit, special_fields_to_add[app_field_idx]),
                                 special_field_info->payload_type);
                    break;
            }

            last_field_idx++;
        }
        else
        {
            LOGI(LM, (U(unit, "\tSpecial field %s was skipped\n"),
                      dnx_flow_special_field_to_string(unit, special_fields_to_add[app_field_idx])));

            /** if required - remove the field from the special_fields_to_add */
            if (update_special_fields_arr == TRUE)
            {
                uint8 special_fields_idx;
                for (special_fields_idx = app_field_idx; special_fields_to_add[special_fields_idx] != FLOW_S_F_EMPTY;
                     special_fields_idx++)
                {
                    special_fields_to_add[special_fields_idx] = special_fields_to_add[special_fields_idx + 1];
                }

                /** the next special field to check is now in the position that just now checked so need to update index */
                app_field_idx--;
            }
        }
    }
    special_fields->actual_nof_special_fields = last_field_idx;

exit:
    SHR_FUNC_EXIT;
}

/** filling the fields that appear in the relevant dbal table to a default value */
static shr_error_e
ctest_flow_ut_struct_fields_fill(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    uint8 update_value,
    uint8 update_special_fields_arr,
    void *app_data,
    bcm_flow_special_fields_t * special_fields,
    uint32 *matching_common_fields_bitmap,
    flow_special_fields_e matching_special_fields[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS])
{
    SHR_FUNC_INIT_VARS(unit);

    /** filling the special flows data */
    SHR_IF_ERR_EXIT(ctest_flow_ut_common_fields_struct_fill(unit, flow_app_info, result_type, update_value,
                                                            app_data, matching_common_fields_bitmap));

    /** filling the special flows data */
    SHR_IF_ERR_EXIT(ctest_flow_ut_special_fields_struct_fill(unit, flow_app_info, result_type, update_value,
                                                             update_special_fields_arr, matching_special_fields,
                                                             special_fields));

exit:
    SHR_FUNC_EXIT;
}

/** verifying the common fields, insuring the set value is as expected */
static shr_error_e
ctest_flow_ut_struct_common_fields_verify(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    uint8 update_valued,
    void *app_data,
    uint32 expected_common_fields_bitmap)
{
    uint32 app_field_idx = 0;
    uint32 actual_common_fields_bitmap;
    int expected_value, actual_value;
    uint8 dummy;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        actual_common_fields_bitmap = ((bcm_flow_terminator_info_t *) app_data)->valid_elements_set;
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        actual_common_fields_bitmap = ((bcm_flow_initiator_info_t *) app_data)->valid_elements_set;
    }
    if (actual_common_fields_bitmap != expected_common_fields_bitmap)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Actual common fields bitmap (0x%x) is not as expected (0x%x)\n",
                     actual_common_fields_bitmap, expected_common_fields_bitmap);
    }

    while (expected_common_fields_bitmap != 0)
    {
        if (expected_common_fields_bitmap & 0x1)
        {
            /** get the expected field's value */
            SHR_IF_ERR_EXIT(ctest_flow_ut_common_field_value_get(unit, flow_app_info, result_type, update_valued,
                                                                 (1 << app_field_idx), &expected_value, &dummy));

            /** get the actual field's value */
            SHR_IF_ERR_EXIT(ctest_flow_get_by_enabler(unit, flow_app_info->flow_app_type, (1 << app_field_idx),
                                                      app_data, &actual_value));

            if (expected_value != actual_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Actual data (0x%x) is not as expected (0x%x) for common field %s (0x%x)\n",
                             actual_value, expected_value,
                             dnx_flow_common_field_enabler_to_string(unit, flow_app_info->flow_app_type,
                                                                     (1 << app_field_idx)), (1 << app_field_idx));
            }
        }
        app_field_idx++;
        expected_common_fields_bitmap = (expected_common_fields_bitmap >> 1);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_flow_ut_special_fields_compare(
    int unit,
    const flow_special_field_info_t * special_field_info,
    bcm_flow_special_field_t * actual_value,
    bcm_flow_special_field_t * expected_value)
{
    int array_size;
    int iter;

    SHR_FUNC_INIT_VARS(unit);

    if (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_UINT32_ARR)
    {
        array_size = BCM_FLOW_SPECIAL_FIELD_UIN32_ARR_MAX_SIZE;

        /** verify the integrity of special field's values */
        for (iter = 0; iter < array_size; iter++)
        {
            if (actual_value->shr_var_uint32_arr[iter] != expected_value->shr_var_uint32_arr[iter])
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Special field id %s at index %d value 0x%x  is not as expected 0x%x",
                             special_field_info->name, iter, actual_value->shr_var_uint32_arr[iter],
                             expected_value->shr_var_uint32_arr[iter]);
            }
        }
    }
    else if (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_UINT8_ARR)
    {

        /** the getting the array size in uint32 units */
        array_size = BCM_FLOW_SPECIAL_FIELD_UIN8_ARR_MAX_SIZE;

        /** verify the integrity of special field's values */
        for (iter = 0; iter < array_size; iter++)
        {
            if (actual_value->shr_var_uint8_arr[iter] != expected_value->shr_var_uint8_arr[iter])
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Special field id %s at index %d value 0x%x is not as expected 0x%x",
                             special_field_info->name,
                             iter, actual_value->shr_var_uint8_arr[iter], expected_value->shr_var_uint8_arr[iter]);
            }
        }
    }
    else if (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_UINT32)
    {
        if (actual_value->shr_var_uint32 != expected_value->shr_var_uint32)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Special field id %s value 0x%x is not as expected 0x%x",
                         special_field_info->name, actual_value->shr_var_uint32, expected_value->shr_var_uint32);
        }
    }
    else if ((special_field_info->payload_type == FLOW_PAYLOAD_TYPE_ENUM)
             || (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_BCM_ENUM))
    {
        if (actual_value->symbol != expected_value->symbol)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Special field id %s value %d is not as expected %d",
                         special_field_info->name, actual_value->symbol, expected_value->symbol);
        }
    }
    else        /* FLOW_PAYLOAD_TYPE_ENABLER */
    {
        /** doesn't need to do anything - this field is a valueless flag */
    }

exit:
    SHR_FUNC_EXIT;
}

/** verifying the special fields, insuring the set value is as expected */
static shr_error_e
ctest_flow_ut_struct_special_fields_verify(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    uint8 update_valued,
    bcm_flow_special_fields_t * special_fields,
    flow_special_fields_e special_fields_added[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS])
{
    bcm_flow_special_field_t expected_values = { 0 };
    bcm_flow_special_field_t actual_values = { 0 };
    uint32 app_field_idx;
    uint8 rv;
    const flow_special_field_info_t *special_field_info;

    SHR_FUNC_INIT_VARS(unit);

    for (app_field_idx = 0; special_fields_added[app_field_idx] != FLOW_S_F_EMPTY; app_field_idx++)
    {
        /** get the expected values */
        SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_value_get(unit, flow_app_info, result_type, update_valued,
                                                              FALSE, special_fields_added[app_field_idx],
                                                              &expected_values));

        /** get the actual values */
        rv = ctest_flow_special_field_value_get(unit, special_fields, special_fields_added[app_field_idx],
                                                &actual_values);

        if (rv != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Special field %s was not written to the table\n",
                         dnx_flow_special_field_to_string(unit, special_fields_added[app_field_idx]));
        }

        SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, special_fields_added[app_field_idx], &special_field_info));

        SHR_IF_ERR_EXIT(ctest_dnx_flow_ut_special_fields_compare
                        (unit, special_field_info, &actual_values, &expected_values));
    }

    if (app_field_idx != special_fields->actual_nof_special_fields)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Expected %d special fields, but received %d", app_field_idx,
                     special_fields->actual_nof_special_fields);
    }

exit:
    SHR_FUNC_EXIT;
}

/** verifying the fields, insuring the set value is as expected */
static shr_error_e
ctest_flow_ut_struct_fields_verify(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    uint8 update_valued,
    void *app_data,
    uint32 matching_common_fields_bitmap,
    bcm_flow_special_fields_t * special_fields,
    flow_special_fields_e result_type_matching_special_fields[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS])
{
    SHR_FUNC_INIT_VARS(unit);

    /** verifying the relevant common fields */
    SHR_IF_ERR_EXIT(ctest_flow_ut_struct_common_fields_verify(unit, flow_app_info, result_type, update_valued,
                                                              app_data, matching_common_fields_bitmap));

    /** verifying the relevant special fields */
    SHR_IF_ERR_EXIT(ctest_flow_ut_struct_special_fields_verify(unit, flow_app_info, result_type, update_valued,
                                                               special_fields, result_type_matching_special_fields));

exit:
    SHR_FUNC_EXIT;
}

/** getting the list of existing common fields (valid_elements_set) and returning a list of common fields to remove (valid_elements_clear) */
static shr_error_e
ctest_flow_ut_common_fields_remove_half(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint32 valid_elements_set,
    uint32 *valid_elements_clear)
{
    uint32 orig_valid_elements = valid_elements_set;
    uint32 remained_valid_elements;
    uint32 mandatory_fields = 0;
    uint32 field_idx = 0;
    uint8 is_removed = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** get the mandatory fields, which cannot be removed */
    ctest_flow_ut_app_mandatory_common_fields_get(unit, flow_app_info, &mandatory_fields);

    if (!valid_elements_set)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        SHR_EXIT();
    }

    /** update removed common fields */
    while (valid_elements_set != 0)
    {
        /** if field is set, and is not part of the mandatory fields */
        if ((valid_elements_set & 0x1) && (!(mandatory_fields & 0x1)))
        {
            if (is_removed)
            {
                (*valid_elements_clear) |= (1 << field_idx);
            }

            /** change is_removed status */
            is_removed ^= 0x1;
        }

        field_idx++;
        valid_elements_set = (valid_elements_set >> 1);
        mandatory_fields = (mandatory_fields >> 1);
    }

    /** Don't change from P2P to MP by not removing the Service-type and Destination fields */
    if ((flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) &&
        ((!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled)) ||
         (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, FLOW_STR_MAX))))
    {
        if (_SHR_IS_FLAG_SET(*valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID))
        {
            *valid_elements_clear &= ~(BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID);
        }
        if (_SHR_IS_FLAG_SET(*valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID))
        {
            *valid_elements_clear &= ~(BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID);
        }
    }

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        /** if remove learn_info or learn_enable, remove both */
        if ((_SHR_IS_FLAG_SET(*valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID)) ||
            (_SHR_IS_FLAG_SET(*valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID)))
        {
            *valid_elements_clear &= ~(BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID |
                                       BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID);
        }

        /** if remove failover_id or failover_state, remove both */
        if ((_SHR_IS_FLAG_SET(*valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_ID_VALID)) ||
            (_SHR_IS_FLAG_SET(*valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_STATE_VALID)))
        {
            *valid_elements_clear &= ~(BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_ID_VALID |
                                       BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_STATE_VALID);
        }
    }

    remained_valid_elements = orig_valid_elements & (~(*valid_elements_clear));

    /** update the clear_bitmap*/
    LOGI(LM, (U(unit, "\tCommon fields bitmap to be removed is 0x%x\n"), *valid_elements_clear));
    LOGI(LM, (U(unit, "\tCommon fields bitmap for the remaining fields is 0x%x\n"), remained_valid_elements));

exit:
    SHR_FUNC_EXIT;
}

/** getting the list of existing special fields (special_fields) and returning a list of special fields to remove (post_remove_special_field_ids) */
static shr_error_e
ctest_flow_ut_special_fields_remove_half(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    uint8 is_qos_exists,
    bcm_flow_special_fields_t * special_fields,
    flow_special_fields_e post_remove_special_field_ids[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    flow_special_fields_e removed_special_field_ids[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS])
{
    uint32 field_idx = 0;
    uint8 is_removed = 0;
    uint8 removed_special_fields_idx = 0;
    uint8 remained_special_fields_idx = 0;
    uint8 mandatory_field_idx;
    uint8 is_mandatory = FALSE;
    flow_special_fields_e mandatory_special_fields[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS];
    uint32 nof_mandatory_special_fields;
    uint8 first_removed = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_flow_ut_app_mandatory_and_unchangeable_special_fields_get
                    (unit, flow_app_info, result_type, mandatory_special_fields, &nof_mandatory_special_fields));

    for (field_idx = 0; field_idx < special_fields->actual_nof_special_fields; field_idx++)
    {
        /** check if field is mandatory, and if so, skip it */
        for (mandatory_field_idx = 0; mandatory_field_idx < nof_mandatory_special_fields; mandatory_field_idx++)
        {
            if (special_fields->special_fields[field_idx].field_id == mandatory_special_fields[mandatory_field_idx])
            {
                is_mandatory = TRUE;
            }
        }
        /** if BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID was removed, remove all qos fields */
        if ((!is_qos_exists) &&
            ((special_fields->special_fields[field_idx].field_id == FLOW_S_F_QOS_TTL) ||
             (special_fields->special_fields[field_idx].field_id == FLOW_S_F_QOS_DSCP) ||
             (special_fields->special_fields[field_idx].field_id == FLOW_S_F_QOS_EXP) ||
             (special_fields->special_fields[field_idx].field_id == FLOW_S_F_QOS_PRI) ||
             (special_fields->special_fields[field_idx].field_id == FLOW_S_F_QOS_CFI)))
        {
            is_removed = TRUE;
            is_mandatory = FALSE;
        }

        if (is_removed && !is_mandatory)
        {
            /** update the removed special field data */
            special_fields->special_fields[removed_special_fields_idx].field_id =
                special_fields->special_fields[field_idx].field_id;
            special_fields->special_fields[removed_special_fields_idx].is_clear = 1;
            /** zero the data as required in the verify */
            special_fields->special_fields[removed_special_fields_idx].shr_var_uint32 = 0;

            if (first_removed == TRUE)
            {
                LOGI(LM, (U(unit, "\tRemoved special fields: %s"),
                          dnx_flow_special_field_to_string(unit,
                                                           special_fields->
                                                           special_fields[removed_special_fields_idx].field_id)));
                first_removed = FALSE;
            }
            else
            {
                LOGI(LM, (U(unit, ", %s"),
                          dnx_flow_special_field_to_string(unit,
                                                           special_fields->
                                                           special_fields[removed_special_fields_idx].field_id)));
            }

            /** save the removed field in array for future usage */
            removed_special_field_ids[removed_special_fields_idx] =
                special_fields->special_fields[removed_special_fields_idx].field_id;

            /** update the removed field index */
            removed_special_fields_idx++;
        }
        else
        {
            post_remove_special_field_ids[remained_special_fields_idx] =
                special_fields->special_fields[field_idx].field_id;
            remained_special_fields_idx++;
            is_mandatory = FALSE;
        }

        /** change is_removed status, unless field was QOS_PRI, which after it come QOS_CFI which need to be handle the same */
        if (special_fields->special_fields[field_idx].field_id != FLOW_S_F_QOS_PRI)
        {
            is_removed ^= 0x1;
        }
    }

    /** If there was print, close the print's line */
    if (first_removed == FALSE)
    {
        LOGI(LM, (U(unit, "\n")));
    }

    /** marking the end of the post_remove_special_field_ids and removed_special_field_ids */
    post_remove_special_field_ids[remained_special_fields_idx] = FLOW_S_F_EMPTY;
    removed_special_field_ids[removed_special_fields_idx] = FLOW_S_F_EMPTY;

    /** updating the special fields new size */
    special_fields->actual_nof_special_fields = removed_special_fields_idx;
    special_fields->special_fields[special_fields->actual_nof_special_fields].field_id = FLOW_S_F_EMPTY;

exit:
    SHR_FUNC_EXIT;
}

/** removing half of the fields (common and special) - getting the fields existing and returning list of fields to be removed */
static shr_error_e
ctest_flow_ut_fields_remove_half(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    uint32 valid_elements_set,
    uint32 *valid_elements_clear,
    bcm_flow_special_fields_t * special_fields,
    flow_special_fields_e post_remove_special_field_ids[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS],
    flow_special_fields_e removed_special_field_ids[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS])
{
    uint8 is_qos_exists;

    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID))
    {
        is_qos_exists = TRUE;
    }

    /** remove half of the common fields */
    SHR_IF_ERR_EXIT(ctest_flow_ut_common_fields_remove_half
                    (unit, flow_app_info, valid_elements_set, valid_elements_clear));

    if ((flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT) &&
        _SHR_IS_FLAG_SET(*valid_elements_clear, BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID))
    {
        is_qos_exists = FALSE;
    }

    /** remove half of the special fields */
    SHR_IF_ERR_EXIT(ctest_flow_ut_special_fields_remove_half(unit, flow_app_info, result_type, is_qos_exists,
                                                             special_fields, post_remove_special_field_ids,
                                                             removed_special_field_ids));

exit:
    SHR_FUNC_EXIT;
}

/** this function run a ut test */
static shr_error_e
ctest_flow_suffix_check(
    int unit,
    const dnx_flow_app_config_t * flow_app_info)
{
    uint8 suffix_start_loc;
    char suffix_name[20];

    SHR_FUNC_INIT_VARS(unit);

    switch (flow_app_info->flow_app_type)
    {
        case FLOW_APP_TYPE_TERM_MATCH:
            strcpy(suffix_name, "TERM_MATCH");
            break;
        case FLOW_APP_TYPE_INIT_MATCH:
            strcpy(suffix_name, "INIT_MATCH");
            break;
        case FLOW_APP_TYPE_TERM:
            strcpy(suffix_name, "TERMINATOR");
            break;
        case FLOW_APP_TYPE_INIT:
            strcpy(suffix_name, "INITIATOR");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "flow app %s has invalid type %s", flow_app_info->app_name,
                         flow_app_type_to_string(unit, flow_app_info->flow_app_type));
            break;
    }

    /** calculate the location of beginning of the suffix */
    suffix_start_loc = sal_strnlen(flow_app_info->app_name, FLOW_STR_MAX) - sal_strnlen(suffix_name, FLOW_STR_MAX);

    /** check the the suffix */
    if ((sal_strncmp(flow_app_info->app_name + suffix_start_loc, suffix_name, sizeof(flow_app_info->app_name))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "App name %s is type %s and must end with %s", flow_app_info->app_name,
                     flow_app_type_to_string(unit, flow_app_info->flow_app_type), suffix_name);
    }

    LOGI(LM, (U(unit, "Suffix verified\n")));

exit:
    SHR_FUNC_EXIT;
}

/*
 * A validation that the Virtual indications and the allowed handle flags (virtual flag) are in sync 
 * for a non-match defined applications
 */
static shr_error_e
ctest_flow_virtual_indication_check(
    int unit,
    const dnx_flow_app_config_t * flow_app_info)
{
    int is_virtual_app, is_virtual_handle_allowed;

    SHR_FUNC_INIT_VARS(unit);

    /** Determine whether the application has one of the Virtual-LIF indications */
    is_virtual_app = (((flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT) &&
                       (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_VIRTUAL_LIF_ONLY) ||
                        dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_VIRTUAL_LIF_SUPPORTED))) ||
                      ((flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) &&
                       (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_IND_VIRTUAL_LIF_SUPPORTED)))) ?
        TRUE : FALSE;

    /** Determine whether the application is allowed to use Virtual-LIF flow handle flag */
    is_virtual_handle_allowed = (_SHR_IS_FLAG_SET(flow_app_info->valid_flow_flags, BCM_FLOW_HANDLE_INFO_VIRTUAL)) ?
        TRUE : FALSE;

    /** Compare the two defines to verify that they are in sync  */
    if (is_virtual_app != is_virtual_handle_allowed)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Incompatible Virtual definition for app %s. Is Virtual APP - %d, Is Virtual handle allowed - %d",
                     flow_app_info->app_name, is_virtual_app, is_virtual_handle_allowed);
    }

    LOGI(LM, (U(unit, "Virtual indication verified for application %s\n"), flow_app_info->app_name));

exit:
    SHR_FUNC_EXIT;
}

/* 
 * Verify that all match payload applications are on the same side (Terminator / Initiator) as the match application 
 * Applicable only for match applications 
 */
static shr_error_e
ctest_flow_match_payload_check(
    int unit,
    const dnx_flow_app_config_t * flow_app_info)
{
    int payload_app_idx;
    bcm_flow_handle_t flow_handle;
    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH)
    {
        for (payload_app_idx = 0; payload_app_idx < FLOW_MAX_NOF_MATCH_PAYLOAD_APPS; payload_app_idx++)
        {
            if (flow_app_info->match_payload_apps[payload_app_idx][0])
            {
                flow_handle =
                    dnx_flow_handle_by_app_name_get(unit, flow_app_info->match_payload_apps[payload_app_idx],
                                                    FLOW_APP_TYPE_TERM);
                if (flow_handle == FLOW_APP_INVALID_IDX)
                {
                    SHR_ERR_EXIT(_SHR_E_INIT,
                                 "Error: Application %s was defined a %s match payload that isn't a Terminator app\n",
                                 flow_app_info->app_name, flow_app_info->match_payload_apps[payload_app_idx]);
                }
            }
        }
    }
    else if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT_MATCH)
    {
        for (payload_app_idx = 0; payload_app_idx < FLOW_MAX_NOF_MATCH_PAYLOAD_APPS; payload_app_idx++)
        {
            if (flow_app_info->match_payload_apps[payload_app_idx][0])
            {
                flow_handle =
                    dnx_flow_handle_by_app_name_get(unit, flow_app_info->match_payload_apps[payload_app_idx],
                                                    FLOW_APP_TYPE_INIT);
                if (flow_handle == FLOW_APP_INVALID_IDX)
                {
                    SHR_ERR_EXIT(_SHR_E_INIT,
                                 "Error: Application %s was defined a %s match payload that isn't an Initiator app\n",
                                 flow_app_info->app_name, flow_app_info->match_payload_apps[payload_app_idx]);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ctest_flow_match_related_lif_entry_add(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_t * related_lif_app_handle_id,
    bcm_gport_t * flow_id)
{
    int rv;
    bsl_severity_t orig_flow_severity;
    dnx_flow_app_config_t *lif_flow_app_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_flow_ut_match_payload_app_find
                    (unit, flow_app_info, related_lif_app_handle_id, &lif_flow_app_info));

    /** closing the prints severity related to flow */
    orig_flow_severity = bslenable_get(bslLayerBcmdnx, bslSourceFlow);
    bslenable_set(bslLayerBcmdnx, bslSourceFlow, bslSeverityOff);

    rv = ctest_flow_ut_run(unit, lif_flow_app_info, 1, flow_id, 0);

    /** restoring the prints severity */
    bslenable_set(bslLayerBcmdnx, bslSourceFlow, orig_flow_severity);

    if (rv)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Test COULD NOT RUN since LIF App (%s) entry has failed\n",
                     lif_flow_app_info->app_name);
    }

    LOGI(LM, (U(unit, "\tDone, LIF 0x%x was added using app %s\n"), (uint32) (*flow_id), lif_flow_app_info->app_name));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_match_validity_check(
    int unit,
    const dnx_flow_app_config_t * flow_app_info)
{
    int is_table_active;
    int table_pos;
    int nof_tables = 0;
    dbal_table_type_e first_table_type = DBAL_TABLE_TYPE_NONE;

    SHR_FUNC_INIT_VARS(unit);

    for (table_pos = 0; table_pos < FLOW_MAX_NOF_MATCH_DBAL_TABLES; table_pos++)
    {
        dbal_table_type_e table_type = DBAL_TABLE_TYPE_NONE;

        if ((flow_app_info->flow_table[table_pos] != DBAL_TABLE_EMPTY) || (table_pos == 0))
        {
            SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, flow_app_info->flow_table[table_pos], &is_table_active));
            if (!is_table_active)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "For Application %s DBAL table in pos %d is not active \n",
                             flow_app_info->app_name, table_pos);
            }
            nof_tables++;
            SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, flow_app_info->flow_table[table_pos], &table_type));
            if (table_pos == 0)
            {
                first_table_type = table_type;
            }
            else
            {
                if (first_table_type != table_type)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "For App %s all DBAL tables must has same type, issue with table in pos %d \n",
                                 flow_app_info->app_name, table_pos);
                }
            }
        }
    }

    if (nof_tables > 1 && !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES)))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "App with Multiple DBAL tables must use FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES Indication\n");
    }

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES))
    {
        if (flow_app_info->dbal_table_to_key_field_map == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Expect that dbal_table_to_key_field_map cb will be defined for applicagtions with FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES Indication\n");
        }
    }
    else
    {
        if (flow_app_info->dbal_table_to_key_field_map != NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "dbal_table_to_key_field_map cb should be defined only if FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES Indication is set\n");
        }
    }

    if (_SHR_IS_FLAG_SET(flow_app_info->valid_flow_flags, BCM_FLOW_HANDLE_INFO_BUD))
    {
        if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD))
        {
            if (flow_app_info->second_pass_table != DBAL_TABLE_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "When BUD_BY_KEY_FIELD set second_pass_table cannot be defined \n");
            }
        }
        else
        {
            if (flow_app_info->second_pass_table == DBAL_TABLE_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "application with bud flag must have second_pass_table in the definitions \n");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static void
ctest_flow_ut_encap_access_set(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    bcm_flow_initiator_info_t * initiator_info)
{
    /** not supported for virtual applications with no local LIF */
    if (!dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF))
    {
        if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_ARP_INITIATOR, FLOW_STR_MAX))
        {
            initiator_info->encap_access = bcmEncapAccessAc;
        }
        else
        {
            initiator_info->encap_access =
                (flow_app_info->encap_access_default_mapping !=
                 bcmEncapAccessInvalid) ? flow_app_info->encap_access_default_mapping : bcmEncapAccessTunnel2;
        }

        /*
         * Encap access must be set
         */
        LOGI(LM, (U(unit, "\tEncap access = %s\n"), flow_encap_access_to_string(unit, initiator_info->encap_access)));
    }
}

/** Check whether a specific result-type of a dbal table is supported for testing */
static shr_error_e
ctest_flow_ut_result_type_is_supported(
    int unit,
    dbal_tables_e table_id,
    uint8 result_type,
    int *is_supported)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_is_result_type_valid(unit, table_id, result_type, is_supported));

    /** Some result-types aren't supported and can't be selected  */
    if (*is_supported)
    {
        if ((table_id == DBAL_TABLE_IN_AC_INFO_DB) &&
            ((result_type == DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_VSI_WO_PROTECTION) ||
             (result_type == DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_VSI_W_1_VLAN) ||
             (result_type == DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_RCH_CONTROL_LIF)))
        {
            *is_supported = FALSE;
        }
        /** exclude PHP result types as FLOW_S_F_MPLS_PHP is exluded from ctests */
        else if ((table_id == DBAL_TABLE_EEDB_MPLS_TUNNEL) &&
                 ((result_type == DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_PHP_STAT)))
        {
            *is_supported = FALSE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_ut_single_result_type_run(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    bcm_gport_t * flow_id,
    uint8 result_type,
    uint32 keep_entry,
    uint8 *entry_written)
{
    flow_special_fields_e flow_special_fields_ids[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS];
    flow_special_fields_e removed_special_field_ids[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS];
    flow_special_fields_e post_remove_special_field_ids[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS];
    uint32 matching_common_fields_bitmap = 0;
    uint32 common_fields_selected_bitmap = 0;
    uint32 common_fields_removed_bitmap = 0;
    uint32 *common_fields_removed_ptr;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };
    void *common_fields_data;
    bcm_flow_terminator_info_t terminator_info = { 0 };
    bcm_flow_initiator_info_t initiator_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** flow configurations */
    common_fields_data = (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) ? (void *) &terminator_info :
        (void *) &initiator_info;

    LOGI(LM, (U(unit,
                "\nPhase 1 - Preparations for running on result type %s (%d)\n"),
              dbal_result_type_to_string(unit, FLOW_LIF_DBAL_TABLE_GET(flow_app_info), result_type), result_type));

    SHR_IF_ERR_EXIT(ctest_flow_ut_matching_fields_find(unit, flow_app_info, result_type,
                                                       &matching_common_fields_bitmap, flow_special_fields_ids));

    LOGI(LM, (U(unit, "\tCommon bitmap after fields selection 0x%x for result_type %s\n"),
              matching_common_fields_bitmap, dbal_result_type_to_string(unit, FLOW_LIF_DBAL_TABLE_GET(flow_app_info),
                                                                        result_type)));

    /** filling the common and special fields values for the create phase */
    SHR_IF_ERR_EXIT(ctest_flow_ut_struct_fields_fill(unit, flow_app_info, result_type, FALSE, TRUE,
                                                     common_fields_data, &special_fields,
                                                     &matching_common_fields_bitmap, flow_special_fields_ids));

    /** keeping the bitmap of all selected common fields for later usage */
    common_fields_selected_bitmap = matching_common_fields_bitmap;

    /** setting the flow_handle_info */
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, flow_app_info->app_name, &flow_handle_info.flow_handle));

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        ((bcm_flow_terminator_info_t *) common_fields_data)->valid_elements_set = matching_common_fields_bitmap;
    }
    else        /* FLOW_APP_TYPE_INIT */
    {

        ctest_flow_ut_encap_access_set(unit,
                                       flow_app_info, result_type, (bcm_flow_initiator_info_t *) common_fields_data);

        ((bcm_flow_initiator_info_t *) common_fields_data)->valid_elements_set = matching_common_fields_bitmap;

        if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_VIRTUAL_LIF_ONLY))
        {
            flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_VIRTUAL;
        }
    }

    LOGI(LM, (U(unit, "Phase 2 - Creating the LIF\n")));

    SHR_IF_ERR_EXIT(ctest_flow_ut_application_lif_entry_create(unit, flow_app_info, &flow_handle_info,
                                                               common_fields_data, &special_fields));

    LOGI(LM, (U(unit, "\tAssigned LIF-ID is 0x%x\n"), (uint32) flow_handle_info.flow_id));

    if (flow_id != NULL)
    {
        (*flow_id) = flow_handle_info.flow_id;
    }

    if (keep_entry)
    {
        (*entry_written) = TRUE;
        /** in case of keeping the entry test will stop here */
        SHR_EXIT();
    }
    else
    {
        uint32 flow_handle;

        /** adding validation for the API flow_lif_flow_app_from_global_lif_get returns the correct flow app */
        SHR_IF_ERR_EXIT(flow_lif_flow_app_from_gport_get
                        (unit, flow_handle_info.flow_id, flow_app_info->flow_app_type, &flow_handle));

        if (flow_handle != flow_handle_info.flow_handle)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "flow handle mismatch, expected %d, received %d\n",
                         flow_handle_info.flow_handle, flow_handle);
        }
    }

    /** get operation */
    LOGI(LM, (U(unit, "Phase 3 - Getting & traversing the LIF ID 0x%x properties and verifying them\n"),
              (uint32) flow_handle_info.flow_id));

    SHR_IF_ERR_EXIT(ctest_flow_ut_lif_application_entry_get(unit, flow_app_info, &flow_handle_info,
                                                            common_fields_data, &special_fields));

    /** verify the field's integrity */
    SHR_IF_ERR_EXIT(ctest_flow_ut_struct_fields_verify(unit, flow_app_info, result_type, FALSE,
                                                       common_fields_data, matching_common_fields_bitmap,
                                                       &special_fields, flow_special_fields_ids));

    LOGI(LM, (U(unit, "\tEntry get verified\n")));

    if (result_type == 0)
    {
        /*
         * perform traverse on the flow application only for result type 0, in order to ease the verify process inside
         * traverse. see xxx_entry_verify_cb 
         */
        SHR_IF_ERR_EXIT(ctest_flow_ut_application_traverse(unit, flow_app_info,
                                                           &flow_handle_info, flow_special_fields_ids));
    }

    LOGI(LM, (U(unit, "\tEntry traverse verified\n")));

    LOGI(LM, (U(unit, "Phase 4 - Replacing entry - removing part of the fields for LIF ID 0x%x \n"),
              (uint32) flow_handle_info.flow_id));

    /** update the relevant data for the replace command */
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE;
    flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        sal_memset(common_fields_data, 0, sizeof(bcm_flow_terminator_info_t));
        common_fields_removed_ptr = &(((bcm_flow_terminator_info_t *) common_fields_data)->valid_elements_clear);
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        sal_memset(common_fields_data, 0, sizeof(bcm_flow_initiator_info_t));
        common_fields_removed_ptr = &(((bcm_flow_initiator_info_t *) common_fields_data)->valid_elements_clear);
    }

    /** 1st phase of the replace operation - removing about half of the fields */
    SHR_IF_ERR_EXIT(ctest_flow_ut_fields_remove_half(unit, flow_app_info, result_type, matching_common_fields_bitmap,
                                                     common_fields_removed_ptr, &special_fields,
                                                     post_remove_special_field_ids, removed_special_field_ids));

    /** updating the new expected common_field_bitmap */
    matching_common_fields_bitmap &= ~(*common_fields_removed_ptr);
    /** keeping the removed common fields bitmap for later usage*/
    common_fields_removed_bitmap = (*common_fields_removed_ptr);

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        ((bcm_flow_initiator_info_t *) common_fields_data)->encap_access = bcmEncapAccessInvalid;
    }
    SHR_IF_ERR_EXIT(ctest_flow_ut_application_lif_entry_create(unit, flow_app_info, &flow_handle_info,
                                                               common_fields_data, &special_fields));

    LOGI(LM, (U(unit, "Phase 5 - Get and verify after replace on LIF with ID 0x%x\n"),
              (uint32) flow_handle_info.flow_id));

    /** removing the replace flag for the get */
    flow_handle_info.flags &= (~(BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID));

    /** perform get on the flow application */
    SHR_IF_ERR_EXIT(ctest_flow_ut_lif_application_entry_get(unit, flow_app_info, &flow_handle_info,
                                                            common_fields_data, &special_fields));

    SHR_IF_ERR_EXIT(ctest_flow_ut_struct_fields_verify(unit, flow_app_info, result_type, FALSE, common_fields_data,
                                                       matching_common_fields_bitmap, &special_fields,
                                                       post_remove_special_field_ids));

    LOGI(LM, (U(unit, "\tEntry get verified\n")));
    LOGI(LM, (U(unit, "Phase 6 - Replacing entry - adding back the removed fields for LIF with ID 0x%x\n"),
              (uint32) flow_handle_info.flow_id));

    /** first init the databases */
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        sal_memset(common_fields_data, 0, sizeof(bcm_flow_terminator_info_t));
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        sal_memset(common_fields_data, 0, sizeof(bcm_flow_initiator_info_t));
    }
    sal_memset(&special_fields, 0, sizeof(bcm_flow_special_fields_t));

    /** fill the databases */
    SHR_IF_ERR_EXIT(ctest_flow_ut_struct_fields_fill(unit, flow_app_info, result_type, FALSE, FALSE,
                                                     common_fields_data, &special_fields,
                                                     &common_fields_removed_bitmap, removed_special_field_ids));

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        ((bcm_flow_terminator_info_t *) common_fields_data)->valid_elements_set = common_fields_removed_bitmap;
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        ((bcm_flow_initiator_info_t *) common_fields_data)->encap_access = bcmEncapAccessInvalid;
        ((bcm_flow_initiator_info_t *) common_fields_data)->valid_elements_set = common_fields_removed_bitmap;
    }

    flow_handle_info.flags |= (BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID);
    SHR_IF_ERR_EXIT(ctest_flow_ut_application_lif_entry_create(unit, flow_app_info, &flow_handle_info,
                                                               common_fields_data, &special_fields));

    LOGI(LM, (U(unit, "Phase 7 - Get and verify after 2nd replace on LIF with ID 0x%x\n"),
              (uint32) flow_handle_info.flow_id));

    flow_handle_info.flags &= (~(BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID));

    /** perform get on the flow application */
    SHR_IF_ERR_EXIT(ctest_flow_ut_lif_application_entry_get(unit, flow_app_info, &flow_handle_info,
                                                            common_fields_data, &special_fields));

    SHR_IF_ERR_EXIT(ctest_flow_ut_struct_fields_verify(unit, flow_app_info, result_type, FALSE,
                                                       common_fields_data, common_fields_selected_bitmap,
                                                       &special_fields, flow_special_fields_ids));

    LOGI(LM, (U(unit, "\tEntry get verified\n")));

    LOGI(LM, (U(unit, "Phase 8 - Changing values for several of the fields using replace on LIF with ID 0x%x\n"),
              (uint32) flow_handle_info.flow_id));

    /** first init the databases */
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        sal_memset(common_fields_data, 0, sizeof(bcm_flow_terminator_info_t));
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        sal_memset(common_fields_data, 0, sizeof(bcm_flow_initiator_info_t));
    }
    matching_common_fields_bitmap = common_fields_selected_bitmap;
    sal_memset(&special_fields, 0, sizeof(bcm_flow_special_fields_t));

    /** filling the common and special fields values for the create phase */
    SHR_IF_ERR_EXIT(ctest_flow_ut_struct_fields_fill(unit, flow_app_info, result_type, TRUE, FALSE,
                                                     common_fields_data, &special_fields,
                                                     &matching_common_fields_bitmap, flow_special_fields_ids));

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        ((bcm_flow_terminator_info_t *) common_fields_data)->valid_elements_set = matching_common_fields_bitmap;
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        ((bcm_flow_initiator_info_t *) common_fields_data)->encap_access = bcmEncapAccessInvalid;
        ((bcm_flow_initiator_info_t *) common_fields_data)->valid_elements_set = matching_common_fields_bitmap;
    }

    flow_handle_info.flags |= (BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID);
    SHR_IF_ERR_EXIT(ctest_flow_ut_application_lif_entry_create(unit, flow_app_info, &flow_handle_info,
                                                               common_fields_data, &special_fields));

    LOGI(LM, (U(unit, "Phase 9 - Get and verify after 3rd replace on LIF with ID 0x%x\n"),
              (uint32) flow_handle_info.flow_id));

    flow_handle_info.flags &= (~(BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID));

    /** perform get on the flow application */
    SHR_IF_ERR_EXIT(ctest_flow_ut_lif_application_entry_get(unit, flow_app_info, &flow_handle_info,
                                                            common_fields_data, &special_fields));

    SHR_IF_ERR_EXIT(ctest_flow_ut_struct_fields_verify(unit, flow_app_info, result_type, TRUE,
                                                       common_fields_data, common_fields_selected_bitmap,
                                                       &special_fields, flow_special_fields_ids));

    LOGI(LM, (U(unit, "\tEntry get verified\n")));

    LOGI(LM, (U(unit, "Phase 10 - Deleting the LIF with ID 0x%x\n"), (uint32) flow_handle_info.flow_id));

    /** delete the entry */
    SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));

    LOGI(LM, (U(unit, "test over result type %d completed successfully\n"), result_type));

exit:
    SHR_FUNC_EXIT;
}

/** runs a flow ut test, when keep entry == 1, test ends just after the entry was created.  */
shr_error_e
ctest_flow_ut_run(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint32 keep_entry,
    bcm_gport_t * flow_id,
    int is_short_mode)
{
    /** general variables */
    uint8 result_type = 0;
    uint8 entry_written = FALSE;
    int nof_res_types;
    int is_res_type_supported;
    int is_table_active;

    SHR_FUNC_INIT_VARS(unit);

    if (FLOW_APP_IS_NOT_VALID(flow_app_info))
    {
        LOGI(LM, (U(unit, "Test skipped Application is invalid %s\n"), flow_app_info->app_name));
        SHR_EXIT();
    }

    LOGI(LM, (U(unit, "Testing application %s\n"), flow_app_info->app_name));
    SHR_IF_ERR_EXIT(ctest_flow_suffix_check(unit, flow_app_info));
    SHR_IF_ERR_EXIT(ctest_flow_virtual_indication_check(unit, flow_app_info));

    SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, FLOW_LIF_DBAL_TABLE_GET(flow_app_info), &is_table_active));
    if (!is_table_active)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "For Application %s DBAL table is not active \n", flow_app_info->app_name);
    }

    if (is_short_mode)
    {
        /** short mode only validate that the app definition is correct */
        LOGI(LM, (U(unit, "Application %s test in short mode finished successfully\n\n"), flow_app_info->app_name));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_nof_res_type_get(unit, FLOW_LIF_DBAL_TABLE_GET(flow_app_info), &nof_res_types));

    for (result_type = 0; result_type < nof_res_types; result_type++)
    {
        SHR_IF_ERR_EXIT(ctest_flow_ut_result_type_is_supported(unit, FLOW_LIF_DBAL_TABLE_GET(flow_app_info),
                                                               result_type, &is_res_type_supported));

        /** is result_type is not valid for this app, skip it */
        if (!is_res_type_supported)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(ctest_flow_ut_single_result_type_run(unit, flow_app_info, flow_id, result_type,
                                                             keep_entry, &entry_written));

        if (keep_entry && entry_written)
        {
            /** for keep entry, exit after one entry was written */
            SHR_EXIT();
        }
    }

    if (keep_entry)
    {
        /** in case we get to this place with keep_entry it means that we didn't add an entry, we should fail the test */
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_INTERNAL);
    }
    LOGI(LM, (U(unit, "Application %s test finished successfully\n\n"), flow_app_info->app_name));

exit:
    SHR_FUNC_EXIT;
}

/** incase app has multiple tables, using the app callback to translate between dbal table and key field. there are two
 *  modes 1. before add - need to update the value of the specail field. 2. after get - need to compare the value and
 *  retreive the old value */
static shr_error_e
ctest_flow_match_multiple_tables_handle_field(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * bcm_special_fields,
    dbal_tables_e dbal_table,
    uint8 result_type,
    uint8 is_after_get)
{
    int ii;
    bcm_flow_special_fields_t temp_bcm_special_fields = { 0 };
    bcm_flow_special_field_t *table_related_special_field = &(temp_bcm_special_fields.special_fields[0]);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flow_app_info->dbal_table_to_key_field_map(unit, flow_handle_info, FLOW_MAP_TYPE_TABLE_TO_KEY_FIELD,
                                                               &temp_bcm_special_fields, &dbal_table));
    if (temp_bcm_special_fields.actual_nof_special_fields == 1)
    {
        for (ii = 0; ii < bcm_special_fields->actual_nof_special_fields; ii++)
        {
            if (table_related_special_field->field_id == bcm_special_fields->special_fields[ii].field_id)
            {
                bcm_flow_special_field_t *special_field = &bcm_special_fields->special_fields[ii];
                if (!is_after_get)
                {
                    LOGI(LM, (U(unit, "\t Multiple dbal tables updating field %s to fit dbal table\n"),
                              dnx_flow_special_field_to_string(unit, table_related_special_field->field_id)));

                    sal_memcpy(special_field, table_related_special_field, sizeof(bcm_flow_special_field_t));
                }
                else
                {
                    bcm_flow_special_field_t bcm_value;
                    const flow_special_field_info_t *special_field_info;

                    SHR_IF_ERR_EXIT(flow_special_field_info_get
                                    (unit, table_related_special_field->field_id, &special_field_info));
                    SHR_IF_ERR_EXIT(ctest_flow_special_field_value_get
                                    (unit, bcm_special_fields, table_related_special_field->field_id, &bcm_value));
                    SHR_IF_ERR_EXIT(ctest_dnx_flow_ut_special_fields_compare
                                    (unit, special_field_info, &bcm_value, table_related_special_field));
                    SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_value_get
                                    (unit, flow_app_info, result_type, FALSE, TRUE,
                                     table_related_special_field->field_id, special_field));

                    special_field->field_id = table_related_special_field->field_id;
                }
                break;
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Expected that dbal_table_to_key_field_map will map 1 key field, instead has %d\n",
                     temp_bcm_special_fields.actual_nof_special_fields);
    }

exit:
    SHR_FUNC_EXIT;
}
int
ctest_flow_match_info_traverse_dummy_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * key_special_fields,
    void *user_data)
{
    return 0;
}
/**
 *  Running ut for match application
 *  Finding related initiator/match application
 *  Running UT test and without deleting the entry (keeping the LIF entry)
 *  Running match UT test using the LIF from the previouse stage
 *  Deleting the LIF
 *   */
static shr_error_e
ctest_flow_ut_match_run_specific_table(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    dbal_tables_e dbal_table,
    bcm_gport_t flow_id)
{
    bcm_flow_special_fields_t bcm_special_fields = { 0 };
    dbal_table_type_e table_type;
    flow_special_fields_e flow_special_fields_ids[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS];
    uint8 result_type = 0;
    uint32 orig_flags = flow_handle_info->flags;

    SHR_FUNC_INIT_VARS(unit);

    LOGI(LM, (U(unit, "Phase 1 - Updating entry to add\n")));

    sal_memcpy(flow_special_fields_ids, flow_app_info->special_fields, sizeof(flow_special_fields_ids));

    SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_table, &table_type));
    if (table_type == DBAL_TABLE_TYPE_TCAM)
    {
        /** for TCAM tables add priority */
        flow_handle_info->flow_priority = CTEST_FLOW_ENTRY_PRIORITY_VALUE;
    }

    SHR_IF_ERR_EXIT(ctest_flow_ut_special_fields_struct_fill(unit, flow_app_info, result_type, FALSE, FALSE,
                                                             flow_special_fields_ids, &bcm_special_fields));

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES))
    {
        SHR_IF_ERR_EXIT(ctest_flow_match_multiple_tables_handle_field
                        (unit, flow_app_info, flow_handle_info, &bcm_special_fields, dbal_table, result_type, FALSE));
    }

    LOGI(LM, (U(unit, "Phase 2 - Adding entry\n")));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_add(unit, flow_handle_info, &bcm_special_fields));

    LOGI(LM, (U(unit, "Phase 3 - Getting entry \n")));

    /** setting back to priority to zero anyway, in order to check that it is being get correctly */
    flow_handle_info->flow_priority = 0;
    SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_get(unit, flow_handle_info, &bcm_special_fields));

    if ((table_type == DBAL_TABLE_TYPE_TCAM) && (flow_handle_info->flow_priority != CTEST_FLOW_ENTRY_PRIORITY_VALUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Failed to verify entry priority expected %d received %d \n",
                     CTEST_FLOW_ENTRY_PRIORITY_VALUE, flow_handle_info->flow_priority);
    }

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES))
    {
        /** incase app has multiple tables, doing the opossite operation. checking that the returned value is as expected and
         *  returning the original value */
        SHR_IF_ERR_EXIT(ctest_flow_match_multiple_tables_handle_field
                        (unit, flow_app_info, flow_handle_info, &bcm_special_fields, dbal_table, result_type, TRUE));
    }

    /** verify the integrity of the special fields */
    SHR_IF_ERR_EXIT(ctest_flow_ut_struct_special_fields_verify(unit, flow_app_info, result_type, FALSE,
                                                               &bcm_special_fields, flow_special_fields_ids));

    LOGI(LM, (U(unit, "\tEntry verified\n")));

    /** removing all flags for traverse operation */
    flow_handle_info->flags = 0;
    LOGI(LM, (U(unit, "Phase 4 - Traversing the entry\n")));

    /** traverse using traverse API validations done inside the traverse */
    SHR_IF_ERR_EXIT(ctest_flow_ut_application_traverse(unit, flow_app_info, flow_handle_info, flow_special_fields_ids));

    /** traverse using flow_match_entry_from_global_lif_get */
    sal_memset(&bcm_special_fields, 0, sizeof(bcm_flow_special_fields_t));
    SHR_IF_ERR_EXIT(flow_match_entry_from_global_lif_get
                    (unit, _SHR_GPORT_FLOW_LIF_VAL_GET(flow_handle_info->flow_id), flow_handle_info,
                     &bcm_special_fields));

    SHR_IF_ERR_EXIT(ctest_flow_ut_struct_special_fields_verify(unit, flow_app_info, result_type, FALSE,
                                                               &bcm_special_fields, flow_special_fields_ids));

    /** restoring flags */
    flow_handle_info->flags = orig_flags;
    LOGI(LM, (U(unit, "Phase 5 - Deleting the entry\n")));

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES))
    {
        SHR_IF_ERR_EXIT(ctest_flow_match_multiple_tables_handle_field
                        (unit, flow_app_info, flow_handle_info, &bcm_special_fields, dbal_table, result_type, FALSE));
    }

    SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, flow_handle_info, &bcm_special_fields));

    LOGI(LM,
         (U(unit, "Semantic validations completed for table %s\n"), dbal_logical_table_to_string(unit, dbal_table)));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_ut_match_run(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    int is_short_mode)
{
    bcm_gport_t flow_id;
    bcm_flow_handle_info_t lif_handle_info = { 0 };
    bcm_flow_handle_t related_lif_app_handle_id;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    int rv, ii = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (FLOW_APP_IS_NOT_VALID(flow_app_info))
    {
        LOGI(LM, (U(unit, "Test skipped Application is invalid %s\n"), flow_app_info->app_name));
        SHR_EXIT();
    }

    LOGI(LM, (U(unit, "Start testing application %s\n"), flow_app_info->app_name));

    LOGI(LM, (U(unit, "\n\n General App validations \n")));
    SHR_IF_ERR_EXIT(ctest_flow_match_validity_check(unit, flow_app_info));
    SHR_IF_ERR_EXIT(ctest_flow_suffix_check(unit, flow_app_info));
    SHR_IF_ERR_EXIT(ctest_flow_match_payload_check(unit, flow_app_info));

    if (is_short_mode)
    {
        /** short mode only validate that the app definition is correct */
        LOGI(LM, (U(unit, "Application %s test in short mode finished successfully\n\n"), flow_app_info->app_name));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, flow_app_info->app_name, &flow_handle_info.flow_handle));

    LOGI(LM, (U(unit, "Test preparations\n")));

    /**deleting all the entries before starting the test */
    LOGI(LM, (U(unit, "\tDeleting all the entries \n")));
    SHR_IF_ERR_EXIT(dnx_flow_match_info_traverse
                    (unit, &flow_handle_info, ctest_flow_match_info_traverse_dummy_cb, (void *) NULL,
                     FLOW_TRAVERSE_OPERATION_DELETE));

    LOGI(LM, (U(unit, "\tAdding a LIF entry \n")));
    rv = ctest_flow_match_related_lif_entry_add(unit, flow_app_info, &related_lif_app_handle_id, &flow_id);

    if (rv == _SHR_E_CONFIG)
    {
        /** unable to add LIF entry, exiting the test without an error */
        SHR_EXIT();
    }

    /** Allocate MPLS gport */
    SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_glob_in_lif_create(unit, flow_app_info));

    lif_handle_info.flow_id = flow_id;
    lif_handle_info.flow_handle = related_lif_app_handle_id;
    flow_handle_info.flow_id = flow_id;

    LOGI(LM, (U(unit, "\n\n Running semantic Validations for table %s\n"),
              dbal_logical_table_to_string(unit, flow_app_info->flow_table[0])));

    while ((flow_app_info->flow_table[ii] != DBAL_TABLE_EMPTY) && (ii < FLOW_MAX_NOF_MATCH_DBAL_TABLES))
    {
        SHR_IF_ERR_EXIT_NO_MSG(ctest_flow_ut_match_run_specific_table
                               (unit, flow_app_info, &flow_handle_info, flow_app_info->flow_table[ii], flow_id));
        ii++;
    }

    if (_SHR_IS_FLAG_SET(flow_app_info->valid_flow_flags, BCM_FLOW_HANDLE_INFO_BUD))
    {
        dbal_tables_e dbal_table = flow_app_info->second_pass_table;

        if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD))
        {
            dbal_table = flow_app_info->flow_table[0];
        }
        /** if BUD supported, run the same test for BUD table */
        LOGI(LM, (U(unit, "\n\n Running BUD test for application %s\n"), flow_app_info->app_name));
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_BUD;

        SHR_IF_ERR_EXIT_NO_MSG(ctest_flow_ut_match_run_specific_table
                               (unit, flow_app_info, &flow_handle_info, dbal_table, flow_id));
    }

    LOGI(LM, (U(unit, "Test cleanups\n")));
    /** Delete the MPLS gport */
    SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_glob_in_lif_delete(unit, flow_app_info));

    LOGI(LM, (U(unit, "\tDeleting the LIF entry, ID 0x%x\n"), (uint32) flow_id));
    SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, FLOW_APP_TYPE_TERM, &lif_handle_info, NULL));

    LOGI(LM, (U(unit, "Application %s test finished successfully\n\n"), flow_app_info->app_name));

exit:
    SHR_FUNC_EXIT;
}
/** this function run a ut test */
shr_error_e
cmd_dnx_flow_ut(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *application = NULL;
    dnx_flow_app_type_e type_in = FLOW_APP_TYPE_NOF_TYPES;
    const dnx_flow_app_config_t *flow_app_info;
    int app_idx, num_apps = dnx_flow_number_of_apps();
    int complete_test_result = 0;
    int test_result = 0;
    bcm_flow_handle_t flow_handle_id;
    int is_short_mode = FALSE;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("application", application);
    SH_SAND_GET_ENUM("Type", type_in);
    SH_SAND_GET_BOOL("Mode", is_short_mode);

    if (type_in != FLOW_APP_TYPE_NOF_TYPES)
    {
        if (!ISEMPTY(application))
        {
            LOG_CLI((BSL_META("cannot support both type and application as an input\n")));
            SHR_EXIT();
        }
    }
    else if (!ISEMPTY(application))
    {
        int rv = diag_flow_app_from_string(unit, application, FLOW_APP_TYPE_NOF_TYPES, &flow_handle_id, sand_control);
        if (rv != _SHR_E_NONE)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }

        flow_app_info = dnx_flow_app_info_get(unit, flow_handle_id);

        if (FLOW_APP_IS_MATCH(flow_app_info->flow_app_type))
        {
            test_result = ctest_flow_ut_match_run(unit, flow_app_info, is_short_mode);
        }
        else
        {
            test_result = ctest_flow_ut_run(unit, flow_app_info, 0, NULL, is_short_mode);
        }
        if (!test_result)
        {
            LOG_CLI((BSL_META("Flow Test for application %s PASSED\n\n"), flow_app_info->app_name));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Flow Test for Application %s FAILED\n\n", flow_app_info->app_name);
        }
        SHR_EXIT();
    }

    /** if user didn't pass a specific application run the tests over all applications */
    PRT_TITLE_SET("flow Unit-Tests results");

    PRT_COLUMN_ADD("Test name");
    PRT_COLUMN_ADD("Result");

    for (app_idx = FLOW_APP_FIRST_IDX; app_idx < num_apps; app_idx++)
    {
        flow_app_info = dnx_flow_app_info_get(unit, app_idx);

        if (FLOW_APP_IS_NOT_VALID(flow_app_info))
        {
            continue;
        }

        if ((type_in != FLOW_APP_TYPE_NOF_TYPES) && (flow_app_info->flow_app_type != type_in))
        {
            /** skipping applications with type different then requested */
            continue;
        }
        if (FLOW_APP_IS_MATCH(flow_app_info->flow_app_type))
        {
            test_result = ctest_flow_ut_match_run(unit, flow_app_info, is_short_mode);
        }
        else
        {
            test_result = ctest_flow_ut_run(unit, flow_app_info, 0, NULL, is_short_mode);
        }
        complete_test_result |= test_result;

        /** updating the PRT table */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", flow_app_info->app_name);
        PRT_CELL_SET("%s", test_result ? "FAIL" : "PASS");
    }

    PRT_COMMITX;
    LOG_CLI((BSL_META("\n")));

    if (!complete_test_result)
    {
        LOG_CLI((BSL_META("Flow Test for ALL applications PASSED\n")));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Flow Test for Application FAILED\n");
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#undef LM
#undef U
#undef LOGI

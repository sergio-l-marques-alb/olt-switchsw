/** \file ctest_dnx_flow.c
 *
 * Main ctest file for flow tests
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
#include <sal/core/boot.h>
#include <bcm/flow.h>
#include <bcm_int/dnx/flow/flow.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/ctest/dnxc/ctest_dnxc_system.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <include/bcm_int/dnx/algo/rx/algo_rx.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_rx_access.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <bcm_int/dnx/flow/flow_lif_mgmt.h>

#include "ctest_dnx_flow.h"
#include "src/appl/diag/dnx/flow/diag_dnx_flow.h"

/*************
 * TYPEDEFS  *
 *************/

#define CTEST_FLOW_BASIC_TEST_MAX_NOF_ENTRIES FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS

/* Global list, holds the flow ids for traverse*/
int basic_test_flow_ids[CTEST_FLOW_BASIC_TEST_MAX_NOF_ENTRIES] = { 0 };

/** Common field descriptor */
typedef struct
{
    uint32 enabler;             /* Field enabler */
    int illegal_value;          /* Illegal value example. Use CTEST_FLOW_SKIP_FIELD to skip field check */
    int legal_value;            /* Legal value example. Use CTEST_FLOW_SKIP_FIELD to skip field check */
} ctest_flow_common_field_desc_t;

/** Special field descriptor */
typedef struct
{
    flow_special_fields_e field_id;     /* field ID */
    int illegal_value;          /* Illegal value example. Use CTEST_FLOW_SKIP_FIELD to skip field check */
    int arr32_illegal_value[BCM_FLOW_SPECIAL_FIELD_UIN32_ARR_MAX_SIZE]; /* Illegal arr32 value example. Use
                                                                         * CTEST_FLOW_SKIP_FIELD to skip field check */
    int arr8_illegal_value[2];  /* Illegal arr8 value example. Use CTEST_FLOW_SKIP_FIELD to skip field check */
    int illegal_symbol;         /* Illegal symbol value example. Use CTEST_FLOW_SKIP_FIELD to skip field check */
} ctest_flow_special_field_desc_t;

#define CTEST_DNX_FLOW_SKIP_SPECIAL_FIELD_ILLEGAL(curr_field, ctest_special_field) \
    (((curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT32) && (ctest_special_field->illegal_value == CTEST_FLOW_SKIP_FIELD)) || \
     ((curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT32_ARR) && (ctest_special_field->arr32_illegal_value[0] == CTEST_FLOW_SKIP_FIELD)) || \
     ((curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT8_ARR) && (ctest_special_field->arr8_illegal_value[0] == CTEST_FLOW_SKIP_FIELD)) || \
     ((curr_field->payload_type == FLOW_PAYLOAD_TYPE_ENUM) && (ctest_special_field->illegal_symbol == CTEST_FLOW_SKIP_FIELD)) || \
     ((curr_field->payload_type == FLOW_PAYLOAD_TYPE_BCM_ENUM) && (ctest_special_field->illegal_symbol == CTEST_FLOW_SKIP_FIELD)))

/** for both macros, for type FLOW_PAYLOAD_TYPE_ENABLER there is no value */
#define CTEST_DNX_FLOW_SPECIAL_FIELD_SET(curr_field, set_special_field, special_field_idx) \
        if (curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT32_ARR) \
        { \
            sal_memcpy(special_fields.special_fields[special_field_idx].shr_var_uint32_arr, set_special_field.shr_var_uint32_arr, \
                       sizeof(set_special_field.shr_var_uint32_arr)); \
        } \
        else if (curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT8_ARR) \
        { \
            sal_memcpy(special_fields.special_fields[special_field_idx].shr_var_uint8_arr, set_special_field.shr_var_uint8_arr, \
                       sizeof(set_special_field.shr_var_uint8_arr)); \
        } \
        else if ((curr_field->payload_type == FLOW_PAYLOAD_TYPE_ENUM) || (curr_field->payload_type == FLOW_PAYLOAD_TYPE_BCM_ENUM)) \
        { \
            special_fields.special_fields[special_field_idx].symbol = set_special_field.symbol; \
        } \
        else if (curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT32)\
        { \
            special_fields.special_fields[special_field_idx].shr_var_uint32 = set_special_field.shr_var_uint32; \
        }

#define CTEST_DNX_FLOW_SPECIAL_FIELD_SET_ILLEGAL(curr_field, ctest_special_field) \
        if (curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT32_ARR) \
        { \
            sal_memcpy(special_fields.special_fields[0].shr_var_uint32_arr, ctest_special_field->arr32_illegal_value, \
                       sizeof(ctest_special_field->arr32_illegal_value)); \
        } \
        else if (curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT8_ARR) \
        { \
            sal_memcpy(special_fields.special_fields[0].shr_var_uint8_arr, ctest_special_field->arr8_illegal_value, \
                       sizeof(ctest_special_field->arr8_illegal_value)); \
        } \
        else if ((curr_field->payload_type == FLOW_PAYLOAD_TYPE_ENUM) || (curr_field->payload_type == FLOW_PAYLOAD_TYPE_BCM_ENUM)) \
        { \
            special_fields.special_fields[0].symbol = ctest_special_field->illegal_symbol; \
        } \
        else if (curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT32)\
        { \
            special_fields.special_fields[0].shr_var_uint32 = ctest_special_field->illegal_value; \
        }

/********************************************
*   ENABLER     *     ILLEGAL VALUE         *
*               *                           *
*               *                           *
*********************************************/
ctest_flow_common_field_desc_t ctest_dnx_flow_term_common_fields[] = {
    {BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID, -1, 1},
    {BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID, -1, 1},
    {BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID, -1, 1},
    {BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID, -1, 1},
    {BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID, 4, 1},
    {BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_ID_VALID, -1, 0x60000001},
    {BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_STATE_VALID, -1, 1},
    {BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID, CTEST_FLOW_SKIP_FIELD, CTEST_FLOW_SKIP_FIELD},
    {BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID, -1, CTEST_FLOW_SKIP_FIELD},
    {BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID, -1, 1},
    {BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID, -1, 1},
    {BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID, -1, CTEST_FLOW_SKIP_FIELD},
    {BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID, -1, CTEST_FLOW_SKIP_FIELD},
    {BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID, 0, 0x16000001},
    {BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID, -1, 0x10030001},
    {BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID, CTEST_FLOW_SKIP_FIELD, CTEST_FLOW_SKIP_FIELD},
};

ctest_flow_common_field_desc_t ctest_dnx_flow_init_common_fields[] = {
    {BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID, -1, 1},
    {BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID, -1, 1},
    {BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID, 0, 0x16000C01},
    {BCM_FLOW_INITIATOR_ELEMENT_MTU_PROFILE_VALID, -1, 1},
    {BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_ID_VALID, -1, 0x80000001},
    {BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_STATE_VALID, -1, 1},
    {BCM_FLOW_INITIATOR_ELEMENT_TPID_CLASS_ID_VALID, -1, 1},
    {BCM_FLOW_INITIATOR_ELEMENT_CLASS_ID_VALID, -1, 1},
    {BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID, CTEST_FLOW_SKIP_FIELD, CTEST_FLOW_SKIP_FIELD},
    {BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID, -1, CTEST_FLOW_SKIP_FIELD},
    {BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID, CTEST_FLOW_SKIP_FIELD, CTEST_FLOW_SKIP_FIELD},
    {BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID, CTEST_FLOW_SKIP_FIELD, CTEST_FLOW_SKIP_FIELD},
};

ctest_flow_special_field_desc_t ctest_dnx_flow_special_fields[] = {
    {FLOW_S_F_TUNNEL_ENDPOINT_IDENTIFIER, CTEST_FLOW_SKIP_FIELD, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_MPLS_LABEL, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_MPLS_LABEL_2, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_MPLS_PHP, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_NEXT_LAYER_NETWORK_DOMAIN, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_IPV4_SIP, CTEST_FLOW_SKIP_FIELD, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_IPV4_DIP, CTEST_FLOW_SKIP_FIELD, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_VLAN_EDIT_PROFILE, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_VLAN_EDIT_PCP_DEI_PROFILE, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_VLAN_EDIT_VID_1, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_VLAN_EDIT_VID_2, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_VSI_ASSIGNMENT_MODE, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_PORT, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_VLAN_DOMAIN, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_S_VID, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_C_VID, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_S_VID_1, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_S_VID_2, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_C_VID_1, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_C_VID_2, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_MAPPED_PORT, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_VID_OUTER_VLAN, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_VID_INNER_VLAN, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_PCP_DEI_OUTER_VLAN, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_LAYER_TYPE, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_QOS_TTL, CTEST_FLOW_SKIP_FIELD, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_QOS_DSCP, CTEST_FLOW_SKIP_FIELD, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_QOS_EXP, CTEST_FLOW_SKIP_FIELD, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_QOS_PRI, CTEST_FLOW_SKIP_FIELD, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_QOS_CFI, CTEST_FLOW_SKIP_FIELD, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_GTP_PDU_TYPE, CTEST_FLOW_SKIP_FIELD, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_BITSTR, CTEST_FLOW_SKIP_FIELD, {-1, -1, -1, -1, -1, -1, -1, -1}, {0}, CTEST_FLOW_SKIP_FIELD},

    {FLOW_S_F_VSI_MATCH, CTEST_FLOW_SKIP_FIELD, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_CLASS_ID_MATCH, CTEST_FLOW_SKIP_FIELD, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_NAME_SPACE_MATCH, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_VID_MATCH, CTEST_FLOW_SKIP_FIELD, {0}, {0}, CTEST_FLOW_SKIP_FIELD},
    {FLOW_S_F_NOF_OF_LAYERS_TO_TERMINATE, -1, {0}, {0}, CTEST_FLOW_SKIP_FIELD}

};

typedef enum
{
    /** An Application that represents a LIF Terminator */
    FLOW_ALLOWED_APP_TYPES_TERM,

    /** An Application that represents a LIF Initiator */
    FLOW_ALLOWED_APP_TYPES_INIT,

    /** An Application that represents a Terminator LIF Match */
    FLOW_ALLOWED_APP_TYPES_TERM_MATCH,

    /** An Application that represents an Initiator LIF Match */
    FLOW_ALLOWED_APP_TYPES_INIT_MATCH,

    FLOW_ALLOWED_APP_TYPES_LIF,

    FLOW_ALLOWED_APP_TYPES_MATCH,

    FLOW_ALLOWED_APP_TYPES_ALL
} ctest_flow_allowed_app_types_e;

/*************
* FUNCTIONS *
*************/

/** Common function that verify basic test params, in case input patram error will return _SHR_E_PARAM */
static shr_error_e
ctest_flow_test_begin(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    ctest_flow_allowed_app_types_e allowed_app_type,
    uint32 allowed_flag,
    const dnx_flow_app_config_t ** flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info)
{
    char *application;
    int rv;
    uint32 flow_handle;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("application", application);

    rv = diag_flow_app_from_string(unit, application, FLOW_APP_TYPE_NOF_TYPES, &flow_handle, sand_control);
    if (rv != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unrecognized application %s\n", application);
    }

    (*flow_app_info) = dnx_flow_app_info_get(unit, flow_handle);
    if (FLOW_APP_IS_NOT_VALID((*flow_app_info)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Test skipped application %s since it is not valid \n", (*flow_app_info)->app_name);
    }

    if (flow_handle_info)
    {
        flow_handle_info->flow_handle = flow_handle;
    }

    if (allowed_app_type != FLOW_ALLOWED_APP_TYPES_ALL)
    {
        if (allowed_app_type >= FLOW_ALLOWED_APP_TYPES_INIT_MATCH)
        {
            if ((allowed_app_type == FLOW_ALLOWED_APP_TYPES_LIF) && (!FLOW_APP_IS_LIF((*flow_app_info)->flow_app_type)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "test supported only for LIF applications %s \n",
                             (*flow_app_info)->app_name);
            }

            if ((allowed_app_type == FLOW_ALLOWED_APP_TYPES_MATCH)
                && (!FLOW_APP_IS_MATCH((*flow_app_info)->flow_app_type)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "test supported only for MATCH applications %s \n",
                             (*flow_app_info)->app_name);
            }
        }
        else
        {
            if (allowed_app_type != (ctest_flow_allowed_app_types_e) ((*flow_app_info)->flow_app_type))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "test supported only for specific app type applications %s \n",
                             (*flow_app_info)->app_name);
            }
        }
    }

    if (allowed_flag)
    {
        if (!_SHR_IS_FLAG_SET((*flow_app_info)->valid_flow_flags, allowed_flag))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "test supported for applications with specific flag only\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_logger_close(
    int unit,
    bsl_severity_t * orig_lif_severity,
    bsl_severity_t * orig_dbal_severity,
    bsl_severity_t * orig_soc_dbal_severity,
    bsl_severity_t * orig_flow_severity,
    bsl_severity_t * orig_port_severity,
    bsl_severity_t * orig_qos_severity)
{
    SHR_FUNC_INIT_VARS(unit);

    *orig_lif_severity = bslenable_get(bslLayerBcmdnx, bslSourceLif);
    *orig_dbal_severity = bslenable_get(bslLayerBcmdnx, bslSourceDbaldnx);
    *orig_soc_dbal_severity = bslenable_get(bslLayerSocdnx, bslSourceDbaldnx);
    *orig_flow_severity = bslenable_get(bslLayerBcmdnx, bslSourceFlow);
    *orig_port_severity = bslenable_get(bslLayerBcmdnx, bslSourcePort);
    *orig_qos_severity = bslenable_get(bslLayerBcmdnx, bslSourceQos);
    bslenable_set(bslLayerBcmdnx, bslSourceLif, bslSeverityOff);
    bslenable_set(bslLayerBcmdnx, bslSourceDbaldnx, bslSeverityOff);
    bslenable_set(bslLayerBcmdnx, bslSourceFlow, bslSeverityOff);
    bslenable_set(bslLayerSocdnx, bslSourceDbaldnx, bslSeverityOff);
    bslenable_set(bslLayerBcmdnx, bslSourcePort, bslSeverityOff);
    bslenable_set(bslLayerBcmdnx, bslSourceQos, bslSeverityOff);

    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_logger_restore(
    int unit,
    bsl_severity_t orig_lif_severity,
    bsl_severity_t orig_dbal_severity,
    bsl_severity_t orig_soc_dbal_severity,
    bsl_severity_t orig_flow_severity,
    bsl_severity_t orig_port_severity,
    bsl_severity_t orig_qos_severity)
{
    SHR_FUNC_INIT_VARS(unit);

    bslenable_set(bslLayerBcmdnx, bslSourceLif, orig_lif_severity);
    bslenable_set(bslLayerBcmdnx, bslSourceDbaldnx, orig_dbal_severity);
    bslenable_set(bslLayerSocdnx, bslSourceDbaldnx, orig_soc_dbal_severity);
    bslenable_set(bslLayerBcmdnx, bslSourceFlow, orig_flow_severity);
    bslenable_set(bslLayerBcmdnx, bslSourcePort, orig_port_severity);
    bslenable_set(bslLayerBcmdnx, bslSourceQos, orig_qos_severity);

    SHR_FUNC_EXIT;
}

/* Check if given flow id is part of the added test's flow id */
static uint8
ctest_flow_is_flow_id_related(
    int flow_id)
{
    int flow_id_idx = 0;

    for (flow_id_idx = 0; flow_id_idx < CTEST_FLOW_BASIC_TEST_MAX_NOF_ENTRIES; flow_id_idx++)
    {
        if (basic_test_flow_ids[flow_id_idx] == flow_id)
        {
            return TRUE;
        }
        if (basic_test_flow_ids[flow_id_idx] == 0)
        {
            return FALSE;
        }
    }
    return FALSE;
}

int
ctest_flow_term_delete_callback(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (ctest_flow_is_flow_id_related(flow_handle_info->flow_id))
    {
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_destroy(unit, flow_handle_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/** count the number of BUD entries   */
int
ctest_flow_match_callback(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * special_fields,
    void *user_data)
{
    int *user_data_as_int = (int *) user_data;

    SHR_FUNC_INIT_VARS(unit);

    if ((flow_handle_info->flags & BCM_FLOW_HANDLE_INFO_BUD))
    {
        (*user_data_as_int)++;
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

int
ctest_flow_init_delete_callback(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (ctest_flow_is_flow_id_related(flow_handle_info->flow_id))
    {
        SHR_IF_ERR_EXIT(bcm_flow_initiator_info_destroy(unit, flow_handle_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set relevant flow terminator struct member by field enabler.
 *
 * \return
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
ctest_flow_term_set_by_enabler(
    int unit,
    int enabler,
    bcm_flow_terminator_info_t * terminator_info,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (enabler)
    {
        case BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID:
            terminator_info->vrf = value;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID:
            terminator_info->stat_id = value;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID:
            terminator_info->stat_pp_profile = value;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID:
            terminator_info->l3_ingress_info.urpf_mode = value;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID:
            COMPILER_64_ZERO(terminator_info->additional_data);
            COMPILER_64_ADD_32(terminator_info->additional_data, value);
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID:
            terminator_info->action_gport = value;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID:
            terminator_info->qos_map_id = value;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID:
            terminator_info->dest_info.dest_port = value;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID:
            terminator_info->l2_ingress_info.ingress_network_group_id = value;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID:
            terminator_info->ingress_qos_model.ingress_phb = value;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID:
            terminator_info->class_id = value;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID:
            terminator_info->vsi = value;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_ID_VALID:
            terminator_info->failover_id = value;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_STATE_VALID:
            terminator_info->failover_state = value;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID:
            terminator_info->l2_learn_info.dest_port = value;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID:
            terminator_info->learn_enable = value;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported field enabler 0x%x\n", enabler);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set relevant flow terminator struct member by field enabler.
 *
 * \return
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
ctest_flow_term_get_by_enabler(
    int unit,
    int enabler,
    bcm_flow_terminator_info_t * terminator_info,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (enabler)
    {
        case BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID:
            *value = terminator_info->vrf;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID:
            *value = terminator_info->stat_id;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID:
            *value = terminator_info->stat_pp_profile;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID:
            *value = terminator_info->l3_ingress_info.urpf_mode;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID:
            *value = COMPILER_64_LO(terminator_info->additional_data);
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID:
            *value = terminator_info->action_gport;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID:
            *value = terminator_info->qos_map_id;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID:
            *value = terminator_info->dest_info.dest_port;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID:
            *value = terminator_info->ingress_qos_model.ingress_phb;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID:
            *value = terminator_info->class_id;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID:
            *value = terminator_info->vsi;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_ID_VALID:
            *value = terminator_info->failover_id;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_STATE_VALID:
            *value = terminator_info->failover_state;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID:
            *value = terminator_info->l2_learn_info.dest_port;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID:
            *value = terminator_info->learn_enable;
            break;
        case BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID:
            *value = terminator_info->l2_ingress_info.ingress_network_group_id;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported field enabler 0x%x\n", enabler);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set relevant flow initiator struct member by field enabler.
 *
 * \return
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
ctest_flow_init_set_by_enabler(
    int unit,
    int enabler,
    bcm_flow_initiator_info_t * initiator_info,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (enabler)
    {
        case BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID:
            initiator_info->stat_id = value;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID:
            initiator_info->stat_pp_profile = value;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID:
            initiator_info->l3_intf_id = value;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID:
            initiator_info->action_gport = value;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_MTU_PROFILE_VALID:
            initiator_info->mtu_profile = value;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID:
            initiator_info->qos_map_id = value;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID:
            initiator_info->egress_qos_model.egress_qos = value;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_ID_VALID:
            initiator_info->failover_id = value;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_STATE_VALID:
            initiator_info->failover_state = value;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID:
            initiator_info->l2_egress_info.egress_network_group_id = value;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_TPID_CLASS_ID_VALID:
            initiator_info->tpid_class_id = value;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_CLASS_ID_VALID:
            initiator_info->class_id = value;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported field enabler 0x%x\n", enabler);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get relevant flow initiator struct member by field enabler.
 *
 * \return
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
ctest_flow_init_get_by_enabler(
    int unit,
    int enabler,
    bcm_flow_initiator_info_t * initiator_info,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (enabler)
    {
        case BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID:
            *value = initiator_info->stat_id;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID:
            *value = initiator_info->stat_pp_profile;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID:
            *value = initiator_info->l3_intf_id;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID:
            *value = initiator_info->action_gport;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_MTU_PROFILE_VALID:
            *value = initiator_info->mtu_profile;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID:
            *value = initiator_info->qos_map_id;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_ID_VALID:
            *value = initiator_info->failover_id;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_STATE_VALID:
            *value = initiator_info->failover_state;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID:
            *value = initiator_info->egress_qos_model.egress_qos;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID:
            *value = initiator_info->l2_egress_info.egress_network_group_id;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_TPID_CLASS_ID_VALID:
            *value = initiator_info->tpid_class_id;
            break;
        case BCM_FLOW_INITIATOR_ELEMENT_CLASS_ID_VALID:
            *value = initiator_info->class_id;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported field enabler 0x%x\n", enabler);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set relevant flow struct member by field enabler - call terminator or initiator function.
 *
 * \return
 * _SHR_E_NONE - Successful command
 */
shr_error_e
ctest_flow_set_by_enabler(
    int unit,
    dnx_flow_app_type_e app_type,
    int enabler,
    void *app_data,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    if (app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(ctest_flow_term_set_by_enabler(unit, enabler, (bcm_flow_terminator_info_t *) app_data, value));
    }
    else if (app_type == FLOW_APP_TYPE_INIT)
    {
        SHR_IF_ERR_EXIT(ctest_flow_init_set_by_enabler(unit, enabler, (bcm_flow_initiator_info_t *) app_data, value));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\nUnsupported app type %s, only INIT and TERM are supported\n",
                     flow_app_type_to_string(unit, app_type));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set relevant flow struct member by field enabler - call terminator or initiator function.
 *
 * \return
 * _SHR_E_NONE - Successful command
 */
shr_error_e
ctest_flow_get_by_enabler(
    int unit,
    dnx_flow_app_type_e app_type,
    int enabler,
    void *app_data,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    if (app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(ctest_flow_term_get_by_enabler(unit, enabler, (bcm_flow_terminator_info_t *) app_data, value));
    }
    else if (app_type == FLOW_APP_TYPE_INIT)
    {
        SHR_IF_ERR_EXIT(ctest_flow_init_get_by_enabler(unit, enabler, (bcm_flow_initiator_info_t *) app_data, value));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\nUnsupported app type %s, only INIT and TERM are supported\n",
                     flow_app_type_to_string(unit, app_type));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get field by enabler.
 *
 * \return
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
ctest_flow_common_field_get(
    int unit,
    int enabler,
    dnx_flow_app_type_e app_type,
    ctest_flow_common_field_desc_t ** field_desc)
{
    int field_idx;
    int num_fields;
    ctest_flow_common_field_desc_t *common_fields;

    SHR_FUNC_INIT_VARS(unit);

    if (app_type == FLOW_APP_TYPE_TERM)
    {
        common_fields = ctest_dnx_flow_term_common_fields;
    }
    else if (app_type == FLOW_APP_TYPE_INIT)
    {
        common_fields = ctest_dnx_flow_init_common_fields;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\nIllegal app type given %s, only TERM, INIT are supported\n",
                     flow_app_type_to_string(unit, app_type));
    }

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, app_type, &num_fields));

    for (field_idx = 0; field_idx < num_fields; field_idx++)
    {
        if (common_fields[field_idx].enabler == enabler)
        {
            *field_desc = &(common_fields[field_idx]);
            break;
        }
    }

    if (field_idx == num_fields)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\nError, couldn't find matching enabler 0x%x\n", enabler);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get special field by field id.
 *
 * \return
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
ctest_flow_special_field_get(
    int unit,
    flow_special_fields_e field_id,
    ctest_flow_special_field_desc_t ** ctest_flow_special_field)
{
    int num_fields, field_idx;

    SHR_FUNC_INIT_VARS(unit);

    num_fields = sizeof(ctest_dnx_flow_special_fields) / sizeof(ctest_dnx_flow_special_fields[0]);

    for (field_idx = 0; field_idx < num_fields; field_idx++)
    {
        if (ctest_dnx_flow_special_fields[field_idx].field_id == field_id)
        {
            *ctest_flow_special_field = &(ctest_dnx_flow_special_fields[field_idx]);
            break;
        }
    }

    if (field_idx == num_fields)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\nError, couldn't find matching special field for field %s\n",
                     dnx_flow_special_field_to_string(unit, field_id));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_special_field_validate_value(
    int unit,
    const flow_special_field_info_t * special_field_info,
    bcm_flow_special_field_t * special_field,
    bcm_flow_special_field_t * set_special_field)
{
    flow_payload_type_e payload_type = special_field_info->payload_type;

    SHR_FUNC_INIT_VARS(unit);

    /** for type FLOW_PAYLOAD_TYPE_ENABLER there is no value */
    if (((payload_type == FLOW_PAYLOAD_TYPE_UINT32_ARR)
         &&
         (!sal_memcmp
          (special_field->shr_var_uint32_arr, set_special_field->shr_var_uint32_arr,
           sizeof(set_special_field->shr_var_uint32_arr)))) || ((payload_type == FLOW_PAYLOAD_TYPE_UINT8_ARR)
                                                                &&
                                                                (!sal_memcmp
                                                                 (special_field->shr_var_uint8_arr,
                                                                  set_special_field->shr_var_uint32_arr,
                                                                  sizeof(set_special_field->shr_var_uint8_arr))))
        || ((payload_type == FLOW_PAYLOAD_TYPE_ENUM) && (special_field->symbol != set_special_field->symbol))
        || ((payload_type == FLOW_PAYLOAD_TYPE_BCM_ENUM) && (special_field->symbol != set_special_field->symbol))
        || ((payload_type == FLOW_PAYLOAD_TYPE_UINT32)
            && (special_field->shr_var_uint32 != set_special_field->shr_var_uint32)))
    {
        char get_print_buffer[FLOW_STR_MAX] = { '\0' };
        char set_print_buffer[FLOW_STR_MAX] = { '\0' };

        SHR_IF_ERR_EXIT(flow_special_field_printable_string_get
                        (unit, special_field->field_id, special_field, get_print_buffer));

        SHR_IF_ERR_EXIT(flow_special_field_printable_string_get
                        (unit, special_field->field_id, set_special_field, set_print_buffer));

        SHR_ERR_EXIT(_SHR_E_PARAM, "For special field %s, expected %s, got %s", special_field_info->name,
                     set_print_buffer, get_print_buffer);
    }

exit:
    SHR_FUNC_EXIT;
}
/** in init of flow */
static shr_error_e
ctest_flow_common_fields_unit_test_create(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Don't exit if deliberately including adapter unsupported tests
     */
    if (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_tests_supported)
        && !test_list->include_unsupported)
    {
        SHR_EXIT();
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add
                        (unit, test_list, "gtp_termination", "negative positive application=GTP_TERMINATOR", 0),
                        "Add test failed");
    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add
                        (unit, test_list, "gtp_initiator", "negative positive application=GTP_INITIATOR", 0),
                        "Add test failed");

    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add
                        (unit, test_list, "ipvx_terminator", "negative positive application=IPVX_TUNNEL_TERMINATOR", 0),
                        "Add test failed");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_basic_test_create(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Don't exit if deliberately including adapter unsupported tests
     */
    if (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_tests_supported)
        && !test_list->include_unsupported)
    {
        SHR_EXIT();
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "gtp_termination", "", 0), "Add test failed");
    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "gtp_initiator", "application=GTP_INITIATOR", 0),
                        "Add test failed");
    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "ipv4_initiator", "application=IPV4_TUNNEL_INITIATOR", 0),
                        "Add test failed");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_negative_test_create(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Don't exit if deliberately including adapter unsupported tests
     */
    if (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_tests_supported)
        && !test_list->include_unsupported)
    {
        SHR_EXIT();
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "gtp_termination", "", 0), "Add test failed");
    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "gtp_initiator", "application=GTP_INITIATOR", 0),
                        "Add test failed");
    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "gtp_match", "application=GTP_TERM_MATCH", 0),
                        "Add test failed");
    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "mpls_initiator", "application=MPLS_TUNNEL_INITIATOR", 0),
                        "Add test failed");
    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "ipv4_initiator", "application=IPV4_TUNNEL_INITIATOR", 0),
                        "Add test failed");
    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "ipv4_terminator", "application=IPVX_TUNNEL_TERMINATOR", 0),
                        "Add test failed");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_common_profiles_test_create(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Don't exit if deliberately including adapter unsupported tests
     */
    if (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_tests_supported)
        && !test_list->include_unsupported)
    {
        SHR_EXIT();
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "gtp_terminator", "", 0), "Add test failed");
    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add
                        (unit, test_list, "gtp_initiator", "application=GTP_INITIATOR", 0), "Add test failed");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_egress_qos_model_test_create(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Don't exit if deliberately including adapter unsupported tests
     */
    if (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_tests_supported)
        && !test_list->include_unsupported)
    {
        SHR_EXIT();
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add
                        (unit, test_list, "ipv4_initiator", "application=IPV4_TUNNEL_INITIATOR", 0), "Add test failed");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_special_fields_replace_test_create(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Don't exit if deliberately including adapter unsupported tests
     */
    if (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_tests_supported)
        && !test_list->include_unsupported)
    {
        SHR_EXIT();
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add
                        (unit, test_list, "vlan_port_term_initiator", "application=VLAN_PORT_LL_TERMINATOR", 0),
                        "Add test failed");
    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "vlan_port__init", "application=VLAN_PORT_LL_INITIATOR", 0),
                        "Add test failed");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_src_address_profile_test_create(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Don't exit if deliberately including adapter unsupported tests
     */
    if (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_tests_supported)
        && !test_list->include_unsupported)
    {
        SHR_EXIT();
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "arp_initiator", "application=ARP_INITIATOR", 0),
                        "Add test failed");

    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add
                        (unit, test_list, "ipv4_tunnel_initiator", "application=IPV4_TUNNEL_INITIATOR", 0),
                        "Add test failed");

    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add
                        (unit, test_list, "ipv6_tunnel_initiator", "application=IPV6_TUNNEL_INITIATOR", 0),
                        "Add test failed");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_lif_delete_all_test_create(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Don't exit if deliberately including adapter unsupported tests
     */
    if (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_tests_supported)
        && !test_list->include_unsupported)
    {
        SHR_EXIT();
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "ipv4_initiator", "application=IPV4_TUNNEL_INITIATOR", 0),
                        "Add test failed");

    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "ipvx_terminator", "application=IPVX_TUNNEL_TERMINATOR", 0),
                        "Add test failed");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_lif_per_core_test_create(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
        SHR_EXIT();
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "", "", 0), "Add test failed");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_bud_test_create(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
        SHR_EXIT();
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, "", "", 0), "Add test failed");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ctest_flow_entry_delete(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * key_special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Delete the entry
     */
    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_destroy(unit, flow_handle_info));
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        SHR_IF_ERR_EXIT(bcm_flow_initiator_info_destroy(unit, flow_handle_info));
    }
    else if (FLOW_APP_IS_MATCH(flow_app_type))
    {
        SHR_IF_ERR_EXIT(bcm_flow_match_info_delete(unit, flow_handle_info, key_special_fields));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_delete_entries(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info)
{
    int idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    while (basic_test_flow_ids[idx] != -1)
    {
        flow_handle_info->flow_id = basic_test_flow_ids[idx];
        SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_type, flow_handle_info, NULL));
        idx++;
    }

exit:
    SHR_FUNC_EXIT;
}

/** create a LIF entry initiator/terminator, in case of initiator & encap_required will set to default encap access
 *  if exists otherwise to bcmEncapAccessTunnel2 */
static shr_error_e
ctest_flow_lif_info_create(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    void *app_data,
    bcm_flow_special_fields_t * special_fields,
    uint32 field_enabler,
    int encap_required)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        ((bcm_flow_terminator_info_t *) app_data)->valid_elements_set = field_enabler;
        SHR_SET_CURRENT_ERR(bcm_flow_terminator_info_create
                            (unit, flow_handle_info, (bcm_flow_terminator_info_t *) app_data, special_fields));
    }
    else if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        ((bcm_flow_initiator_info_t *) app_data)->valid_elements_set = field_enabler;
        if (encap_required)
        {
            ((bcm_flow_initiator_info_t *) app_data)->encap_access =
                (flow_app_info->encap_access_default_mapping !=
                 bcmEncapAccessInvalid) ? flow_app_info->encap_access_default_mapping : bcmEncapAccessTunnel2;
        }

        SHR_SET_CURRENT_ERR(bcm_flow_initiator_info_create
                            (unit, flow_handle_info, (bcm_flow_initiator_info_t *) app_data, special_fields));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\n Invalid flow_app_type \n");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_lif_info_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    void *app_data,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_SET_CURRENT_ERR(bcm_flow_terminator_info_get
                            (unit, flow_handle_info, (bcm_flow_terminator_info_t *) app_data, special_fields));
    }
    else if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        SHR_SET_CURRENT_ERR(bcm_flow_initiator_info_get
                            (unit, flow_handle_info, (bcm_flow_initiator_info_t *) app_data, special_fields));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\n Invalid flow_app_type \n");
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Run common fields unit test.
 * Set illegal value per every field, expect error
 *
 * \return
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
ctest_flow_common_fields_negative_test(
    int unit,
    bcm_flow_handle_t flow_handle_id,
    const dnx_flow_app_config_t * flow_app_info)
{
    shr_error_e rv;
    int num_common_fields = 0, field_idx = 0;
    uint32 illegal_value = 0;
    const dnx_flow_common_field_desc_t *field_desc;
    int common_field_idx;
    uint32 field_enabler;
    void *app_data;
    bcm_flow_terminator_info_t terminator_info = { 0 };
    bcm_flow_initiator_info_t initiator_info = { 0 };
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };
    ctest_flow_special_field_desc_t *ctest_special_field;
    ctest_flow_common_field_desc_t *ctest_field_desc;
    bsl_severity_t lif_severity = bslSeverityOff, dbal_severity = bslSeverityOff, flow_severity =
        bslSeverityOff, dbal_soc_severity = bslSeverityOff, port_severity = bslSeverityOff, qos_severity =
        bslSeverityOff;
    const flow_special_field_info_t *curr_field;
    bcm_flow_special_field_t special_field_values;

    SHR_FUNC_INIT_VARS(unit);

    flow_handle_info.flow_handle = flow_handle_id;

    app_data =
        (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) ? (void *) &terminator_info : (void *) &initiator_info;

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_info->flow_app_type, &num_common_fields));

    for (common_field_idx = 0; common_field_idx < num_common_fields; common_field_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                        (unit, flow_app_info->flow_app_type, common_field_idx, &field_desc));

        if (field_desc == NULL)
        {
            continue;
        }

        field_enabler =
            (flow_app_info->flow_app_type ==
             FLOW_APP_TYPE_TERM) ? field_desc->term_field_enabler : field_desc->init_field_enabler;
        if (_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field_enabler))
        {
            SHR_IF_ERR_EXIT(ctest_flow_common_field_get
                            (unit, field_enabler, flow_app_info->flow_app_type, &ctest_field_desc));
            illegal_value = ctest_field_desc->illegal_value;
            if (illegal_value == CTEST_FLOW_SKIP_FIELD)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                            (unit, flow_app_info->flow_app_type, field_enabler, app_data, illegal_value));
            /**Don't show error messages*/
            SHR_IF_ERR_EXIT(ctest_flow_logger_close
                            (unit, &lif_severity, &dbal_severity, &dbal_soc_severity, &flow_severity, &port_severity,
                             &qos_severity));

            rv = ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, app_data, NULL, 1, field_enabler);
            SHR_IF_ERR_EXIT(ctest_flow_logger_restore
                            (unit, lif_severity, dbal_severity, dbal_soc_severity, flow_severity, port_severity,
                             qos_severity));
            if ((rv != _SHR_E_PARAM) && (rv != _SHR_E_UNAVAIL) && (rv != _SHR_E_PORT))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "\nFAILURE on field %s, 0x%x\n", field_desc->field_name, field_enabler);
            }
        }
    }

    /*
     * Special fields 
     */
    while (flow_app_info->special_fields[field_idx] != FLOW_S_F_EMPTY)
    {
        SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_legal_value_get
                        (unit, flow_app_info, 0, flow_app_info->special_fields[field_idx], &special_field_values));

        SHR_IF_ERR_EXIT(ctest_flow_special_field_get
                        (unit, flow_app_info->special_fields[field_idx], &ctest_special_field));
        special_fields.special_fields[0].field_id = flow_app_info->special_fields[field_idx];
        SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, special_fields.special_fields[0].field_id, &curr_field));
        if (CTEST_DNX_FLOW_SKIP_SPECIAL_FIELD_ILLEGAL(curr_field, ctest_special_field))
        {
            field_idx++;
            continue;
        }
        CTEST_DNX_FLOW_SPECIAL_FIELD_SET_ILLEGAL(curr_field, ctest_special_field);

        special_fields.actual_nof_special_fields = 1;
        /**Don't show error messages*/
        SHR_IF_ERR_EXIT(ctest_flow_logger_close
                        (unit, &lif_severity, &dbal_severity, &dbal_soc_severity, &flow_severity, &port_severity,
                         &qos_severity));

        rv = ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, app_data, NULL, 1, 0);

        SHR_IF_ERR_EXIT(ctest_flow_logger_restore
                        (unit, lif_severity, dbal_severity, dbal_soc_severity, flow_severity, port_severity,
                         qos_severity));

        if (rv != _SHR_E_PARAM)
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "\nFAILURE on special field %s, value 0x%x\n"),
                      dnx_flow_special_field_to_string(unit, flow_app_info->special_fields[field_idx]),
                      ctest_special_field->illegal_value));
            if (rv == _SHR_E_NONE)
            {
                /*
                 * Delete the entry
                 */
                ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL);
            }
            SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            SHR_EXIT();
        }
        field_idx++;
    }

exit:
    SHR_FUNC_EXIT;
}

/** for match applications gets the value of a requested special field from the special field struct */
shr_error_e
ctest_flow_special_field_value_get(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    flow_special_fields_e field_id,
    bcm_flow_special_field_t * special_field_data)
{
    int field_idx;
    const flow_special_field_info_t *special_field_info;

    SHR_FUNC_INIT_VARS(unit);

    for (field_idx = 0; field_idx < special_fields->actual_nof_special_fields; field_idx++)
    {
        if (special_fields->special_fields[field_idx].field_id == field_id)
        {
            SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, field_id, &special_field_info));

            /** for type FLOW_PAYLOAD_TYPE_ENABLER there is no value */
            if (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_UINT32_ARR)
            {
                sal_memcpy(&special_field_data->shr_var_uint32_arr,
                           special_fields->special_fields[field_idx].shr_var_uint32_arr,
                           sizeof(special_fields->special_fields[field_idx].shr_var_uint32_arr));
            }
            else if (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_UINT8_ARR)
            {
                sal_memcpy(&special_field_data->shr_var_uint8_arr,
                           special_fields->special_fields[field_idx].shr_var_uint8_arr,
                           sizeof(special_fields->special_fields[field_idx].shr_var_uint8_arr));
            }
            else if (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_UINT32)
            {
                special_field_data->shr_var_uint32 = special_fields->special_fields[field_idx].shr_var_uint32;
            }
            else if ((special_field_info->payload_type == FLOW_PAYLOAD_TYPE_ENUM)
                     || (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_BCM_ENUM))
            {
                special_field_data->symbol = special_fields->special_fields[field_idx].symbol;
            }
            SHR_EXIT();
        }
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_compare_term_info_structs(
    int unit,
    bcm_flow_terminator_info_t * set_info,
    bcm_flow_terminator_info_t * get_info)
{
    int num_common_fields, common_field_idx, field_enabler;
    const dnx_flow_common_field_desc_t *field_desc;
    int set_value, get_value;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, FLOW_APP_TYPE_TERM, &num_common_fields));

    for (common_field_idx = 0; common_field_idx < num_common_fields; common_field_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                        (unit, FLOW_APP_TYPE_TERM, common_field_idx, &field_desc));

        if (field_desc == NULL)
        {
            continue;
        }

        field_enabler = field_desc->term_field_enabler;

        if (_SHR_IS_FLAG_SET(set_info->valid_elements_set, field_enabler))
        {
            SHR_IF_ERR_EXIT(ctest_flow_get_by_enabler(unit, FLOW_APP_TYPE_TERM, field_enabler, set_info, &set_value));
            SHR_IF_ERR_EXIT(ctest_flow_get_by_enabler(unit, FLOW_APP_TYPE_TERM, field_enabler, set_info, &get_value));
            if (set_value != get_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Comparison failed on %s. Set value: 0x%x, get_value: 0x%x",
                             field_desc->field_name, set_value, get_value);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_compare_init_info_structs(
    int unit,
    bcm_flow_initiator_info_t * set_info,
    bcm_flow_initiator_info_t * get_info)
{
    int num_common_fields, common_field_idx, field_enabler;
    const dnx_flow_common_field_desc_t *field_desc;
    int set_value, get_value;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, FLOW_APP_TYPE_INIT, &num_common_fields));

    for (common_field_idx = 0; common_field_idx < num_common_fields; common_field_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                        (unit, FLOW_APP_TYPE_INIT, common_field_idx, &field_desc));

        if (field_desc == NULL)
        {
            continue;
        }

        field_enabler = field_desc->init_field_enabler;

        if (_SHR_IS_FLAG_SET(set_info->valid_elements_set, field_enabler))
        {
            SHR_IF_ERR_EXIT(ctest_flow_get_by_enabler(unit, FLOW_APP_TYPE_INIT, field_enabler, set_info, &set_value));
            SHR_IF_ERR_EXIT(ctest_flow_get_by_enabler(unit, FLOW_APP_TYPE_INIT, field_enabler, set_info, &get_value));
            if (set_value != get_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Comparison failed on %s. Set value: 0x%x, get_value: 0x%x",
                             field_desc->field_name, set_value, get_value);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Run common fields unit test.
 * Set legal value per every field, expect success
 *
 * \return
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
ctest_flow_common_fields_positive_test(
    int unit,
    bcm_flow_handle_t flow_handle_id,
    const dnx_flow_app_config_t * flow_app_info)
{
    shr_error_e rv;
    int num_common_fields = 0, get_value;
    uint32 legal_value = 0;
    const dnx_flow_common_field_desc_t *field_desc;
    int common_field_idx, field_idx = 0;
    uint32 field_enabler;
    void *app_data;
    bcm_flow_terminator_info_t terminator_info = { 0 };
    bcm_flow_initiator_info_t initiator_info = { 0 };
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 }, *ptr_special_fields;
    ctest_flow_common_field_desc_t *ctest_field_desc;
    const flow_special_field_info_t *curr_field;

    SHR_FUNC_INIT_VARS(unit);

    flow_handle_info.flow_handle = flow_handle_id;

    app_data =
        (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) ? (void *) &terminator_info : (void *) &initiator_info;

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_info->flow_app_type, &num_common_fields));

    for (common_field_idx = 0; common_field_idx < num_common_fields; common_field_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                        (unit, flow_app_info->flow_app_type, common_field_idx, &field_desc));

        if (field_desc == NULL)
        {
            continue;
        }

        field_enabler =
            (flow_app_info->flow_app_type ==
             FLOW_APP_TYPE_TERM) ? field_desc->term_field_enabler : field_desc->init_field_enabler;
        if (_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field_enabler))
        {
            SHR_IF_ERR_EXIT(ctest_flow_common_field_get
                            (unit, field_enabler, flow_app_info->flow_app_type, &ctest_field_desc));
            legal_value = ctest_field_desc->legal_value;
            if (legal_value == CTEST_FLOW_SKIP_FIELD)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                            (unit, flow_app_info->flow_app_type, field_enabler, app_data, legal_value));
            rv = ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, app_data, NULL, field_enabler, 1);

            if (rv != _SHR_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "\nFAILURE on field %s, 0x%x\n", field_desc->field_name, field_enabler);
            }

            /*
             * Get the entry 
             */
            ptr_special_fields = (flow_app_info->special_fields[0] != FLOW_S_F_EMPTY) ? &special_fields : NULL;
            if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
            {
                SHR_IF_ERR_EXIT(bcm_flow_terminator_info_get
                                (unit, &flow_handle_info, (bcm_flow_terminator_info_t *) app_data, ptr_special_fields));
            }
            else        /* FLOW_APP_TYPE_INIT */
            {
                SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get
                                (unit, &flow_handle_info, (bcm_flow_initiator_info_t *) app_data, ptr_special_fields));
            }

            SHR_IF_ERR_EXIT(ctest_flow_get_by_enabler
                            (unit, flow_app_info->flow_app_type, field_enabler, app_data, &get_value));

            /*
             * Delete the entry 
             */
            SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));

            if (legal_value != get_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failed on field %s, set 0x%x, got 0x%x", field_desc->field_name,
                             legal_value, get_value);
            }
        }
    }

    /*
     * Special fields 
     */
    
    while ((flow_app_info->special_fields[field_idx] != FLOW_S_F_EMPTY) &&
           (flow_app_info->special_fields[field_idx] == FLOW_S_F_QOS_DSCP))
    {
        bcm_flow_special_field_t value, set_value;

        SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_legal_value_get
                        (unit, flow_app_info, 0, flow_app_info->special_fields[field_idx], &set_value));

        special_fields.special_fields[0].field_id = flow_app_info->special_fields[field_idx];
        SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, special_fields.special_fields[0].field_id, &curr_field));
        if (set_value.shr_var_uint32 == CTEST_FLOW_SKIP_FIELD)
        {
            field_idx++;
            continue;
        }

        CTEST_DNX_FLOW_SPECIAL_FIELD_SET(curr_field, set_value, 0);

        special_fields.actual_nof_special_fields = 1;

        rv = ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, app_data, &special_fields, 0, 1);

        if (rv != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "\nFAILURE on special field %s, value 0x%x\n",
                         dnx_flow_special_field_to_string(unit, flow_app_info->special_fields[field_idx]),
                         value.shr_var_uint32);
        }

        /*
         * Get the entry
         */
        if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
        {
            SHR_IF_ERR_EXIT(bcm_flow_terminator_info_get
                            (unit, &flow_handle_info, (bcm_flow_terminator_info_t *) app_data, &special_fields));
        }
        else    /* FLOW_APP_TYPE_INIT */
        {
            SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get
                            (unit, &flow_handle_info, (bcm_flow_initiator_info_t *) app_data, &special_fields));
        }
        rv = ctest_flow_special_field_value_get(unit, &special_fields, flow_app_info->special_fields[field_idx],
                                                &value);

        if (rv != _SHR_E_NONE)
        {
            if (rv == _SHR_E_NOT_FOUND)
            {
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "\nCouldn't retrieve special field %s\n"),
                          dnx_flow_special_field_to_string(unit, flow_app_info->special_fields[field_idx])));
            }
            else if (rv != _SHR_E_NONE)
            {
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "\nInternal error for field %s\n"),
                          dnx_flow_special_field_to_string(unit, flow_app_info->special_fields[field_idx])));
            }

            /*
             * Delete the entry
             */
            SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));

            SHR_SET_CURRENT_ERR(rv);
            SHR_EXIT();
        }
        else    /* Validate retrieved value */
        {
            const flow_special_field_info_t *special_field_info;

            SHR_IF_ERR_EXIT(flow_special_field_info_get
                            (unit, flow_app_info->special_fields[field_idx], &special_field_info));

            rv = ctest_flow_special_field_validate_value(unit, special_field_info, &value, &set_value);

            /*
             * Delete the entry
             */
            SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));

            if (rv != _SHR_E_NONE)
            {
                SHR_SET_CURRENT_ERR(rv);
                SHR_EXIT();
            }
        }

        field_idx++;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Run common fields unit test.
 *
 * \return
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
ctest_flow_fields_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 negative, positive;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    const dnx_flow_app_config_t *flow_app_info = NULL;
    int rv_negative = _SHR_E_NONE, rv_positive = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_WITH_EXCEPTION(ctest_flow_test_begin
                                   (unit, args, sand_control, FLOW_ALLOWED_APP_TYPES_ALL, 0, &flow_app_info,
                                    &flow_handle_info), _SHR_E_PARAM);

    SH_SAND_GET_BOOL("negative", negative);
    SH_SAND_GET_BOOL("positive", positive);

    if (negative)
    {
        rv_negative = ctest_flow_common_fields_negative_test(unit, flow_handle_info.flow_handle, flow_app_info);
        if (rv_negative == _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("Negative test application %s PASSED\n"), flow_app_info->app_name));
        }
        else
        {
            LOG_CLI((BSL_META("Negative test application %s FAILED\n"), flow_app_info->app_name));
            SHR_SET_CURRENT_ERR(rv_negative);
        }
    }

    if (positive)
    {
        rv_positive = ctest_flow_common_fields_positive_test(unit, flow_handle_info.flow_handle, flow_app_info);
        if (rv_positive == _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("Positive test application %s PASSED\n"), flow_app_info->app_name));
        }
        else
        {
            LOG_CLI((BSL_META("Positive test application %s FAILED\n"), flow_app_info->app_name));
            SHR_SET_CURRENT_ERR(rv_positive);
        }
    }

    if ((rv_negative == _SHR_E_NONE) && (rv_positive == _SHR_E_NONE))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Run basic test.
 * Validate set, get, replace, delete, traverse APIs
 *
 * \return
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
ctest_flow_basic_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    const dnx_flow_app_config_t *flow_app_info = NULL;
    int rv;
    int num_common_fields = 0;
    uint32 legal_value = 0;
    const dnx_flow_common_field_desc_t *field_desc;
    int common_field_idx;
    uint32 field_enabler, field_enablers;
    void *app_data;
    bcm_flow_terminator_info_t terminator_info = { 0 };
    bcm_flow_terminator_info_t get_terminator_info = { 0 };
    bcm_flow_initiator_info_t initiator_info = { 0 };
    bcm_flow_initiator_info_t get_initiator_info = { 0 };
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 }, *ptr_special_fields;
    ctest_flow_common_field_desc_t *ctest_field_desc;
    int num_app_common_fields = 0, count = 0;
    bsl_severity_t lif_severity = bslSeverityOff, dbal_severity = bslSeverityOff, flow_severity =
        bslSeverityOff, dbal_soc_severity = bslSeverityOff, port_severity = bslSeverityOff, qos_severity =
        bslSeverityOff;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_WITH_EXCEPTION(ctest_flow_test_begin
                                   (unit, args, sand_control, FLOW_ALLOWED_APP_TYPES_ALL, 0, &flow_app_info,
                                    &flow_handle_info), _SHR_E_PARAM);

    app_data =
        (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) ? (void *) &terminator_info : (void *) &initiator_info;

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_info->flow_app_type, &num_common_fields));

    /*
     * Get number of supported common fields in application 
     */
    shr_bitop_range_count(&flow_app_info->valid_common_fields_bitmap, 0, num_common_fields, &num_app_common_fields);

    /*
     * First - add all but the last common field 
     */
    for (common_field_idx = 0; common_field_idx < num_common_fields; common_field_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                        (unit, flow_app_info->flow_app_type, common_field_idx, &field_desc));

        if (field_desc == NULL)
        {
            continue;
        }

        field_enabler =
            (flow_app_info->flow_app_type ==
             FLOW_APP_TYPE_TERM) ? field_desc->term_field_enabler : field_desc->init_field_enabler;
        if (_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field_enabler))
        {
            count++;
            flow_handle_info.flags = 0;
            SHR_IF_ERR_EXIT(ctest_flow_common_field_get
                            (unit, field_enabler, flow_app_info->flow_app_type, &ctest_field_desc));
            if (count == num_app_common_fields)
            {
                break;
            }
            legal_value = ctest_field_desc->legal_value;
            if (legal_value == CTEST_FLOW_SKIP_FIELD)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                            (unit, flow_app_info->flow_app_type, field_enabler, app_data, legal_value));

            if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
            {
                ((bcm_flow_terminator_info_t *) app_data)->valid_elements_set |= field_enabler;
            }
            else        /* FLOW_APP_TYPE_INIT */
            {
                /*
                 * Encap access is a mandatory field for some initiator entries
                 */
                ((bcm_flow_initiator_info_t *) app_data)->valid_elements_set |= field_enabler;
            }
        }
    }

    field_enablers =
        (flow_app_info->flow_app_type ==
         FLOW_APP_TYPE_TERM) ? ((bcm_flow_terminator_info_t *) app_data)->valid_elements_set
        : ((bcm_flow_initiator_info_t *) app_data)->valid_elements_set;
    rv = ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, app_data, NULL, field_enablers, 1);

    if (rv != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\nFAILURE on field set(), application: %s\n", flow_app_info->app_name);
    }

    /*
     * Try to replace the entry with illegal combination - clear bitmap is the same as the set bitmap
     */
    /**Don't show error messages*/
    SHR_IF_ERR_EXIT(ctest_flow_logger_close
                    (unit, &lif_severity, &dbal_severity, &dbal_soc_severity, &flow_severity, &port_severity,
                     &qos_severity));
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        ((bcm_flow_terminator_info_t *) app_data)->valid_elements_clear =
            ((bcm_flow_terminator_info_t *) app_data)->valid_elements_set;
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        ((bcm_flow_initiator_info_t *) app_data)->valid_elements_clear =
            ((bcm_flow_initiator_info_t *) app_data)->valid_elements_set;
        ((bcm_flow_initiator_info_t *) app_data)->encap_access = bcmEncapAccessInvalid;
    }

    rv = ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, app_data, NULL, field_enablers, 0);
    SHR_IF_ERR_EXIT(ctest_flow_logger_restore
                    (unit, lif_severity, dbal_severity, dbal_soc_severity, flow_severity, port_severity, qos_severity));

    flow_handle_info.flags = 0;

    if (rv != _SHR_E_PARAM)
    {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit,
                   "\nSucceeded replacing an invalid entry, expected failure with _SHR_E_PARAM, application: %s\n"),
                  flow_app_info->app_name));

        SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
        SHR_EXIT();
    }

    /*
     * Now - try to replace the entry with all the relevant fields, clear some of them. expect success
     */
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        ((bcm_flow_terminator_info_t *) app_data)->valid_elements_clear = 0;
        ((bcm_flow_terminator_info_t *) app_data)->valid_elements_set = 0;
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        ((bcm_flow_initiator_info_t *) app_data)->valid_elements_clear = 0;
        ((bcm_flow_initiator_info_t *) app_data)->valid_elements_set = 0;
    }

    for (common_field_idx = 0; common_field_idx < num_common_fields; common_field_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                        (unit, flow_app_info->flow_app_type, common_field_idx, &field_desc));

        if (field_desc == NULL)
        {
            continue;
        }

        field_enabler =
            (flow_app_info->flow_app_type ==
             FLOW_APP_TYPE_TERM) ? field_desc->term_field_enabler : field_desc->init_field_enabler;
        if (_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field_enabler))
        {
            SHR_IF_ERR_EXIT(ctest_flow_common_field_get
                            (unit, field_enabler, flow_app_info->flow_app_type, &ctest_field_desc));
            legal_value = ctest_field_desc->legal_value;
            if ((legal_value == CTEST_FLOW_SKIP_FIELD) || (common_field_idx % 2))
            {
                if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
                {
                    ((bcm_flow_terminator_info_t *) app_data)->valid_elements_clear |= field_enabler;
                    continue;
                }
                else    /* FLOW_APP_TYPE_INIT */
                {
                    ((bcm_flow_initiator_info_t *) app_data)->valid_elements_clear |= field_enabler;
                    continue;
                }
            }
            SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                            (unit, flow_app_info->flow_app_type, field_enabler, app_data, legal_value));

            if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
            {
                ((bcm_flow_terminator_info_t *) app_data)->valid_elements_set |= field_enabler;
            }
            else        /* FLOW_APP_TYPE_INIT */
            {
                ((bcm_flow_initiator_info_t *) app_data)->valid_elements_set |= field_enabler;
            }
        }
    }

    field_enablers =
        (flow_app_info->flow_app_type ==
         FLOW_APP_TYPE_TERM) ? ((bcm_flow_terminator_info_t *) app_data)->valid_elements_set
        : ((bcm_flow_initiator_info_t *) app_data)->valid_elements_set;
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    rv = ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, app_data, NULL, field_enablers, 0);
    flow_handle_info.flags = 0;
    if (rv != _SHR_E_NONE)
    {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "\nFailed replacing a valid entry, application: %s\n"), flow_app_info->app_name));

        /*
         * Delete the entry
         */
        SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }
    /*
     * Get the entry 
     */
    ptr_special_fields = (flow_app_info->special_fields[0] != FLOW_S_F_EMPTY) ? &special_fields : NULL;
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        rv = (bcm_flow_terminator_info_get(unit, &flow_handle_info, &get_terminator_info, ptr_special_fields));
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        rv = (bcm_flow_initiator_info_get(unit, &flow_handle_info, &get_initiator_info, ptr_special_fields));
    }
    if (rv != _SHR_E_NONE)
    {
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\nGet failure, application: %s\n"), flow_app_info->app_name));

        SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }
    /*
     * Delete the entry
     */
    SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));

    /*
     * Compare the set and get structures 
     */
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(ctest_flow_compare_term_info_structs
                        (unit, (bcm_flow_terminator_info_t *) app_data, &get_terminator_info));
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        SHR_IF_ERR_EXIT(ctest_flow_compare_init_info_structs
                        (unit, (bcm_flow_initiator_info_t *) app_data, &get_initiator_info));
    }

    /*
     * Now - add as many entries as there're supported common fields
     */
    count = 0;
    for (common_field_idx = 0; common_field_idx < num_common_fields; common_field_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                        (unit, flow_app_info->flow_app_type, common_field_idx, &field_desc));

        if (field_desc == NULL)
        {
            continue;
        }

        field_enabler =
            (flow_app_info->flow_app_type ==
             FLOW_APP_TYPE_TERM) ? field_desc->term_field_enabler : field_desc->init_field_enabler;
        if (_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field_enabler))
        {
            SHR_IF_ERR_EXIT(ctest_flow_common_field_get
                            (unit, field_enabler, flow_app_info->flow_app_type, &ctest_field_desc));
            legal_value = ctest_field_desc->legal_value;
            if (legal_value == CTEST_FLOW_SKIP_FIELD)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                            (unit, flow_app_info->flow_app_type, field_enabler, app_data, legal_value));

            rv = ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, app_data, NULL, field_enabler, 1);
            if (rv != _SHR_E_NONE)
            {
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "\nFailure on field %s, 0x%x, application: %s\n"), field_desc->field_name,
                          field_enabler, flow_app_info->app_name));
                /*
                 * Delete the entries
                 */
                SHR_IF_ERR_EXIT(ctest_flow_delete_entries(unit, flow_app_info->flow_app_type, &flow_handle_info));
                SHR_SET_CURRENT_ERR(rv);
                SHR_EXIT();
            }
            basic_test_flow_ids[count++] = flow_handle_info.flow_id;
        }
    }

    /*
     * Traverse the entries with delete cb 
     */
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        rv = bcm_flow_terminator_info_traverse(unit, &flow_handle_info, ctest_flow_term_delete_callback,
                                               &get_terminator_info);
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        rv = bcm_flow_initiator_info_traverse(unit, &flow_handle_info, ctest_flow_init_delete_callback,
                                              &get_initiator_info);
    }

    /*
     * Make sure that none of the entries remained 
     */
    count = 0;
    /**Don't show error messages*/
    SHR_IF_ERR_EXIT(ctest_flow_logger_close
                    (unit, &lif_severity, &dbal_severity, &dbal_soc_severity, &flow_severity, &port_severity,
                     &qos_severity));

    while (basic_test_flow_ids[count] != 0)
    {
        flow_handle_info.flow_id = basic_test_flow_ids[count++];

        /*
         * Get the entry
         */
        if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
        {
            rv = (bcm_flow_terminator_info_get
                  (unit, &flow_handle_info, (bcm_flow_terminator_info_t *) app_data, &special_fields));
        }
        else    /* FLOW_APP_TYPE_INIT */
        {
            rv = (bcm_flow_initiator_info_get
                  (unit, &flow_handle_info, (bcm_flow_initiator_info_t *) app_data, &special_fields));
        }
        if (rv != _SHR_E_NOT_FOUND)
        {
            LOG_CLI((BSL_META("Found an entry which was supposed to be deleted\n")));
            /*
             * Delete the entries
             */
            SHR_IF_ERR_EXIT(ctest_flow_delete_entries(unit, flow_app_info->flow_app_type, &flow_handle_info));
            SHR_ERR_EXIT(_SHR_E_PARAM, "TEST FAILED, application: %s", flow_app_info->app_name);
        }
    }
    SHR_IF_ERR_EXIT(ctest_flow_logger_restore
                    (unit, lif_severity, dbal_severity, dbal_soc_severity, flow_severity, port_severity, qos_severity));

    LOG_CLI((BSL_META("Basic test application %s PASSED\n"), flow_app_info->app_name));

exit:
    rv = ctest_flow_logger_restore
        (unit, lif_severity, dbal_severity, dbal_soc_severity, flow_severity, port_severity, qos_severity);
    if (rv != _SHR_E_NONE)
    {
        LOG_CLI((BSL_META("Failed to restore logger\n")));
        if (SHR_FUNC_VAL_IS(_SHR_E_NONE))
        {
            SHR_SET_CURRENT_ERR(rv);
        }
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Run negative test.
 * Validate negative scenarios: Illegal flow handle, unsupported application field, wrong number of key fields for match, illegal special field
 *
 * \return
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
ctest_flow_negative_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_flow_handle_t flow_handle_id = -1;
    const dnx_flow_app_config_t *flow_app_info = NULL;
    int rv;
    int num_common_fields = 0;
    uint32 legal_value = 0;
    const dnx_flow_common_field_desc_t *field_desc;
    int common_field_idx, special_field_idx = 0;
    uint32 field_enabler = 0;
    void *app_data;
    bcm_flow_terminator_info_t terminator_info = { 0 };
    bcm_flow_initiator_info_t initiator_info = { 0 };
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };
    ctest_flow_special_field_desc_t *ctest_special_field;
    ctest_flow_common_field_desc_t *ctest_field_desc;
    bsl_severity_t lif_severity = bslSeverityOff, dbal_severity = bslSeverityOff, flow_severity =
        bslSeverityOff, dbal_soc_severity = bslSeverityOff, port_severity = bslSeverityOff, qos_severity =
        bslSeverityOff;
    const flow_special_field_info_t *curr_field;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_WITH_EXCEPTION(ctest_flow_test_begin
                                   (unit, args, sand_control, FLOW_ALLOWED_APP_TYPES_ALL, 0, &flow_app_info,
                                    &flow_handle_info), _SHR_E_PARAM);

    flow_handle_id = flow_handle_info.flow_handle;

    app_data =
        (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) ? (void *) &terminator_info : (void *) &initiator_info;

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_info->flow_app_type, &num_common_fields));

    /*
     * First - add all common fields
     */
    for (common_field_idx = 0; common_field_idx < num_common_fields; common_field_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                        (unit, flow_app_info->flow_app_type, common_field_idx, &field_desc));

        if (field_desc == NULL)
        {
            continue;
        }

        field_enabler =
            (flow_app_info->flow_app_type ==
             FLOW_APP_TYPE_TERM) ? field_desc->term_field_enabler : field_desc->init_field_enabler;
        if (_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field_enabler))
        {
            SHR_IF_ERR_EXIT(ctest_flow_common_field_get
                            (unit, field_enabler, flow_app_info->flow_app_type, &ctest_field_desc));
            legal_value = ctest_field_desc->legal_value;
            if (legal_value == CTEST_FLOW_SKIP_FIELD)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                            (unit, flow_app_info->flow_app_type, field_enabler, app_data, legal_value));

            if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
            {
                ((bcm_flow_terminator_info_t *) app_data)->valid_elements_set |= field_enabler;
            }
            else        /* FLOW_APP_TYPE_INIT */
            {
                ((bcm_flow_initiator_info_t *) app_data)->valid_elements_set |= field_enabler;
            }
        }
    }

    /**Don't show error messages*/
    SHR_IF_ERR_EXIT(ctest_flow_logger_close
                    (unit, &lif_severity, &dbal_severity, &dbal_soc_severity, &flow_severity, &port_severity,
                     &qos_severity));

    /*
     * Invalid flow handle - 0
     */
    flow_handle_info.flow_handle = 0;

    rv = ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, app_data, NULL, field_enabler, 1);
    flow_handle_info.flow_handle = flow_handle_id;
    if (rv != _SHR_E_PARAM)
    {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "\nFailure on invalid flow handle, application: %s\n"), flow_app_info->app_name));

        if (rv == _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));
        }
        SHR_ERR_EXIT(_SHR_E_PARAM, "\nTest TAILED\n");
    }

    SHR_IF_ERR_EXIT(ctest_flow_logger_restore
                    (unit, lif_severity, dbal_severity, dbal_soc_severity, flow_severity, port_severity, qos_severity));

    /*
     * Invalid flags
     */
    for (common_field_idx = 0; common_field_idx < num_common_fields; common_field_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                        (unit, flow_app_info->flow_app_type, common_field_idx, &field_desc));

        if ((field_desc == NULL) || ((flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT) && (common_field_idx == 4)))
        {
            continue;
        }

        field_enabler =
            (flow_app_info->flow_app_type ==
             FLOW_APP_TYPE_TERM) ? field_desc->term_field_enabler : field_desc->init_field_enabler;
        if ((!_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field_enabler))
            && (field_desc->field_cbs.field_set_cb))
        {
            SHR_IF_ERR_EXIT(ctest_flow_common_field_get
                            (unit, field_enabler, flow_app_info->flow_app_type, &ctest_field_desc));
            legal_value = ctest_field_desc->legal_value;
            if (legal_value == CTEST_FLOW_SKIP_FIELD)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                            (unit, flow_app_info->flow_app_type, field_enabler, app_data, legal_value));

            if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
            {
                ((bcm_flow_terminator_info_t *) app_data)->valid_elements_set |= field_enabler;
            }
            else        /* FLOW_APP_TYPE_INIT */
            {
                ((bcm_flow_initiator_info_t *) app_data)->valid_elements_set |= field_enabler;
            }
            break;
        }
    }

    /**Don't show error messages*/
    SHR_IF_ERR_EXIT(ctest_flow_logger_close
                    (unit, &lif_severity, &dbal_severity, &dbal_soc_severity, &flow_severity, &port_severity,
                     &qos_severity));

    /*
     * Common field which is not supported by the application was found
     */
    if (common_field_idx < num_common_fields)
    {
        rv = ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, app_data, NULL, field_enabler, 1);

        if (rv != _SHR_E_PARAM)
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "\nFAILURE on invalid application common field enabler, application: %s\n"),
                      flow_app_info->app_name));

            if (rv == _SHR_E_NONE)
            {
                SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));
            }
            SHR_ERR_EXIT(_SHR_E_PARAM, "\nTest TAILED\n");
        }
    }

    SHR_IF_ERR_EXIT(ctest_flow_logger_restore
                    (unit, lif_severity, dbal_severity, dbal_soc_severity, flow_severity, port_severity, qos_severity));

    /*
     * Invalid number of match key fields 
     */
    if (FLOW_APP_IS_MATCH(flow_app_info->flow_app_type))
    {
        int special_fields_count = 0;
        special_fields.actual_nof_special_fields = 0;
        while (flow_app_info->special_fields[special_field_idx] != FLOW_S_F_EMPTY)
        {
            SHR_IF_ERR_EXIT(ctest_flow_special_field_get
                            (unit, flow_app_info->special_fields[special_field_idx], &ctest_special_field));
            special_fields.special_fields[special_fields_count].field_id =
                flow_app_info->special_fields[special_field_idx];
            SHR_IF_ERR_EXIT(flow_special_field_info_get
                            (unit, special_fields.special_fields[special_fields_count].field_id, &curr_field));
            if (curr_field->payload_type == FLOW_PAYLOAD_TYPE_ENABLER)
            {
                /** special fields of type enabler have no negative value*/
                continue;
            }
            if (curr_field->payload_type == FLOW_PAYLOAD_TYPE_UINT32)
            {
                sal_memcpy(special_fields.special_fields[special_fields_count].shr_var_uint32_arr,
                           ctest_special_field->arr32_illegal_value, sizeof(ctest_special_field->illegal_value));
            }
            else
            {
                special_fields.special_fields[special_fields_count].shr_var_uint32 = ctest_special_field->illegal_value;
            }
            special_field_idx++;
        }
        special_fields.actual_nof_special_fields = special_fields_count++;

        /**Don't show error messages*/
        SHR_IF_ERR_EXIT(ctest_flow_logger_close
                        (unit, &lif_severity, &dbal_severity, &dbal_soc_severity, &flow_severity, &port_severity,
                         &qos_severity));

        rv = bcm_dnx_flow_match_info_add(unit, &flow_handle_info, &special_fields);

        if (rv != _SHR_E_PARAM)
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "\nFAILURE on invalid number of special fields, application: %s\n"),
                      flow_app_info->app_name));

            if (rv == _SHR_E_NONE)
            {
                SHR_IF_ERR_EXIT(ctest_flow_entry_delete
                                (unit, flow_app_info->flow_app_type, &flow_handle_info, &special_fields));
            }
            SHR_ERR_EXIT(_SHR_E_PARAM, "\nTest TAILED\n");
        }

        /*
         * Non existing key special field
         */
        special_fields.special_fields[0].field_id = DBAL_FIELD_EMPTY;
        rv = bcm_dnx_flow_match_info_add(unit, &flow_handle_info, &special_fields);

        if (rv != _SHR_E_PARAM)
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "\nFAILURE on invalid key special field, application: %s\n"),
                      flow_app_info->app_name));

            if (rv == _SHR_E_NONE)
            {
                SHR_IF_ERR_EXIT(ctest_flow_entry_delete
                                (unit, flow_app_info->flow_app_type, &flow_handle_info, &special_fields));
            }
            SHR_ERR_EXIT(_SHR_E_PARAM, "\nTest TAILED\n");
        }
    }

    SHR_IF_ERR_EXIT(ctest_flow_logger_restore
                    (unit, lif_severity, dbal_severity, dbal_soc_severity, flow_severity, port_severity, qos_severity));

    /*
     * Invalid special field 
     */
    if (flow_app_info->special_fields[0] != FLOW_S_F_EMPTY)
    {
        SHR_IF_ERR_EXIT(ctest_flow_special_field_get(unit, flow_app_info->special_fields[0], &ctest_special_field));
        special_fields.special_fields[0].field_id = DBAL_NOF_FIELDS;
        special_fields.special_fields[0].shr_var_uint32 = ctest_special_field->illegal_value;

        /**Don't show error messages*/
        SHR_IF_ERR_EXIT(ctest_flow_logger_close
                        (unit, &lif_severity, &dbal_severity, &dbal_soc_severity, &flow_severity, &port_severity,
                         &qos_severity));

        if ((flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
            || (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT))
        {
            special_fields.actual_nof_special_fields = 1;
            rv = ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, app_data, &special_fields,
                                            field_enabler, 1);
        }
        else
        {
            rv = bcm_dnx_flow_match_info_add(unit, &flow_handle_info, &special_fields);
        }
        if (rv != _SHR_E_PARAM)
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "\nFAILURE on invalid special field, application: %s\n"),
                      flow_app_info->app_name));

            if (rv == _SHR_E_NONE)
            {
                SHR_IF_ERR_EXIT(ctest_flow_entry_delete
                                (unit, flow_app_info->flow_app_type, &flow_handle_info, &special_fields));
            }
            SHR_ERR_EXIT(_SHR_E_PARAM, "\nTest TAILED\n");
        }
    }

    LOG_CLI((BSL_META("Negative test application %s PASSED\n"), flow_app_info->app_name));

exit:
    rv = ctest_flow_logger_restore
        (unit, lif_severity, dbal_severity, dbal_soc_severity, flow_severity, port_severity, qos_severity);
    if (rv != _SHR_E_NONE)
    {
        LOG_CLI((BSL_META("Failed to restore logger\n")));
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_special_fields_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    const flow_special_field_info_t *curr_field;
    const flow_special_field_info_t *linked_field;
    int idx;

    SHR_FUNC_INIT_VARS(unit);

    for (idx = 0; idx < NOF_FLOW_SPECIAL_FIELDS; idx++)
    {
        SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, idx, &curr_field));

        /** Mask field validates */
        if (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_MASK_SUPPORTED))
        {
            if (curr_field->linked_special_field == FLOW_S_F_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field %s with Mask option doesn't have related mask field\n",
                             curr_field->name);
            }
            else
            {
                flow_special_field_info_get(unit, curr_field->linked_special_field, &linked_field);
                if (!dnx_flow_special_field_is_ind_set(unit, linked_field, FLOW_SPECIAL_FIELD_IND_IS_MASK))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field %s used field %s as mask which it is not\n", curr_field->name,
                                 linked_field->name);
                }
            }
            if ((curr_field->payload_type == FLOW_PAYLOAD_TYPE_BCM_ENUM) ||
                (curr_field->payload_type == FLOW_PAYLOAD_TYPE_ENUM))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Ilegal payload type (%s) for mask field %s \n",
                             dnx_flow_payload_types_to_string(unit, curr_field->payload_type), curr_field->name);
            }
        }

        if ((curr_field->payload_type == FLOW_PAYLOAD_TYPE_BCM_ENUM)
            || (curr_field->payload_type == FLOW_PAYLOAD_TYPE_ENUM))
        {
            if (curr_field->description == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Description callback is missing for field (%s) \n", curr_field->name);
            }

            if (curr_field->print == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Print callback is missing for field (%s) \n", curr_field->name);
            }

        }
    }

exit:
    if (SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        LOG_CLI((BSL_META("Special fields definition test PASSED\n")));
    }
    else
    {
        LOG_CLI((BSL_META("Special fields definition test FAILED\n")));
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_common_profiles_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    const dnx_flow_app_config_t *flow_app_info = NULL;
    void *app_data;
    bcm_flow_terminator_info_t terminator_info = { 0 };
    bcm_flow_initiator_info_t initiator_info = { 0 };
    bcm_flow_handle_info_t flow_handle_info = { 0 }, flow_handle_info_2 =
    {
    0};
    const dnx_flow_common_field_desc_t *field_desc;
    ctest_flow_common_field_desc_t *ctest_field_desc;
    uint32 field_enabler;
    int field_index, legal_value, common_field_idx, num_common_fields, rv, in_lif_first_ref_count, in_lif_ref_count,
        qos_profile_first_ref_count, qos_profile_ref_count, action_gport_ref_count, action_gport_first_ref_count;
    lif_table_mngr_inlif_info_t inlif_info;
    uint32 entry_handle_id, in_lif_profile, qos_profile, action_gport;
    uint32 term_profile_enablers[] =
        { BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID, BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID,
        BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID
    };
    uint32 init_profile_enablers[] = { BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID };
    uint32 *profile_enablers;
    qos_propagation_profile_template_t propagation_profile_type;
    bcm_gport_t profile_data;
    uint32 lif_flags = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources = { 0 };
    int profile, num_profiles, is_local_lif = TRUE;
    lif_table_mngr_outlif_info_t outlif_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_WITH_EXCEPTION(ctest_flow_test_begin
                                   (unit, args, sand_control, FLOW_ALLOWED_APP_TYPES_LIF, 0, &flow_app_info,
                                    &flow_handle_info), _SHR_E_PARAM);

    flow_handle_info_2.flow_handle = flow_handle_info.flow_handle;

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));
    }

    app_data =
        (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) ? (void *) &terminator_info : (void *) &initiator_info;

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_info->flow_app_type, &num_common_fields));

    profile_enablers =
        (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) ? term_profile_enablers : init_profile_enablers;
    num_profiles =
        (flow_app_info->flow_app_type ==
         FLOW_APP_TYPE_TERM) ? (sizeof(term_profile_enablers) / sizeof(uint32)) : (sizeof(init_profile_enablers) /
                                                                                   sizeof(uint32));

    for (profile = 0; profile < num_profiles; profile++)
    {
        field_enabler = profile_enablers[profile];
        if (_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field_enabler))
        {
            SHR_IF_ERR_EXIT(dnx_flow_common_fields_index_by_enabler_get
                            (unit, flow_app_info->flow_app_type, field_enabler, &field_index));
            SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                            (unit, flow_app_info->flow_app_type, field_index, &field_desc));

            if (field_desc == NULL)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(ctest_flow_common_field_get
                            (unit, field_enabler, flow_app_info->flow_app_type, &ctest_field_desc));
            legal_value = ctest_field_desc->legal_value;
            SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                            (unit, flow_app_info->flow_app_type, field_enabler, app_data, legal_value));

            if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
            {
                ((bcm_flow_terminator_info_t *) app_data)->valid_elements_set |= field_enabler;
            }
            else if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
            {
                ((bcm_flow_initiator_info_t *) app_data)->valid_elements_set |= field_enabler;
            }
        }
    }

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_create
                        (unit, &flow_handle_info, (bcm_flow_terminator_info_t *) app_data, NULL));

        lif_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, flow_handle_info.flow_id, lif_flags, &gport_hw_resources));

        /** Take handle to read from DBAL table */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                        (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, entry_handle_id, &inlif_info));

        /*
         * get inlif profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, &in_lif_profile));
        /*
         * get qos profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROFILE, INST_SINGLE, &qos_profile));
        /*
         * get action_gport
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE, &action_gport));

        /*
         * Get l3_intf_id reference count 
         */
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_ref_count(unit, in_lif_profile, &in_lif_first_ref_count, LIF));
        /*
         * Get qos_profile reference count 
         */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_ingress_propagation_profile.profile_data_get
                        (unit, qos_profile, &qos_profile_first_ref_count, &propagation_profile_type));
        /*
         * Get action_gport reference count 
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.profile_data_get
                        (unit, action_gport, &action_gport_first_ref_count, &profile_data));

        /*
         * Create another entry with the same profiles 
         */
        /*
         * First - find the first supported non-profile common field to set 
         */
        for (common_field_idx = 0; common_field_idx < num_common_fields; common_field_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                            (unit, flow_app_info->flow_app_type, common_field_idx, &field_desc));

            if (field_desc == NULL)
            {
                continue;
            }

            field_enabler = field_desc->term_field_enabler;
            if ((field_desc->is_profile == TRUE)
                || (!_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field_enabler)))
            {
                continue;
            }

            SHR_IF_ERR_EXIT(ctest_flow_common_field_get
                            (unit, field_enabler, flow_app_info->flow_app_type, &ctest_field_desc));
            legal_value = ctest_field_desc->legal_value;
            if (legal_value == CTEST_FLOW_SKIP_FIELD)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                            (unit, flow_app_info->flow_app_type, field_enabler, app_data, legal_value));

            ((bcm_flow_terminator_info_t *) app_data)->valid_elements_set |= field_enabler;
            flow_handle_info_2.flow_handle = flow_handle_info.flow_handle;
            SHR_IF_ERR_EXIT(bcm_flow_terminator_info_create
                            (unit, &flow_handle_info_2, (bcm_flow_terminator_info_t *) app_data, NULL));
            break;
        }
        /*
         * Second entry created 
         */
        if (common_field_idx < num_common_fields)
        {
            /*
             * Validate l3_intf_id reference count 
             */
            SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_ref_count(unit, in_lif_profile, &in_lif_ref_count, LIF));
            if (in_lif_ref_count != (in_lif_first_ref_count + 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for l3 intf");
            }
            /*
             * Validate qos_profile reference count 
             */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_ingress_propagation_profile.profile_data_get
                            (unit, qos_profile, &qos_profile_ref_count, &propagation_profile_type));
            if (qos_profile_ref_count != (qos_profile_first_ref_count + 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for qos profile");
            }
            /*
             * Validate action_gport reference count 
             */
            SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.profile_data_get
                            (unit, action_gport, &action_gport_ref_count, &profile_data));
            if (action_gport_ref_count != (action_gport_first_ref_count + 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action gport");
            }
            /*
             * Remove the first entry, validate ref count 
             */
            SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));
            /*
             * Mark as deleted
             */
            flow_handle_info.flow_id = 0;
            /*
             * Validate l3_intf_id reference count 
             */
            SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_ref_count(unit, in_lif_profile, &in_lif_ref_count, LIF));
            if (in_lif_ref_count != in_lif_first_ref_count)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for l3 intf");
            }
            /*
             * Validate qos_profile reference count 
             */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_ingress_propagation_profile.profile_data_get
                            (unit, qos_profile, &qos_profile_ref_count, &propagation_profile_type));
            if (qos_profile_ref_count != qos_profile_first_ref_count)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for qos profile");
            }
            /*
             * Validate action_gport reference count 
             */
            SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.profile_data_get
                            (unit, action_gport, &action_gport_ref_count, &profile_data));
            if (action_gport_ref_count != action_gport_first_ref_count)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action gport");
            }
            /*
             * Remove the second entry, validate ref count 
             */
            SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info_2, NULL));
            /*
             * Mark as deleted
             */
            flow_handle_info_2.flow_id = 0;
            /*
             * Validate l3 intf reference count 
             */
            SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_ref_count(unit, in_lif_profile, &in_lif_ref_count, LIF));
            if (in_lif_ref_count != (in_lif_first_ref_count - 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for l3 intf");
            }
            /*
             * Validate qos_profile reference count 
             */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_ingress_propagation_profile.profile_data_get
                            (unit, qos_profile, &qos_profile_ref_count, &propagation_profile_type));
            if (qos_profile_ref_count != (qos_profile_first_ref_count - 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for qos profile");
            }
            /*
             * Validate action_gport reference count 
             */
            SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.profile_data_get
                            (unit, action_gport, &action_gport_ref_count, &profile_data));
            if (action_gport_ref_count != (action_gport_first_ref_count - 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action gport");
            }
        }
        /*
         * Re add the first entry again 
         */
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_create
                        (unit, &flow_handle_info, (bcm_flow_terminator_info_t *) app_data, NULL));
    }
    else if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        lif_table_mngr_outlif_info_t outlif_info;

        /*
         * Encap access must be set
         */
        ((bcm_flow_initiator_info_t *) app_data)->encap_access =
            (flow_app_info->encap_access_default_mapping !=
             bcmEncapAccessInvalid) ? flow_app_info->encap_access_default_mapping : bcmEncapAccessTunnel2;
        SHR_IF_ERR_EXIT(bcm_flow_initiator_info_create
                        (unit, &flow_handle_info, (bcm_flow_initiator_info_t *) app_data, NULL));

        lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, flow_handle_info.flow_id, lif_flags, &gport_hw_resources));

        /** Take handle to read from DBAL table and retrieve the fields */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
        if (is_local_lif)
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                            (unit, gport_hw_resources.local_out_lif, entry_handle_id, &outlif_info));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_no_local_lif_entry_on_handle_get
                            (unit, flow_handle_info.flow_id, entry_handle_id));
        }

        /*
         * get action_gport
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, &action_gport));

        /*
         * Get action_gport reference count
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_etpp.profile_data_get
                        (unit, action_gport, &action_gport_first_ref_count, &profile_data));

        /*
         * Create another entry with the same profiles
         */
        /*
         * First - find the first supported non-profile common field to set
         */
        for (common_field_idx = 0; common_field_idx < num_common_fields; common_field_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                            (unit, flow_app_info->flow_app_type, common_field_idx, &field_desc));

            if (field_desc == NULL)
            {
                continue;
            }

            field_enabler = field_desc->init_field_enabler;
            if ((field_desc->is_profile == TRUE)
                || (!_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field_enabler)))
            {
                continue;
            }

            SHR_IF_ERR_EXIT(ctest_flow_common_field_get
                            (unit, field_enabler, flow_app_info->flow_app_type, &ctest_field_desc));
            legal_value = ctest_field_desc->legal_value;
            if (legal_value == CTEST_FLOW_SKIP_FIELD)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                            (unit, flow_app_info->flow_app_type, field_enabler, app_data, legal_value));

            ((bcm_flow_initiator_info_t *) app_data)->valid_elements_set |= field_enabler;
            flow_handle_info_2.flow_handle = flow_handle_info.flow_handle;
            SHR_IF_ERR_EXIT(bcm_flow_initiator_info_create
                            (unit, &flow_handle_info_2, (bcm_flow_initiator_info_t *) app_data, NULL));
            break;
        }
        /*
         * Second entry created
         */
        if (common_field_idx < num_common_fields)
        {
            /*
             * Validate action_gport reference count
             */
            SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_etpp.profile_data_get
                            (unit, action_gport, &action_gport_ref_count, &profile_data));
            if (action_gport_ref_count != (action_gport_first_ref_count + 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action gport");
            }
            /*
             * Remove the first entry, validate ref count
             */
            SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));
            /*
             * Mark as deleted
             */
            flow_handle_info.flow_id = 0;
            /*
             * Validate action_gport reference count
             */
            SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_etpp.profile_data_get
                            (unit, action_gport, &action_gport_ref_count, &profile_data));
            if (action_gport_ref_count != action_gport_first_ref_count)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action gport");
            }
            /*
             * Remove the second entry, validate ref count
             */
            SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info_2, NULL));
            /*
             * Mark as deleted
             */
            flow_handle_info_2.flow_id = 0;
            /*
             * Validate action_gport reference count
             */
            SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_etpp.profile_data_get
                            (unit, action_gport, &action_gport_ref_count, &profile_data));
            if (action_gport_ref_count != (action_gport_first_ref_count - 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action gport");
            }
        }
        /*
         * Re-add an entry
         */
        SHR_IF_ERR_EXIT(bcm_flow_initiator_info_create
                        (unit, &flow_handle_info, (bcm_flow_initiator_info_t *) app_data, NULL));
    }

    /*
     * set profiles to +1
     */
    for (profile = 0; profile < num_profiles; profile++)
    {
        field_enabler = profile_enablers[profile];
        if (_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field_enabler))
        {
            SHR_IF_ERR_EXIT(dnx_flow_common_fields_index_by_enabler_get
                            (unit, flow_app_info->flow_app_type, field_enabler, &field_index));
            SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                            (unit, flow_app_info->flow_app_type, field_index, &field_desc));

            if (field_desc == NULL)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(ctest_flow_common_field_get
                            (unit, field_enabler, flow_app_info->flow_app_type, &ctest_field_desc));
            legal_value = ctest_field_desc->legal_value + 1;
            SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                            (unit, flow_app_info->flow_app_type, field_enabler, app_data, legal_value));
        }
    }

    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        uint32 new_in_lif_profile, new_qos_profile, new_action_gport;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, flow_handle_info.flow_id, lif_flags, &gport_hw_resources));

        /** Take handle to read from DBAL table */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, gport_hw_resources.inlif_dbal_table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                        (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, entry_handle_id, &inlif_info));

        /*
         * get inlif profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, &in_lif_profile));
        /*
         * get qos profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROFILE, INST_SINGLE, &qos_profile));
        /*
         * get action_gport
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE, &action_gport));

        /*
         * Get l3_intf_id reference count
         */
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_ref_count(unit, in_lif_profile, &in_lif_first_ref_count, LIF));
        /*
         * Get qos_profile reference count
         */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_ingress_propagation_profile.profile_data_get
                        (unit, qos_profile, &qos_profile_first_ref_count, &propagation_profile_type));
        /*
         * Get action_gport reference count
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.profile_data_get
                        (unit, action_gport, &action_gport_first_ref_count, &profile_data));

        /*
         * Replace the entry 
         */
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_create
                        (unit, &flow_handle_info, (bcm_flow_terminator_info_t *) app_data, NULL));

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, flow_handle_info.flow_id, lif_flags, &gport_hw_resources));

        /** Take handle to read from DBAL table */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, gport_hw_resources.inlif_dbal_table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                        (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, entry_handle_id, &inlif_info));

        /*
         * get new inlif profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, &new_in_lif_profile));
        /*
         * get qos profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROFILE, INST_SINGLE, &new_qos_profile));
        /*
         * get action_gport
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE, &new_action_gport));

        /*
         * Validate l3_intf_id reference count
         */
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_ref_count(unit, in_lif_profile, &in_lif_ref_count, LIF));
        if (new_in_lif_profile != in_lif_profile)
        {
            if (in_lif_ref_count != (in_lif_first_ref_count - 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for l3 intf");
            }
        }
        else    /* Same in lif profile */
        {
            if (in_lif_ref_count != in_lif_first_ref_count)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for l3 intf");
            }
        }
        /*
         * Validate qos_profile reference count
         */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_ingress_propagation_profile.profile_data_get
                        (unit, qos_profile, &qos_profile_ref_count, &propagation_profile_type));
        if (qos_profile != new_qos_profile)
        {
            if (qos_profile_ref_count != (qos_profile_first_ref_count - 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for qos profile");
            }
        }
        else    /* Same qos profile */
        {
            if (qos_profile_ref_count != qos_profile_first_ref_count)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for qos profile");
            }
        }
        /*
         * Validate action_gport reference count
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.profile_data_get
                        (unit, action_gport, &action_gport_ref_count, &profile_data));
        if (action_gport != new_action_gport)
        {
            if (action_gport_ref_count != (action_gport_first_ref_count - 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action gport");
            }
        }
        else    /* Same action gport */
        {
            if (action_gport_ref_count != action_gport_first_ref_count)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action gport");
            }
        }

        /*
         * Validate ref counts for new profiles
         */
        /*
         * Get l3_intf_id reference count
         */
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_ref_count(unit, new_in_lif_profile, &in_lif_first_ref_count, LIF));
        /*
         * Get qos_profile reference count
         */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_ingress_propagation_profile.profile_data_get
                        (unit, new_qos_profile, &qos_profile_first_ref_count, &propagation_profile_type));
        /*
         * Get action_gport reference count
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.profile_data_get
                        (unit, new_action_gport, &action_gport_first_ref_count, &profile_data));

        flow_handle_info.flags = 0;
        /*
         * Remove the first entry, validate ref count
         */
        SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));
        /*
         * Mark as deleted
         */
        flow_handle_info.flow_id = 0;

        /*
         * Validate l3_intf_id reference count
         */
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_ref_count(unit, new_in_lif_profile, &in_lif_ref_count, LIF));
        if (in_lif_ref_count != (in_lif_first_ref_count - 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for l3 intf after replace");
        }
        /*
         * Validate qos_profile reference count
         */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_ingress_propagation_profile.profile_data_get
                        (unit, new_qos_profile, &qos_profile_ref_count, &propagation_profile_type));
        if (qos_profile_ref_count != (qos_profile_first_ref_count - 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for qos profile after replace");
        }
        /*
         * Validate action_gport reference count
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.profile_data_get
                        (unit, new_action_gport, &action_gport_ref_count, &profile_data));
        if (action_gport_ref_count != (action_gport_first_ref_count - 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action gport after replace");
        }
    }
    else if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        uint32 new_action_gport;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, flow_handle_info.flow_id, lif_flags, &gport_hw_resources));

        /** Take handle to read from DBAL table and retrieve the fields */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, gport_hw_resources.outlif_dbal_table_id, entry_handle_id));
        if (is_local_lif)
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                            (unit, gport_hw_resources.local_out_lif, entry_handle_id, &outlif_info));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_no_local_lif_entry_on_handle_get
                            (unit, flow_handle_info.flow_id, entry_handle_id));
        }

        /*
         * get action_gport
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, &action_gport));

        /*
         * Get action_gport reference count
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_etpp.profile_data_get
                        (unit, action_gport, &action_gport_first_ref_count, &profile_data));

        /*
         * Replace with new profiles values 
         */
        /*
         * Encap access must be set to invalid on replace
         */
        ((bcm_flow_initiator_info_t *) app_data)->encap_access = bcmEncapAccessInvalid;
        SHR_IF_ERR_EXIT(bcm_flow_initiator_info_create
                        (unit, &flow_handle_info, (bcm_flow_initiator_info_t *) app_data, NULL));

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, flow_handle_info.flow_id, lif_flags, &gport_hw_resources));

        /** Take handle to read from DBAL table and retrieve the fields */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, gport_hw_resources.outlif_dbal_table_id, entry_handle_id));
        if (is_local_lif)
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                            (unit, gport_hw_resources.local_out_lif, entry_handle_id, &outlif_info));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_no_local_lif_entry_on_handle_get
                            (unit, flow_handle_info.flow_id, entry_handle_id));
        }

        /*
         * get action_gport
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, &new_action_gport));

        /*
         * Validate action_gport reference count
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_etpp.profile_data_get
                        (unit, action_gport, &action_gport_ref_count, &profile_data));
        if (new_action_gport != action_gport)
        {
            if (action_gport_ref_count != (action_gport_first_ref_count - 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action gport after replace");
            }
        }
        else    /* Same action gport */
        {
            if (action_gport_ref_count != action_gport_first_ref_count)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action gport after replace");
            }
        }

        /*
         * Get action_gport reference count
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_etpp.profile_data_get
                        (unit, new_action_gport, &action_gport_first_ref_count, &profile_data));

        flow_handle_info.flags = 0;

        /*
         * Remove the first entry, validate ref count
         */
        SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));
        /*
         * Mark as deleted
         */
        flow_handle_info.flow_id = 0;
        /*
         * Validate action_gport reference count
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_etpp.profile_data_get
                        (unit, new_action_gport, &action_gport_ref_count, &profile_data));
        if (action_gport_ref_count != (action_gport_first_ref_count - 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action gport after replace");
        }
    }

    LOG_CLI((BSL_META("App fields test for application %s PASSED\n"), flow_app_info->app_name));

exit:
    flow_handle_info.flags = 0;
    if (flow_handle_info.flow_id)
    {
        rv = ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("Failed removing flow entry at cleanup\n")));
        }
    }

    if (flow_handle_info_2.flow_id)
    {
        rv = ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info_2, NULL);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("Failed removing flow entry at cleanup\n")));
        }

    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_flow_egress_qos_model_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    shr_error_e rv;
    const dnx_flow_app_config_t *flow_app_info = NULL;
    bcm_flow_initiator_info_t initiator_info = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_handle_info_t flow_handle_info_2 = { 0 };
    const dnx_flow_common_field_desc_t *field_desc;
    ctest_flow_common_field_desc_t *ctest_field_desc;
    uint32 field_enabler;
    int legal_value, field_idx, common_field_idx, num_common_fields;
    int is_local_lif = TRUE;
    uint32 entry_handle_id;
    uint8 ipv4_tunnel_exists = FALSE;
    uint8 ttl_enabled = FALSE;
    uint8 nwk_qos_enabled = FALSE;
    lif_table_mngr_outlif_info_t outlif_info;
    uint32 ttl_idx = 0;
    uint32 lif_flags = 0;
    uint32 ttl_profile = 0, nwk_profile = 0, new_ttl_profile = 0, new_nwk_profile = 0;
    int ttl_profile_first_ref_count = 0, ttl_profile_ref_count = 0;
    int nwk_profile_first_ref_count = 0, nwk_profile_ref_count = 0;
    qos_pipe_profile_template_t network_qos_profile_data;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_WITH_EXCEPTION(ctest_flow_test_begin
                                   (unit, args, sand_control, FLOW_ALLOWED_APP_TYPES_ALL, 0, &flow_app_info,
                                    &flow_handle_info), _SHR_E_PARAM);

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));
    }

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_info->flow_app_type, &num_common_fields));

    if (!_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Application %s doesn't support egress qos model", flow_app_info->app_name);
    }

    /*
     * Apply the ENCAP ACCESS logic only for applications that require it as a user parameter
     */
    if ((flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT) && is_local_lif)
    {
        initiator_info.encap_access =
            (flow_app_info->encap_access_default_mapping !=
             bcmEncapAccessInvalid) ? flow_app_info->encap_access_default_mapping : bcmEncapAccessTunnel2;
    }

    for (field_idx = 0; flow_app_info->special_fields[field_idx] != FLOW_S_F_EMPTY; field_idx++)
    {
        /** for all qos params, taking the default value of 1 */
        if (flow_app_info->special_fields[field_idx] == FLOW_S_F_QOS_TTL)
        {
            SPECIAL_FIELD_UINT32_DATA_ADD((&special_fields), FLOW_S_F_QOS_TTL, 1);
            ttl_enabled = TRUE;
            ttl_idx = field_idx;
        }
        else if (flow_app_info->special_fields[field_idx] == FLOW_S_F_QOS_DSCP)
        {
            SPECIAL_FIELD_UINT32_DATA_ADD((&special_fields), FLOW_S_F_QOS_DSCP, 1);
            nwk_qos_enabled = TRUE;
        }
        else if (flow_app_info->special_fields[field_idx] == FLOW_S_F_QOS_EXP)
        {
            SPECIAL_FIELD_UINT32_DATA_ADD((&special_fields), FLOW_S_F_QOS_EXP, 1);
            nwk_qos_enabled = TRUE;
        }
        else if (flow_app_info->special_fields[field_idx] == FLOW_S_F_QOS_PRI)
        {
            SPECIAL_FIELD_UINT32_DATA_ADD((&special_fields), FLOW_S_F_QOS_PRI, 1);
            nwk_qos_enabled = TRUE;
        }
        else if (flow_app_info->special_fields[field_idx] == FLOW_S_F_QOS_CFI)
        {
            SPECIAL_FIELD_UINT32_DATA_ADD((&special_fields), FLOW_S_F_QOS_CFI, 1);
            nwk_qos_enabled = TRUE;
        }
        /** mandatory special fields */
        else if (flow_app_info->special_fields[field_idx] == FLOW_S_F_IPV4_TUNNEL_TYPE)
        {
            ipv4_tunnel_exists = TRUE;
        }
    }
    if ((nwk_qos_enabled == FALSE) && (ttl_enabled == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Application %s doesn't support ttl or nwk profiles, nothing to do in the test",
                     flow_app_info->app_name);
    }

    /** IPV4_TUNNEL_TYPE should be last for test convenience */
    if (ipv4_tunnel_exists == TRUE)
    {
        SPECIAL_FIELD_SYMBOL_DATA_ADD((&special_fields), FLOW_S_F_IPV4_TUNNEL_TYPE, bcmTunnelTypeIpAnyIn4);
    }

    /** add qos_egress_model common field and its values */
    initiator_info.valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID;

    initiator_info.egress_qos_model.egress_qos = nwk_qos_enabled ? bcmQosEgressModelPipeMyNameSpace
        : bcmQosEgressModelPipeNextNameSpace;

    initiator_info.egress_qos_model.egress_ttl = ttl_enabled ? bcmQosEgressModelPipeMyNameSpace :
        bcmQosEgressModelPipeNextNameSpace;

    /*
     * Create first entry
     */
    SHR_IF_ERR_EXIT(ctest_flow_lif_info_create
                    (unit, flow_app_info, &flow_handle_info, &initiator_info, &special_fields,
                     initiator_info.valid_elements_set, 1));

    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, flow_handle_info.flow_id, lif_flags, &gport_hw_resources));

    /** Take handle to read from DBAL table and retrieve the fields */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
    if (is_local_lif)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                        (unit, gport_hw_resources.local_out_lif, entry_handle_id, &outlif_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_no_local_lif_entry_on_handle_get
                        (unit, flow_handle_info.flow_id, entry_handle_id));
    }

    if (ttl_enabled)
    {
        /*
         * get ttl profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE, &ttl_profile));

        /** get TTL ref count from template manager */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.profile_data_get
                        (unit, ttl_profile, &ttl_profile_first_ref_count,
                         (void *) &(special_fields.special_fields[ttl_idx].shr_var_uint32)));
    }
    if (nwk_qos_enabled)
    {
        /*
         * get nwk profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, &nwk_profile));

        /** get network profile ref count from template manager */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_network_qos_profile.profile_data_get
                        (unit, nwk_profile, &nwk_profile_first_ref_count, &network_qos_profile_data));
    }

    /*
     * Create another entry with the same profiles
     */
    /*
     * First - find the first supported non-profile common field to set
     */
    for (common_field_idx = 0; common_field_idx < num_common_fields; common_field_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                        (unit, flow_app_info->flow_app_type, common_field_idx, &field_desc));

        if (field_desc == NULL)
        {
            continue;
        }

        field_enabler = field_desc->init_field_enabler;
        if ((field_desc->is_profile == TRUE)
            || (!_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field_enabler)))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(ctest_flow_common_field_get
                        (unit, field_enabler, flow_app_info->flow_app_type, &ctest_field_desc));
        legal_value = ctest_field_desc->legal_value;
        if (legal_value == CTEST_FLOW_SKIP_FIELD)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                        (unit, flow_app_info->flow_app_type, field_enabler, (void *) &initiator_info, legal_value));

        initiator_info.valid_elements_set |= field_enabler;
        flow_handle_info_2.flow_handle = flow_handle_info.flow_handle;
        SHR_IF_ERR_EXIT(ctest_flow_lif_info_create
                        (unit, flow_app_info, &flow_handle_info_2, &initiator_info, &special_fields,
                         initiator_info.valid_elements_set, 1));
        break;
    }

    /*
     * Second entry created
     */
    if (common_field_idx < num_common_fields)
    {
        if (ttl_enabled)
        {
            /** get TTL ref count from template manager */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.profile_data_get
                            (unit, ttl_profile, &ttl_profile_ref_count,
                             (void *) &(special_fields.special_fields[ttl_idx].shr_var_uint32)));
            if (ttl_profile_ref_count != (ttl_profile_first_ref_count + 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action ttl");
            }
        }

        if (nwk_qos_enabled)
        {
            /** get network profile ref count from template manager */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_network_qos_profile.profile_data_get
                            (unit, nwk_profile, &nwk_profile_ref_count, &network_qos_profile_data));
            if (nwk_profile_ref_count != (nwk_profile_first_ref_count + 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action nwk");
            }
        }

        /*
         * Remove the first entry, validate ref count
         */
        SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));
        /*
         * Mark as deleted
         */
        flow_handle_info.flow_id = 0;

        if (ttl_enabled)
        {
            /** get TTL ref count from template manager */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.profile_data_get
                            (unit, ttl_profile, &ttl_profile_ref_count,
                             (void *) &(special_fields.special_fields[ttl_idx].shr_var_uint32)));
            if (ttl_profile_ref_count != ttl_profile_first_ref_count)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action ttl");
            }
        }

        if (nwk_qos_enabled)
        {
            /** get network profile ref count from template manager */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_network_qos_profile.profile_data_get
                            (unit, nwk_profile, &nwk_profile_ref_count, &network_qos_profile_data));
            if (nwk_profile_ref_count != nwk_profile_first_ref_count)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action nwk");
            }
        }

        /*
         * Remove the second entry, validate ref count
         */
        SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info_2, NULL));
        /*
         * Mark as deleted
         */
        flow_handle_info_2.flow_id = 0;

        if (ttl_enabled)
        {
            /** get TTL ref count from template manager */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.profile_data_get
                            (unit, ttl_profile, &ttl_profile_ref_count,
                             (void *) &(special_fields.special_fields[ttl_idx].shr_var_uint32)));
            if (ttl_profile_ref_count != (ttl_profile_first_ref_count - 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action ttl");
            }
        }

        if (nwk_qos_enabled)
        {
            /** get network profile ref count from template manager */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_network_qos_profile.profile_data_get
                            (unit, nwk_profile, &nwk_profile_ref_count, &network_qos_profile_data));
            if (nwk_profile_ref_count != (nwk_profile_first_ref_count - 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for action nwk");
            }
        }
    }

    /*
     * Re-add an entry
     */
    SHR_IF_ERR_EXIT(bcm_flow_initiator_info_create(unit, &flow_handle_info, &initiator_info, &special_fields));

    /*
     * set profiles to +1
     */
    for (field_idx = 0; field_idx < special_fields.actual_nof_special_fields; field_idx++)
    {
        if (flow_app_info->special_fields[field_idx] == FLOW_S_F_QOS_TTL)
        {
            special_fields.special_fields[field_idx].shr_var_uint32++;
        }
        else if (flow_app_info->special_fields[field_idx] == FLOW_S_F_QOS_DSCP)
        {
            special_fields.special_fields[field_idx].shr_var_uint32++;
        }
        else if (flow_app_info->special_fields[field_idx] == FLOW_S_F_QOS_EXP)
        {
            special_fields.special_fields[field_idx].shr_var_uint32++;
        }
        else if (flow_app_info->special_fields[field_idx] == FLOW_S_F_QOS_PRI)
        {
            special_fields.special_fields[field_idx].shr_var_uint32++;
        }
        else if (flow_app_info->special_fields[field_idx] == FLOW_S_F_QOS_CFI)
        {
            special_fields.special_fields[field_idx].shr_var_uint32++;
        }
    }

    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

    /** if IPV4_TUNNEL_TYPE exists, we should remove it before replace */
    if (ipv4_tunnel_exists == TRUE)
    {
        special_fields.actual_nof_special_fields--;
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, flow_handle_info.flow_id, lif_flags, &gport_hw_resources));

    /** Take handle to read from DBAL table and retrieve the fields */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, gport_hw_resources.outlif_dbal_table_id, entry_handle_id));
    if (is_local_lif)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                        (unit, gport_hw_resources.local_out_lif, entry_handle_id, &outlif_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_no_local_lif_entry_on_handle_get
                        (unit, flow_handle_info.flow_id, entry_handle_id));
    }

    if (ttl_enabled)
    {
        /*
         * get ttl profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE, &ttl_profile));

        /** get TTL ref count from template manager */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.profile_data_get
                        (unit, ttl_profile, &ttl_profile_first_ref_count,
                         (void *) &(special_fields.special_fields[ttl_idx].shr_var_uint32)));
    }
    if (nwk_qos_enabled)
    {
        /*
         * get nwk profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, &nwk_profile));

        /** get network profile ref count from template manager */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_network_qos_profile.profile_data_get
                        (unit, nwk_profile, &nwk_profile_first_ref_count, &network_qos_profile_data));
    }

    /*
     * Replace with new profiles values
     */
    /*
     * encap access must be set to invalid on replace
     */
    initiator_info.encap_access = bcmEncapAccessInvalid;
    SHR_IF_ERR_EXIT(bcm_flow_initiator_info_create(unit, &flow_handle_info, &initiator_info, &special_fields));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, flow_handle_info.flow_id, lif_flags, &gport_hw_resources));

    /** Take handle to read from DBAL table and retrieve the fields */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, gport_hw_resources.outlif_dbal_table_id, entry_handle_id));
    if (is_local_lif)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                        (unit, gport_hw_resources.local_out_lif, entry_handle_id, &outlif_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_no_local_lif_entry_on_handle_get
                        (unit, flow_handle_info.flow_id, entry_handle_id));
    }

    if (ttl_enabled)
    {
        /*
         * get ttl profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE, &new_ttl_profile));

        /** get TTL ref count from template manager */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.profile_data_get
                        (unit, ttl_profile, &ttl_profile_ref_count,
                         (void *) &(special_fields.special_fields[ttl_idx].shr_var_uint32)));
    }
    if (nwk_qos_enabled)
    {
        /*
         * get nwk profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, &new_nwk_profile));

        /** get network profile ref count from template manager */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_network_qos_profile.profile_data_get
                        (unit, nwk_profile, &nwk_profile_ref_count, &network_qos_profile_data));
    }

    if (new_ttl_profile != ttl_profile)
    {
        if (ttl_profile_ref_count != (ttl_profile_first_ref_count - 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for ttl after replace");
        }
    }
    else        /* Same ttl profile */
    {
        if (ttl_profile_ref_count != ttl_profile_first_ref_count)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for ttl after replace");
        }
    }

    if (new_nwk_profile != nwk_profile)
    {
        if (nwk_profile_ref_count != (nwk_profile_first_ref_count - 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for nwk after replace");
        }
    }
    else        /* Same nwk profile */
    {
        if (nwk_profile_ref_count != nwk_profile_first_ref_count)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for nwk after replace");
        }
    }

    /*
     * Remove the first entry, validate ref count
     */
    flow_handle_info.flags = 0;
    SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));
    /*
     * Mark as deleted
     */
    flow_handle_info.flow_id = 0;

    if (ttl_enabled)
    {
        /*
         * get ttl profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE, &new_ttl_profile));

        /** get TTL ref count from template manager */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.profile_data_get
                        (unit, ttl_profile, &ttl_profile_ref_count,
                         (void *) &(special_fields.special_fields[ttl_idx].shr_var_uint32)));
        if (ttl_profile_ref_count != (ttl_profile_first_ref_count - 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for ttl after replace");
        }
    }
    if (nwk_qos_enabled)
    {
        /*
         * get nwk profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, &new_nwk_profile));

        /** get network profile ref count from template manager */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_network_qos_profile.profile_data_get
                        (unit, nwk_profile, &nwk_profile_ref_count, &network_qos_profile_data));

        if (nwk_profile_ref_count != (nwk_profile_first_ref_count - 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Failure, wrong reference count for nwk profile after replace");
        }
    }

    LOG_CLI((BSL_META("Egress qos model test for application %s PASSED\n"), flow_app_info->app_name));

exit:
    if (flow_handle_info.flow_id)
    {
        rv = ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("Failed removing flow entry at cleanup\n")));
        }
    }

    if (flow_handle_info_2.flow_id)
    {
        rv = ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info_2, NULL);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("Failed removing flow entry at cleanup\n")));
        }
    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Run special fields replace test.
 * 1. Add entry with 1 special field, remove this field field and set another one, validate
 * 2. Add entry with 1 special field, keep the original special field and add another one, validate
 * 3. Add entry with 2 special fields, remove one and update the second, validate
 *
 * \return
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
ctest_flow_special_fields_replace_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    const dnx_flow_app_config_t *flow_app_info = NULL;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    int rv;
    bcm_flow_special_fields_t special_fields = { 0 };
    bcm_flow_special_fields_t special_fields_get = { 0 };
    const flow_special_field_info_t *special_field_info0, *special_field_info1;
    void *app_data;
    bcm_flow_terminator_info_t terminator_info = { 0 };
    bcm_flow_initiator_info_t initiator_info = { 0 };
    bcm_flow_special_field_t set_value0, set_value1;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_WITH_EXCEPTION(ctest_flow_test_begin
                                   (unit, args, sand_control, FLOW_ALLOWED_APP_TYPES_LIF, 0, &flow_app_info,
                                    &flow_handle_info), _SHR_E_PARAM);

    SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, flow_app_info->special_fields[0], &special_field_info0));
    SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, flow_app_info->special_fields[1], &special_field_info1));

    /*
     * Check if given application has at least 2 special fields 
     */
    if (flow_app_info->special_fields[1] == FLOW_S_F_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Test application must have at least 2 special fields\n");
    }

    if (((special_field_info0->payload_type != FLOW_PAYLOAD_TYPE_UINT32)) ||
        ((special_field_info1->payload_type != FLOW_PAYLOAD_TYPE_UINT32)) ||
        (dnx_flow_special_field_is_ind_set
         (unit, special_field_info0, FLOW_SPECIAL_FIELD_IND_SRC_ADDR_PROFILE_SUPPORTED))
        ||
        (dnx_flow_special_field_is_ind_set
         (unit, special_field_info1, FLOW_SPECIAL_FIELD_IND_SRC_ADDR_PROFILE_SUPPORTED)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Test only supports applications with uint first 2 special fields\n");
    }

    if (dnx_flow_special_field_is_ind_set(unit, special_field_info0, FLOW_SPECIAL_FIELD_IND_LIF_MANDATORY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot run test with mandatory fields %s \n",
                     dnx_flow_special_field_to_string(unit, flow_app_info->special_fields[0]));
    }

    if (dnx_flow_special_field_is_ind_set(unit, special_field_info1, FLOW_SPECIAL_FIELD_IND_LIF_MANDATORY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot run test with mandatory fields %s \n",
                     dnx_flow_special_field_to_string(unit, flow_app_info->special_fields[1]));
    }

    app_data =
        (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) ? (void *) &terminator_info : (void *) &initiator_info;

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        initiator_info.encap_access =
            (flow_app_info->encap_access_default_mapping !=
             bcmEncapAccessInvalid) ? flow_app_info->encap_access_default_mapping : bcmEncapAccessTunnel2;
    }

    special_fields.special_fields[0].field_id = flow_app_info->special_fields[0];
    special_fields.special_fields[1].field_id = flow_app_info->special_fields[1];

    SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_legal_value_get
                    (unit, flow_app_info, 0, flow_app_info->special_fields[0], &set_value0));

    SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_legal_value_get
                    (unit, flow_app_info, 0, flow_app_info->special_fields[1], &set_value1));

    SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, special_fields.special_fields[0].field_id, &special_field_info0));
    SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, special_fields.special_fields[1].field_id, &special_field_info1));

    CTEST_DNX_FLOW_SPECIAL_FIELD_SET(special_field_info0, set_value0, 0);
    CTEST_DNX_FLOW_SPECIAL_FIELD_SET(special_field_info1, set_value1, 1);

    /*
     * First test case - 1 special field, to be replaced 
     */

    LOG_CLI((BSL_META("First test case - one special field, to be replaced with another \n\n")));

    LOG_CLI((BSL_META("Adding entry with one special field %s - value %d\n"),
             dnx_flow_special_field_to_string(unit, flow_app_info->special_fields[0]), set_value0.shr_var_uint32));

    special_fields.actual_nof_special_fields = 1;
    SHR_IF_ERR_EXIT(ctest_flow_lif_info_create
                    (unit, flow_app_info, &flow_handle_info, app_data, &special_fields, 0, 1));

    /*
     * Get entry 
     */
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_get
                        (unit, &flow_handle_info, (bcm_flow_terminator_info_t *) app_data, &special_fields_get));
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get
                        (unit, &flow_handle_info, (bcm_flow_initiator_info_t *) app_data, &special_fields_get));
    }

    if ((special_fields_get.actual_nof_special_fields != 1) ||
        (special_fields_get.special_fields[0].field_id != special_fields.special_fields[0].field_id) ||
        (special_fields_get.special_fields[0].shr_var_uint32 != special_fields.special_fields[0].shr_var_uint32))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Scenario 1: Original special field mismatch between set and get nof_special_fields = %d\n",
                     special_fields_get.actual_nof_special_fields);
    }

    LOG_CLI((BSL_META("Entry Confirmed\n")));

    /*
     * Now - replace with the other special field 
     */
    LOG_CLI((BSL_META("Replacing entry removing old field and adding special field %s - value %d\n"),
             dnx_flow_special_field_to_string(unit, flow_app_info->special_fields[1]), set_value1.shr_var_uint32));

    special_fields.actual_nof_special_fields = 2;
    special_fields.special_fields[0].is_clear = TRUE;
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

    initiator_info.encap_access = bcmEncapAccessInvalid;

    SHR_IF_ERR_EXIT(ctest_flow_lif_info_create
                    (unit, flow_app_info, &flow_handle_info, app_data, &special_fields, 0, 0));

    /*
     * Get entry 
     */
    flow_handle_info.flags = 0;
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_get
                        (unit, &flow_handle_info, (bcm_flow_terminator_info_t *) app_data, &special_fields_get));
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get
                        (unit, &flow_handle_info, (bcm_flow_initiator_info_t *) app_data, &special_fields_get));
    }

    if ((special_fields_get.actual_nof_special_fields != 1) ||
        (special_fields_get.special_fields[0].field_id != special_fields.special_fields[1].field_id) ||
        (special_fields_get.special_fields[0].shr_var_uint32 != special_fields.special_fields[1].shr_var_uint32))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Scenario 1: Special field mismatch between set and get after replace nof_special_fields = %d\n",
                     special_fields_get.actual_nof_special_fields);
    }

    LOG_CLI((BSL_META("Entry Confirmed\n")));

    /*
     * Delete the entry
     */
    SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));
    flow_handle_info.flow_id = 0;

    LOG_CLI((BSL_META("First test case completed\n\n")));

    /*
     * Second test case - 1 special field, then add another one 
     */

    LOG_CLI((BSL_META("Second test case - one special field, then add another one\n\n")));
    LOG_CLI((BSL_META("Adding entry with 1 special field %s - value %d\n"),
             dnx_flow_special_field_to_string(unit, flow_app_info->special_fields[0]), set_value0.shr_var_uint32));

    special_fields.actual_nof_special_fields = 1;
    special_fields.special_fields[0].is_clear = FALSE;
    flow_handle_info.flags = 0;

    SHR_IF_ERR_EXIT(ctest_flow_lif_info_create
                    (unit, flow_app_info, &flow_handle_info, app_data, &special_fields, 0, 1));

    /*
     * Get entry 
     */
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_get
                        (unit, &flow_handle_info, (bcm_flow_terminator_info_t *) app_data, &special_fields_get));
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get
                        (unit, &flow_handle_info, (bcm_flow_initiator_info_t *) app_data, &special_fields_get));
    }

    if ((special_fields_get.actual_nof_special_fields != 1) ||
        (special_fields_get.special_fields[0].field_id != special_fields.special_fields[0].field_id) ||
        (special_fields_get.special_fields[0].shr_var_uint32 != special_fields.special_fields[0].shr_var_uint32))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Scenario 2: Original special field mismatch between set and get nof_special_fields = %d\n",
                     special_fields_get.actual_nof_special_fields);
    }

    LOG_CLI((BSL_META("Entry Confirmed\n")));

    /*
     * Now - add the second special field 
     */
    sal_memset(&special_fields, 0x0, sizeof(bcm_flow_special_fields_t));
    special_fields.actual_nof_special_fields = 1;
    special_fields.special_fields[0].field_id = flow_app_info->special_fields[1];
    CTEST_DNX_FLOW_SPECIAL_FIELD_SET(special_field_info0, set_value1, 0);

    LOG_CLI((BSL_META("Adding another entry with 1 special field %s - value %d\n"),
             dnx_flow_special_field_to_string(unit, flow_app_info->special_fields[1]), set_value1.shr_var_uint32));

    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

    initiator_info.encap_access = bcmEncapAccessInvalid;

    SHR_IF_ERR_EXIT(ctest_flow_lif_info_create
                    (unit, flow_app_info, &flow_handle_info, app_data, &special_fields, 0, 0));

    /*
     * Get entry 
     */
    flow_handle_info.flags = 0;

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_get
                        (unit, &flow_handle_info, (bcm_flow_terminator_info_t *) app_data, &special_fields_get));
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get
                        (unit, &flow_handle_info, (bcm_flow_initiator_info_t *) app_data, &special_fields_get));
    }

    if (special_fields_get.actual_nof_special_fields != 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Scenario 2: Wrong number of special fields %d after adding another special field (expected 2)\n",
                     special_fields_get.actual_nof_special_fields);
    }
    else
    {
        uint8 field_1_idx, field_2_idx;

        field_1_idx =
            (special_fields_get.special_fields[0].field_id == special_fields.special_fields[0].field_id) ? 0 : 1;
        field_2_idx = !field_1_idx;

        /** verifing the values (order doesn't matter) */
        if ((special_fields_get.special_fields[field_1_idx].shr_var_uint32 != set_value1.shr_var_uint32)
            || (special_fields_get.special_fields[field_2_idx].shr_var_uint32 != set_value0.shr_var_uint32))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Scenario 2: After adding another field, failed getting the correct value for the special fields field %s val = %d field %s val = %d \n",
                         dnx_flow_special_field_to_string(unit,
                                                          special_fields_get.special_fields[field_1_idx].field_id),
                         special_fields_get.special_fields[field_1_idx].shr_var_uint32,
                         dnx_flow_special_field_to_string(unit,
                                                          special_fields_get.special_fields[field_2_idx].field_id),
                         special_fields_get.special_fields[field_2_idx].shr_var_uint32);
        }
    }

    /*
     * Delete the entry
     */
    SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));
    flow_handle_info.flow_id = 0;

    LOG_CLI((BSL_META("Second test case completed\n")));

    /*
     * Third test case - 2 special field, then remove one and update the other 
     */
    sal_memset(&special_fields, 0x0, sizeof(bcm_flow_special_fields_t));
    special_fields.actual_nof_special_fields = 2;
    special_fields.special_fields[0].field_id = flow_app_info->special_fields[0];
    special_fields.special_fields[1].field_id = flow_app_info->special_fields[1];

    CTEST_DNX_FLOW_SPECIAL_FIELD_SET(special_field_info0, set_value0, 0);
    CTEST_DNX_FLOW_SPECIAL_FIELD_SET(special_field_info1, set_value1, 1);

    SHR_IF_ERR_EXIT(ctest_flow_lif_info_create
                    (unit, flow_app_info, &flow_handle_info, app_data, &special_fields, 0, 1));

    /*
     * Get entry 
     */
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_get
                        (unit, &flow_handle_info, (bcm_flow_terminator_info_t *) app_data, &special_fields_get));
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get
                        (unit, &flow_handle_info, (bcm_flow_initiator_info_t *) app_data, &special_fields_get));
    }

    if (special_fields_get.actual_nof_special_fields != 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Scenario 3: Wrong number of special fields after set 2 of them\n");
    }
    else
    {
        uint8 field_1_idx, field_2_idx;

        field_1_idx =
            (special_fields_get.special_fields[0].field_id == special_fields.special_fields[0].field_id) ? 0 : 1;
        field_2_idx = !field_1_idx;

        if ((special_fields_get.special_fields[field_1_idx].shr_var_uint32 !=
             special_fields.special_fields[0].shr_var_uint32)
            || (special_fields_get.special_fields[field_2_idx].shr_var_uint32 !=
                special_fields.special_fields[1].shr_var_uint32))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Scenario 3: After setting 2 special fields, failed getting getting their correct values\n");
        }
    }

    /*
     * Now - remove the first field and update the second 
     */
    special_fields.actual_nof_special_fields = 2;
    special_fields.special_fields[0].is_clear = TRUE;
    special_fields.special_fields[1].shr_var_uint32++;
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

    initiator_info.encap_access = bcmEncapAccessInvalid;

    SHR_IF_ERR_EXIT(ctest_flow_lif_info_create
                    (unit, flow_app_info, &flow_handle_info, app_data, &special_fields, 0, 0));

    /*
     * Get entry 
     */
    flow_handle_info.flags = 0;
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_get
                        (unit, &flow_handle_info, (bcm_flow_terminator_info_t *) app_data, &special_fields_get));
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get
                        (unit, &flow_handle_info, (bcm_flow_initiator_info_t *) app_data, &special_fields_get));
    }

    if ((special_fields_get.actual_nof_special_fields != 1) ||
        (special_fields_get.special_fields[0].field_id != special_fields.special_fields[1].field_id) ||
        (special_fields_get.special_fields[0].shr_var_uint32 != special_fields.special_fields[1].shr_var_uint32))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Scenario 3: After removing the first special field and updating the other, wrong special fields values\n");
    }

    /*
     * Delete the entry
     */
    SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL));
    flow_handle_info.flow_id = 0;

    LOG_CLI((BSL_META("Special fields replace test for application %s PASSED\n"), flow_app_info->app_name));

exit:
    if (flow_handle_info.flow_id)
    {
        flow_handle_info.flags = 0;
        rv = ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info, NULL);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("Failed removing flow entry at cleanup\n")));
        }
    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/******************************************************************************************************
 *                                              COMMON_FIELDS_TEST START
 ******************************************************************************************************/

/*
 * Find the first set bit position in a uint32 number
 */
static int
dnx_flow_first_bit_set_find(
    uint32 number)
{
    int cnt = 0;

    if (number)
    {
        while (!(number & (1 << cnt++)));
    }
    else
    {
        cnt = -1;
    }

    return cnt;
}

/*
 * Validate that each common field in the term, init common fields list is located in its appropriate position according to its enabler
 */
static shr_error_e
ctest_flow_common_fields_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    /** validations for the application definitions, needed only during development. (can be moved to ctests) */
    int field_pos, num_terminator_common_fields, num_initiator_common_fields;
    const dnx_flow_common_field_desc_t *field_desc = NULL;
    uint8 test_failed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, FLOW_APP_TYPE_TERM, &num_terminator_common_fields));
    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, FLOW_APP_TYPE_INIT, &num_initiator_common_fields));

    /** Iterate over terminator common fields, see that they're in the correct position compared to their enabler  */
    for (field_pos = 0; field_pos < num_terminator_common_fields; field_pos++)
    {
        SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get(unit, FLOW_APP_TYPE_TERM, field_pos, &field_desc));
        if ((field_desc != NULL) && (field_desc->term_field_enabler != FLOW_ENABLER_INVALID))
        {
            if (field_desc->term_field_enabler != (1 << field_pos))
            {
                /*
                 * Get on bit index in enabler
                 */
                int index = dnx_flow_first_bit_set_find(field_desc->term_field_enabler);

                LOG_CLI((BSL_META
                         ("For field %s, index in list %d, field should be at index %d in term common fields list\n"),
                         field_desc->field_name, field_pos, index));

                test_failed = TRUE;
            }
        }
    }

    /*
     * Iterate over initiator common fields, see that they're in the correct position compared to their enabler
     */
    for (field_pos = 0; field_pos < num_initiator_common_fields; field_pos++)
    {
        SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get(unit, FLOW_APP_TYPE_INIT, field_pos, &field_desc));
        if ((field_desc != NULL) && (field_desc->init_field_enabler != FLOW_ENABLER_INVALID))
        {
            if (field_desc->init_field_enabler != (1 << field_pos))
            {
                /*
                 * Get on bit index in enabler
                 */
                int index = dnx_flow_first_bit_set_find(field_desc->init_field_enabler);

                LOG_CLI((BSL_META
                         ("For field %s, index in list %d, field should be at index %d in init common fields list\n"),
                         field_desc->field_name, field_pos, index));

                test_failed = TRUE;
            }
        }
    }

    if (test_failed)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "common fields test FAILED");
    }
    else
    {
        LOG_CLI((BSL_META("common fields test PASSED\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

/******************************************************************************************************
 *                                              SRC_ADDRESS_TEST START
 ******************************************************************************************************/

/* Internal, get the profile associated with the src addr created (according to its type) */
static shr_error_e
ctest_flow_src_addr_profile_get(
    int unit,
    bcm_flow_special_field_t * src_addr_field,
    uint32 dbal_field_id,
    int *profile)
{
    source_address_entry_t source_address_entry = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    switch (dbal_field_id)
    {
        case DBAL_FIELD_IPV4_SIP:
            source_address_entry.address_type = source_address_type_ipv4;
            source_address_entry.address.ipv4_address = src_addr_field->shr_var_uint32;
            break;
        case DBAL_FIELD_IPV6_SIP:
            source_address_entry.address_type = source_address_type_ipv6;
            sal_memcpy(&source_address_entry.address.ipv6_address, &(src_addr_field->shr_var_uint8_arr),
                       sizeof(bcm_ip6_t));
            break;
        case DBAL_FIELD_SOURCE_ADDRESS:
            source_address_entry.address_type = source_address_type_full_mac;
            sal_memcpy(source_address_entry.address.mac_address, &(src_addr_field->shr_var_uint8_arr),
                       sizeof(bcm_mac_t));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported src addr profile field %s",
                         dbal_field_to_string(unit, dbal_field_id));
            break;
    }

    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.source_address_table.profile_get
                    (unit, (void *) &source_address_entry, profile));

exit:
    SHR_FUNC_EXIT;
}

/* Internal src address profile test function */
static shr_error_e
ctest_flow_src_address_test(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    const flow_special_field_info_t * special_field_info,
    flow_special_fields_e special_field_id,
    bcm_flow_handle_info_t * flow_handle_info)
{
    int common_field_idx, num_common_fields;
    uint32 field_enabler = 0;
    const dnx_flow_common_field_desc_t *field_desc = NULL;
    bcm_flow_initiator_info_t initiator_info = { 0 }, initiator_info_2 =
    {
    0};
    bcm_flow_special_fields_t special_fields = { 0 };
    bcm_flow_special_fields_t special_fields_get = { 0 };
    uint8 test_failed = TRUE;
    bcm_flow_handle_info_t flow_handle_info_2 = { 0 };
    bcm_flow_special_field_t set_value, get_value, tunnel_set_value;
    int rv, ref_count, first_ref_count, profile, common_field_value;
    source_address_entry_t source_address_entry;
    const flow_special_field_info_t *tunnel_type_special_field_info;
    int l3_intf_id_value;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Step 1 - add entry 
     */
    initiator_info.encap_access =
        (flow_app_info->encap_access_default_mapping !=
         bcmEncapAccessInvalid) ? flow_app_info->encap_access_default_mapping : bcmEncapAccessTunnel2;

    /*
     * Set src address value on field 
     */
    special_fields.actual_nof_special_fields = 1;
    special_fields.special_fields[0].field_id = special_field_id;
    SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_legal_value_get(unit, flow_app_info, 0, special_field_id, &set_value));
    CTEST_DNX_FLOW_SPECIAL_FIELD_SET(special_field_info, set_value, 0);

    if (FLOW_LIF_DBAL_TABLE_GET(flow_app_info) == DBAL_TABLE_EEDB_IPV4_TUNNEL)
    {
        /*
         * For IPV4 - tunnel type is a mandatory special field 
         */
        special_fields.actual_nof_special_fields++;
        special_fields.special_fields[1].field_id = FLOW_S_F_IPV4_TUNNEL_TYPE;
        SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_legal_value_get
                        (unit, flow_app_info, 1, FLOW_S_F_IPV4_TUNNEL_TYPE, &tunnel_set_value));
        SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, FLOW_S_F_IPV4_TUNNEL_TYPE, &tunnel_type_special_field_info));
        CTEST_DNX_FLOW_SPECIAL_FIELD_SET(tunnel_type_special_field_info, tunnel_set_value, 1);
    }
    else if (FLOW_LIF_DBAL_TABLE_GET(flow_app_info) == DBAL_TABLE_EEDB_IPV6_TUNNEL)
    {
        /*
         * For IPV6 - tunnel type and dip are mandatory special fields, l3_intf_id is a common mandatory field 
         */
        special_fields.actual_nof_special_fields = 3;
        special_fields.special_fields[1].field_id = FLOW_S_F_IPV6_TUNNEL_TYPE;
        SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_legal_value_get
                        (unit, flow_app_info, 1, FLOW_S_F_IPV6_TUNNEL_TYPE, &tunnel_set_value));
        SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, FLOW_S_F_IPV6_TUNNEL_TYPE, &tunnel_type_special_field_info));
        CTEST_DNX_FLOW_SPECIAL_FIELD_SET(tunnel_type_special_field_info, tunnel_set_value, 1);
        special_fields.special_fields[2].field_id = FLOW_S_F_IPV6_DIP;
        SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_legal_value_get
                        (unit, flow_app_info, 1, FLOW_S_F_IPV6_DIP, &tunnel_set_value));
        SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, FLOW_S_F_IPV6_TUNNEL_TYPE, &tunnel_type_special_field_info));
        CTEST_DNX_FLOW_SPECIAL_FIELD_SET(tunnel_type_special_field_info, tunnel_set_value, 2);

        SHR_IF_ERR_EXIT(ctest_flow_ut_common_field_legal_value_get
                        (unit, flow_app_info, 0, BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID, &l3_intf_id_value));

        SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                        (unit, flow_app_info->flow_app_type, BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID,
                         &initiator_info, l3_intf_id_value));

        initiator_info.valid_elements_set = BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID;
    }

    SHR_IF_ERR_EXIT(bcm_flow_initiator_info_create(unit, flow_handle_info, &initiator_info, &special_fields));

    /*
     * Step 2 - get, verify entry 
     */
    SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get(unit, flow_handle_info, &initiator_info, &special_fields_get));

    SHR_IF_ERR_EXIT(ctest_flow_special_field_value_get(unit, &special_fields, special_field_id, &get_value));

    SHR_IF_ERR_EXIT(ctest_flow_special_field_validate_value(unit, special_field_info, &get_value, &set_value));

    /*
     * Get the profile idx (will use it later) 
     */
    SHR_IF_ERR_EXIT(ctest_flow_src_addr_profile_get(unit, &set_value, special_field_info->mapped_dbal_field, &profile));

    /*
     * Get src addr reference count
     */
    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.source_address_table.profile_data_get
                    (unit, profile, &first_ref_count, (void *) &source_address_entry));

    /*
     * Step 3 - Replace value, increment value 
     */
    if (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_UINT32)
    {
        set_value.shr_var_uint32++;
    }
    else if (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_UINT32_ARR)
    {
        set_value.shr_var_uint32_arr[0]++;
    }
    else if (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_UINT8_ARR)
    {
        set_value.shr_var_uint8_arr[0]++;
    }

    CTEST_DNX_FLOW_SPECIAL_FIELD_SET(special_field_info, set_value, 0);

    flow_handle_info->flags |= BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    initiator_info.encap_access = bcmEncapAccessInvalid;
    special_fields.actual_nof_special_fields = 1;
    SHR_IF_ERR_EXIT(bcm_flow_initiator_info_create(unit, flow_handle_info, &initiator_info, &special_fields));

    /*
     * Step 4 - get, verify entry 
     */
    flow_handle_info->flags = 0;
    SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get(unit, flow_handle_info, &initiator_info, &special_fields_get));

    SHR_IF_ERR_EXIT(ctest_flow_special_field_value_get(unit, &special_fields, special_field_id, &get_value));

    SHR_IF_ERR_EXIT(ctest_flow_special_field_validate_value(unit, special_field_info, &get_value, &set_value));

    /*
     * Get src addr reference count
     */
    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.source_address_table.profile_data_get
                    (unit, profile, &ref_count, (void *) &source_address_entry));

    if (ref_count != first_ref_count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Profile ref count error, first was %d, after replace - %d", first_ref_count,
                     ref_count);
    }

    /*
     * Step 5 - replace without field, expect to keep the previously set value
     */
    /*
     * First - find the first supported non-profile common field to set
     */
    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_info->flow_app_type, &num_common_fields));

    for (common_field_idx = 0; common_field_idx < num_common_fields; common_field_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                        (unit, flow_app_info->flow_app_type, common_field_idx, &field_desc));

        if (field_desc == NULL)
        {
            continue;
        }

        field_enabler = field_desc->term_field_enabler;
        if ((field_desc->is_profile == TRUE)
            || (!_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field_enabler)))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(ctest_flow_ut_common_field_legal_value_get
                        (unit, flow_app_info, 0, (int) field_enabler, &common_field_value));
        if (common_field_value == CTEST_FLOW_SKIP_FIELD)
        {
            continue;
        }

        initiator_info.valid_elements_set |= field_enabler;

        SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                        (unit, flow_app_info->flow_app_type, field_enabler, (void *) &initiator_info,
                         common_field_value));
        break;
    }

    if (common_field_idx == num_common_fields)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No supported common fields for app, can't run step");
    }

    special_fields.actual_nof_special_fields = 0;
    initiator_info.encap_access = bcmEncapAccessInvalid;
    flow_handle_info->flags |= BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    SHR_IF_ERR_EXIT(bcm_flow_initiator_info_create(unit, flow_handle_info, &initiator_info, &special_fields));

    /*
     * Step 6 - get, verify entry and profile
     */
    flow_handle_info->flags = 0;
    SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get(unit, flow_handle_info, &initiator_info, &special_fields_get));

    if (special_fields_get.actual_nof_special_fields == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Didn't get the special fields back after replace");
    }
    SHR_IF_ERR_EXIT(ctest_flow_special_field_value_get(unit, &special_fields_get, special_field_id, &get_value));

    SHR_IF_ERR_EXIT(ctest_flow_special_field_validate_value(unit, special_field_info, &get_value, &set_value));

    /*
     * Get src addr reference count
     */
    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.source_address_table.profile_data_get
                    (unit, profile, &ref_count, (void *) &source_address_entry));

    if (ref_count != first_ref_count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Profile ref count error - was %d, after replace %d", first_ref_count, ref_count);
    }

    SHR_IF_ERR_EXIT(ctest_flow_special_field_validate_value(unit, special_field_info, &get_value, &set_value));

    /*
     * Step 7 - add another entry with the same profile 
     */
    flow_handle_info_2.flow_handle = flow_handle_info->flow_handle;
    initiator_info_2.encap_access =
        (flow_app_info->encap_access_default_mapping !=
         bcmEncapAccessInvalid) ? flow_app_info->encap_access_default_mapping : bcmEncapAccessTunnel2;
    initiator_info_2.valid_elements_set |= field_enabler;

    SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                    (unit, flow_app_info->flow_app_type, field_enabler, (void *) &initiator_info,
                     (common_field_value + 1)));

    special_fields.actual_nof_special_fields = 1;
    if (FLOW_LIF_DBAL_TABLE_GET(flow_app_info) == DBAL_TABLE_EEDB_IPV4_TUNNEL)
    {
        /*
         * For IPV4 - we also need to take the tunnel type, set before 
         */
        special_fields.actual_nof_special_fields = 2;
    }
    else if (FLOW_LIF_DBAL_TABLE_GET(flow_app_info) == DBAL_TABLE_EEDB_IPV6_TUNNEL)
    {
        /*
         * For IPV6 - we also need to take the tunnel type and dip special fields, l3_intf_id, set before 
         */
        special_fields.actual_nof_special_fields = 3;
        SHR_IF_ERR_EXIT(ctest_flow_set_by_enabler
                        (unit, flow_app_info->flow_app_type, BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID,
                         &initiator_info_2, l3_intf_id_value));

        initiator_info_2.valid_elements_set = BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID;

    }
    SHR_IF_ERR_EXIT(bcm_flow_initiator_info_create(unit, &flow_handle_info_2, &initiator_info_2, &special_fields));

    /*
     * Step 8 - validate reference count 
     */
    /*
     * Get src addr reference count
     */
    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.source_address_table.profile_data_get
                    (unit, profile, &ref_count, (void *) &source_address_entry));

    if (ref_count != (first_ref_count + 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Profile ref count error, expected %d, got %d", (first_ref_count + 1), ref_count);
    }

    /*
     * Step 9 - delete second entry, expect ref count -1
     */
    SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info_2, NULL));
    flow_handle_info_2.flow_id = 0;

    /*
     * Get src addr reference count
     */
    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.source_address_table.profile_data_get
                    (unit, profile, &ref_count, (void *) &source_address_entry));

    if (ref_count != first_ref_count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Profile ref count error, expected %d, got %d", first_ref_count, ref_count);
    }

    /*
     * Step 10 - delete first entry, validate ref count
     */
    SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, flow_handle_info, NULL));
    flow_handle_info->flow_id = 0;
    /*
     * Get src addr reference count
     */
    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.source_address_table.profile_data_get
                    (unit, profile, &ref_count, (void *) &source_address_entry));

    if (ref_count != (first_ref_count - 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Profile ref count error, expected %d, got %d", (first_ref_count - 1), ref_count);
    }

    test_failed = FALSE;

exit:
    if (flow_handle_info->flow_id)
    {
        rv = ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, flow_handle_info, NULL);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("Failed removing flow entry at cleanup\n")));
            test_failed = TRUE;
        }
    }

    if (flow_handle_info_2.flow_id)
    {
        rv = ctest_flow_entry_delete(unit, flow_app_info->flow_app_type, &flow_handle_info_2, NULL);
        if (rv != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META("Failed removing flow entry at cleanup\n")));
            test_failed = TRUE;
        }
    }

    if (test_failed)
    {
        LOG_CLI((BSL_META("src address test FAILED\n")));
    }
    else
    {
        LOG_CLI((BSL_META("src address test PASSED\n")));
    }

    SHR_FUNC_EXIT;
}

/*
 * Validate correct behavior of src address in applications which support it
 */
static shr_error_e
ctest_flow_src_address_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    const dnx_flow_app_config_t *flow_app_info = NULL;
    int field_idx = 0;
    const flow_special_field_info_t *special_field_info;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_field_id_t special_field_id = FLOW_S_F_EMPTY;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_WITH_EXCEPTION(ctest_flow_test_begin
                                   (unit, args, sand_control, FLOW_ALLOWED_APP_TYPES_INIT, 0, &flow_app_info,
                                    &flow_handle_info), _SHR_E_PARAM);

    if (!dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Test is only for applications which support src addr profile (application: %s)",
                     flow_app_info->app_name);
    }

    /*
     * Get src address field position
     */
    while (flow_app_info->special_fields[field_idx] != FLOW_S_F_EMPTY)
    {
        SHR_IF_ERR_EXIT(flow_special_field_info_get
                        (unit, flow_app_info->special_fields[field_idx], &special_field_info));
        if (dnx_flow_special_field_is_ind_set
            (unit, special_field_info, FLOW_SPECIAL_FIELD_IND_SRC_ADDR_PROFILE_SUPPORTED))
        {
            special_field_id = flow_app_info->special_fields[field_idx];
            break;
        }
        field_idx++;
    }

    if (flow_app_info->special_fields[field_idx] == FLOW_S_F_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Application %s has src address indication set but no related src address field",
                     flow_app_info->app_name);
    }

    SHR_IF_ERR_EXIT(ctest_flow_src_address_test
                    (unit, flow_app_info, special_field_info, special_field_id, &flow_handle_info));

exit:
    SHR_FUNC_EXIT;
}

/******************************************************************************************************
 *                                              SRC_ADDRESS_TEST END
 ******************************************************************************************************/

static shr_error_e
ctest_flow_egress_last_layer_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    const dnx_flow_app_config_t *flow_app_info = NULL;
    int rv;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_initiator_info_t initiator_info = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };
    bsl_severity_t orig_flow_severity = bslenable_get(bslLayerBcmdnx, bslSourceFlow);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_WITH_EXCEPTION(ctest_flow_test_begin
                                   (unit, args, sand_control, FLOW_ALLOWED_APP_TYPES_INIT, BCM_FLOW_HANDLE_NATIVE,
                                    &flow_app_info, &flow_handle_info), _SHR_E_PARAM);

    flow_handle_info.flags |= BCM_FLOW_HANDLE_NATIVE;
    SHR_IF_ERR_EXIT(ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, &initiator_info, NULL, 1, 0));
    flow_handle_info.flags = 0;

    LOG_CLI((BSL_META("Entry created with BCM_FLOW_HANDLE_NATIVE flag\n")));

    SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get(unit, &flow_handle_info, &initiator_info, &special_fields));

    if (!_SHR_IS_FLAG_SET(flow_handle_info.flags, BCM_FLOW_HANDLE_NATIVE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Entry get didn't return BCM_FLOW_HANDLE_NATIVE flag set %x\n",
                     flow_handle_info.flags);
    }

    LOG_CLI((BSL_META("Entry get returned BCM_FLOW_HANDLE_NATIVE flag set\n")));

    flow_handle_info.flags = 0;

    flow_handle_info.flags |= BCM_FLOW_HANDLE_NATIVE | BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

    bslenable_set(bslLayerBcmdnx, bslSourceFlow, bslSeverityOff);
    rv = ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, &initiator_info, NULL, 1, 0);
    bslenable_set(bslLayerBcmdnx, bslSourceFlow, orig_flow_severity);

    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Entry replace didn't returned correct error (%d) when setting BCM_FLOW_HANDLE_NATIVE flag ", rv);
    }

    LOG_CLI((BSL_META("Negative test with Replace Passed \n")));

    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    SHR_IF_ERR_EXIT(ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, &initiator_info, NULL, 1, 0));

    LOG_CLI((BSL_META("Entry Replaced \n")));

    flow_handle_info.flags = 0;

    SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get(unit, &flow_handle_info, &initiator_info, &special_fields));

    if (!_SHR_IS_FLAG_SET(flow_handle_info.flags, BCM_FLOW_HANDLE_NATIVE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Entry get didn't return BCM_FLOW_HANDLE_NATIVE flag set %x\n",
                     flow_handle_info.flags);
    }

    LOG_CLI((BSL_META("Entry get returned BCM_FLOW_HANDLE_NATIVE flag set\n")));

    flow_handle_info.flags = 0;
    SHR_IF_ERR_EXIT(bcm_flow_initiator_info_destroy(unit, &flow_handle_info));

    LOG_CLI((BSL_META("Entry created without BCM_FLOW_HANDLE_NATIVE flag\n")));

    SHR_IF_ERR_EXIT(ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, &initiator_info, NULL, 1, 0));
    flow_handle_info.flags = 0;

    SHR_IF_ERR_EXIT(bcm_flow_initiator_info_get(unit, &flow_handle_info, &initiator_info, &special_fields));

    if (_SHR_IS_FLAG_SET(flow_handle_info.flags, BCM_FLOW_HANDLE_NATIVE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Entry get didn't returned BCM_FLOW_HANDLE_NATIVE flag set %x\n",
                     flow_handle_info.flags);
    }

    LOG_CLI((BSL_META("Entry get returned correct flag set\n")));

    LOG_CLI((BSL_META("Test PASSED\n")));

exit:
    SHR_FUNC_EXIT;
}

/** test add 3 entris, delete  entries using traverse and verify that it was deleted */
static shr_error_e
ctest_flow_lif_delete_all_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    const dnx_flow_app_config_t *flow_app_info = NULL;
    int rv;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_initiator_info_t initiator_info = { 0 };
    bcm_flow_terminator_info_t terminator_info = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };
    uint32 flow_ids[3] = { 0, 0, 0 };
    void *app_data;
    int ii;
    bsl_severity_t lif_severity = bslSeverityOff, dbal_severity = bslSeverityOff, flow_severity =
        bslSeverityOff, dbal_soc_severity = bslSeverityOff, port_severity = bslSeverityOff, qos_severity =
        bslSeverityOff;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_WITH_EXCEPTION(ctest_flow_test_begin
                                   (unit, args, sand_control, FLOW_ALLOWED_APP_TYPES_LIF, 0, &flow_app_info,
                                    &flow_handle_info), _SHR_E_PARAM);

    app_data =
        (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) ? (void *) &terminator_info : (void *) &initiator_info;

    for (ii = 0; ii < 3; ii++)
    {
        SHR_IF_ERR_EXIT(ctest_flow_lif_info_create(unit, flow_app_info, &flow_handle_info, app_data, NULL, 1, 1));
        flow_ids[ii] = flow_handle_info.flow_id;
        LOG_CLI((BSL_META("Entries %d created IDs %d\n"), ii, flow_handle_info.flow_id));
    }

    flow_handle_info.flags = BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL;
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_traverse
                        (unit, &flow_handle_info, ctest_flow_term_delete_callback, NULL));
    }
    else        /* FLOW_APP_TYPE_INIT */
    {
        SHR_IF_ERR_EXIT(bcm_flow_initiator_info_traverse
                        (unit, &flow_handle_info, ctest_flow_init_delete_callback, NULL));
    }
    LOG_CLI((BSL_META("3 entries deleted using traverse \n")));

    flow_handle_info.flags = 0;

    SHR_IF_ERR_EXIT(ctest_flow_logger_close
                    (unit, &lif_severity, &dbal_severity, &dbal_soc_severity, &flow_severity, &port_severity,
                     &qos_severity));

    for (ii = 0; ii < 3; ii++)
    {
        flow_handle_info.flow_id = flow_ids[ii];
        rv = ctest_flow_lif_info_get(unit, flow_app_info, &flow_handle_info, app_data, &special_fields);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Entry %d found when expected to be deleted flow_id %d\n", ii,
                         flow_handle_info.flow_id);
        }
    }

    SHR_IF_ERR_EXIT(ctest_flow_logger_restore
                    (unit, lif_severity, dbal_severity, dbal_soc_severity, flow_severity, port_severity, qos_severity));

    LOG_CLI((BSL_META("Delete all validated, entries not found\n")));

    LOG_CLI((BSL_META("Test PASSED\n")));

exit:
    SHR_FUNC_EXIT;
}

/** test add 3 entris, delete  entries using traverse and verify that it was deleted */
static shr_error_e
ctest_flow_lif_per_core_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    const dnx_flow_app_config_t *flow_app_info = NULL;
    int rv, ii;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_terminator_info_t terminator_info = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };
    uint32 flow_ids[3] = { 0, 0, 0 };
    uint32 common_fields_bm[3] = {
        BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID,
        BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID,
        BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID
    };
    uint32 common_fields_bm_replace[3] = {
        BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID,
        BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID,
        BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID
    };

    bsl_severity_t lif_severity = bslSeverityOff, dbal_severity = bslSeverityOff, flow_severity =
        bslSeverityOff, dbal_soc_severity = bslSeverityOff, port_severity = bslSeverityOff, qos_severity =
        bslSeverityOff;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
        LOG_CLI((BSL_META("test not supported when dnx_data_lif_in_lif_phy_db_dpc is not set \n")));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT_WITH_EXCEPTION(ctest_flow_test_begin
                                   (unit, args, sand_control, FLOW_ALLOWED_APP_TYPES_LIF,
                                    BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM, &flow_app_info, &flow_handle_info),
                                   _SHR_E_PARAM) LOG_CLI((BSL_META("Negative test - use core bm without flag\n")));

    SHR_IF_ERR_EXIT(ctest_flow_logger_close
                    (unit, &lif_severity, &dbal_severity, &dbal_soc_severity, &flow_severity, &port_severity,
                     &qos_severity));

    terminator_info.valid_elements_set = common_fields_bm[0];
    flow_handle_info.core_bitmap = 3;
    rv = bcm_flow_terminator_info_create(unit, &flow_handle_info, &terminator_info, &special_fields);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Entry with core_bitmap set was added when expected to fail with E_PARAM rv = %d\n", rv);

    }

    SHR_IF_ERR_EXIT(ctest_flow_logger_restore
                    (unit, lif_severity, dbal_severity, dbal_soc_severity, flow_severity, port_severity, qos_severity));

    LOG_CLI((BSL_META("Negative test PASS\n")));

    LOG_CLI((BSL_META("Adding 3 entries \n")));

    for (ii = 0; ii < 3; ii++)
    {
        terminator_info.valid_elements_set = common_fields_bm[ii];
        flow_handle_info.core_bitmap = ii + 1;
        flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM;

        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_create(unit, &flow_handle_info, &terminator_info, &special_fields));
        flow_ids[ii] = flow_handle_info.flow_id;
        LOG_CLI((BSL_META("\tEntry %d created IDs %d, core_bm %x\n"), ii, flow_ids[ii], flow_handle_info.core_bitmap));
    }

    LOG_CLI((BSL_META("3 entries added successfully\n")));

    LOG_CLI((BSL_META("Getting the entries and values\n")));

    for (ii = 0; ii < 3; ii++)
    {
        flow_handle_info.flow_id = flow_ids[ii];
        flow_handle_info.flags = 0;
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_get(unit, &flow_handle_info, &terminator_info, &special_fields));

        if (terminator_info.valid_elements_set != common_fields_bm[ii])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong valid_elements_set expected %x received %x\n", common_fields_bm[ii],
                         terminator_info.valid_elements_set);
        }
    }

    LOG_CLI((BSL_META("Getting entries verified\n")));

    LOG_CLI((BSL_META("Replacing the 2nd entry \n")));

    flow_handle_info.core_bitmap = 0;
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

    terminator_info.valid_elements_set = BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID;
    terminator_info.valid_elements_clear = common_fields_bm[1];
    flow_handle_info.flow_id = flow_ids[1];
    SHR_IF_ERR_EXIT(bcm_flow_terminator_info_create(unit, &flow_handle_info, &terminator_info, &special_fields));
    LOG_CLI((BSL_META("\tEntry %d replaced IDs %d\n"), ii, flow_handle_info.flow_id));

    LOG_CLI((BSL_META("Getting the entries and values\n")));

    for (ii = 0; ii < 3; ii++)
    {
        flow_handle_info.flow_id = flow_ids[ii];
        flow_handle_info.flags = 0;
        SHR_IF_ERR_EXIT(bcm_flow_terminator_info_get(unit, &flow_handle_info, &terminator_info, &special_fields));

        if (terminator_info.valid_elements_set != common_fields_bm_replace[ii])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Entry %d, Wrong valid_elements_set expected %x received %x\n", ii,
                         common_fields_bm_replace[ii], terminator_info.valid_elements_set);
        }
    }

    LOG_CLI((BSL_META("Getting entries verified\n")));

    /** this part is supported only if the lif app is VLAN_PORT_LL_TERMINATOR */
    if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, FLOW_STR_MAX))
    {
        bcm_flow_handle_info_t flow_match_handle_info = { 0 };

        SHR_IF_ERR_EXIT(diag_flow_app_from_string(unit, FLOW_APP_NAME_VLAN_PORT_LL_AC_S_VLAN_TERM_MATCH,
                                                  FLOW_APP_TYPE_TERM_MATCH, &flow_match_handle_info.flow_handle,
                                                  sand_control));

        special_fields.actual_nof_special_fields = 2;
        special_fields.special_fields[0].field_id = FLOW_S_F_S_VID;
        special_fields.special_fields[0].shr_var_uint32 = 1;
        special_fields.special_fields[1].field_id = FLOW_S_F_VLAN_DOMAIN;
        special_fields.special_fields[1].shr_var_uint32 = 1;

        flow_match_handle_info.flow_id = flow_ids[0];

        LOG_CLI((BSL_META("Adding Match entry \n")));

        SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_add(unit, &flow_match_handle_info, &special_fields));

        flow_match_handle_info.flow_id = 0;

        LOG_CLI((BSL_META("Getting the match entry \n")));
        rv = bcm_dnx_flow_match_info_get(unit, &flow_match_handle_info, &special_fields);

        if (rv != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not find the entry rv = %d\n", rv);
        }

        if (flow_match_handle_info.flow_id != flow_ids[0])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong flow_id received %x, expected %x \n", flow_match_handle_info.flow_id,
                         flow_ids[0]);
        }

        LOG_CLI((BSL_META("Getting entry verified \n")));

        LOG_CLI((BSL_META("Deleting the match entry \n")));

        flow_match_handle_info.flow_id = 0;
        rv = bcm_dnx_flow_match_info_delete(unit, &flow_match_handle_info, &special_fields);

        if (rv != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not delete the entry rv = %d\n", rv);
        }
    }

    flow_handle_info.flags = 0;
    flow_handle_info.flow_id = flow_ids[0];
    SHR_IF_ERR_EXIT(bcm_flow_terminator_info_destroy(unit, &flow_handle_info));
    LOG_CLI((BSL_META("Entry 0 IDs %d deleted by destroy API\n"), flow_ids[0]));

    /** setting the flow IDs that the delete callback will know that those entries should be deleted */
    basic_test_flow_ids[0] = flow_ids[1];
    basic_test_flow_ids[1] = flow_ids[2];
    SHR_IF_ERR_EXIT(bcm_flow_terminator_info_traverse
                    (unit, &flow_handle_info, ctest_flow_term_delete_callback, &flow_handle_info));

    LOG_CLI((BSL_META("Rest of the entries deleted by iterator \n")));

    LOG_CLI((BSL_META("Test PASSED\n")));

exit:
    SHR_FUNC_EXIT;
}

/** test sequence:
 * adding LIF related entry
 * adding regular match entry
 * adding BUD match entry
 * traversing the entries - validate that we receive only one BUD entry
 * deleting all entries using traverse
 * validating no entries using get
 **/
static shr_error_e
ctest_flow_lif_bud_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    const dnx_flow_app_config_t *flow_app_info = NULL;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_special_fields_t key_special_fields = { 0 };
    int ii, rv;
    bcm_flow_handle_info_t lif_handle_info = { 0 };
    int nof_bud_entries = 0;

    bcm_gport_t flow_id;
    bcm_flow_handle_t related_lif_app_handle_id;

    bsl_severity_t lif_severity = bslSeverityOff, dbal_severity = bslSeverityOff, flow_severity =
        bslSeverityOff, dbal_soc_severity = bslSeverityOff, port_severity = bslSeverityOff, qos_severity =
        bslSeverityOff;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_WITH_EXCEPTION(ctest_flow_test_begin
                                   (unit, args, sand_control, FLOW_ALLOWED_APP_TYPES_MATCH, BCM_FLOW_HANDLE_INFO_BUD,
                                    &flow_app_info, &flow_handle_info), _SHR_E_PARAM)
        rv = ctest_flow_match_related_lif_entry_add(unit, flow_app_info, &related_lif_app_handle_id, &flow_id);

    if (rv == _SHR_E_CONFIG)
    {
        /** unable to add LIF entry, exiting the test without an error */
        SHR_EXIT();
    }

    flow_handle_info.flags = BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL;

    LOG_CLI((BSL_META("Deleting all entries in app\n")));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_traverse(unit, &flow_handle_info, ctest_flow_match_callback, NULL));

    flow_handle_info.flow_id = flow_id;

    lif_handle_info.flow_id = flow_id;
    lif_handle_info.flow_handle = related_lif_app_handle_id;

    for (ii = 0; ii < BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS; ii++)
    {
        if (flow_app_info->special_fields[ii] == FLOW_S_F_EMPTY)
        {
            break;
        }

        SHR_IF_ERR_EXIT(ctest_flow_ut_special_field_legal_value_get
                        (unit, flow_app_info, 0, flow_app_info->special_fields[ii],
                         &key_special_fields.special_fields[ii]));

        if (key_special_fields.special_fields[ii].shr_var_uint32 == CTEST_FLOW_SKIP_FIELD)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "NO legal value for field %s, cannot perform operation\n",
                         dnx_flow_special_field_to_string(unit, flow_app_info->special_fields[ii]));
        }

        key_special_fields.special_fields[ii].field_id = flow_app_info->special_fields[ii];

        key_special_fields.actual_nof_special_fields++;
    }

    SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_add(unit, &flow_handle_info, &key_special_fields));

    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_BUD;
    SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_add(unit, &flow_handle_info, &key_special_fields));

    flow_handle_info.flags = 0;
    SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_traverse
                    (unit, &flow_handle_info, ctest_flow_match_callback, &nof_bud_entries));

    if (nof_bud_entries != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "expected one BUD entry to be found %d\n", nof_bud_entries);
    }

    flow_handle_info.flags = BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL;

    LOG_CLI((BSL_META("Deleting entries using traverse \n")));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_match_info_traverse(unit, &flow_handle_info, ctest_flow_match_callback, NULL));

    LOG_CLI((BSL_META("Entries deleted \n")));

    flow_handle_info.flags = 0;

    SHR_IF_ERR_EXIT(ctest_flow_logger_close
                    (unit, &lif_severity, &dbal_severity, &dbal_soc_severity, &flow_severity, &port_severity,
                     &qos_severity));

    rv = bcm_dnx_flow_match_info_get(unit, &flow_handle_info, &key_special_fields);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Regular Entry found when expected to be deleted \n");
    }

    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_BUD;
    rv = bcm_dnx_flow_match_info_get(unit, &flow_handle_info, &key_special_fields);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BUD Entry found when expected to be deleted \n");
    }

    SHR_IF_ERR_EXIT(ctest_flow_logger_restore
                    (unit, lif_severity, dbal_severity, dbal_soc_severity, flow_severity, port_severity, qos_severity));

    LOG_CLI((BSL_META("Delete all validated, entries not found\n")));

    SHR_IF_ERR_EXIT(ctest_flow_entry_delete(unit, FLOW_APP_TYPE_TERM, &lif_handle_info, NULL));

    LOG_CLI((BSL_META("Test PASSED\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief consist a mapping between the name of the test and the corresponding enum value. it is initialized dynamically
 *        according to the ut struct.
 */
static sh_sand_option_t dnx_flow_fields_unit_test_options[] = {
    {"application", SAL_FIELD_TYPE_STR, "Application to use", FLOW_APP_NAME_GTP_TERMINATOR},
    {"negative", SAL_FIELD_TYPE_BOOL, "Use illegal values, expect failure", "FALSE"},
    {"positive", SAL_FIELD_TYPE_BOOL, "Use illegal values, expect failure", "TRUE"},
    {NULL}
};

static sh_sand_option_t dnx_flow_basic_or_negative_test_options[] = {
    {"application", SAL_FIELD_TYPE_STR, "Application to use", FLOW_APP_NAME_GTP_TERMINATOR},
    {NULL}
};

static sh_sand_option_t dnx_flow_application_test_options[] = {
    {"application", SAL_FIELD_TYPE_STR, "Application to use", FLOW_APP_NAME_IPV4_INITIATOR},
    {NULL}
};

static sh_sand_option_t dnx_flow_egress_last_layer_test_options[] = {
    {"application", SAL_FIELD_TYPE_STR, "Application to use", FLOW_APP_NAME_VLAN_PORT_LL_INITIATOR},
    {NULL}
};

static sh_sand_option_t dnx_flow_lif_per_core_test_options[] = {
    {"application", SAL_FIELD_TYPE_STR, "Application to use", FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},
    {NULL}
};

static sh_sand_option_t dnx_flow_bud_test_options[] = {
    {"application", SAL_FIELD_TYPE_STR, "Application to use", FLOW_APP_NAME_MPLS_TUNNEL_LABEL_TERM_MATCH},
    {NULL}
};

static sh_sand_man_t dnx_flow_fields_unit_test_man = {
    "Try to set illegal or legal values to common fields."
};

static sh_sand_man_t dnx_flow_basic_test_man = {
    "Validate set, get, replace, traverse, delete APIs."
};

static sh_sand_man_t dnx_flow_negative_test_man = {
    "General negative test, illegal parameters and scenarios."
};

static sh_sand_man_t dnx_flow_app_fields_test_man = {
    "Special field tests, validate that the special fields are defined correctly."
};

static sh_sand_man_t dnx_flow_common_profiles_test_man = {
    "General common profiles test. Validate profiles, add, replace, delete etc."
};

static sh_sand_man_t dnx_flow_egress_qos_model_test_man = {
    "Validate egress qos model functionality."
};

static sh_sand_man_t dnx_flow_special_fields_replace_test_man = {
    "Validate correct behavior of is_clear attribute and replace functioning in special fields."
};

static sh_sand_man_t dnx_flow_common_fields_test_man = {
    "Validate correct order of common fields in flow configuration files."
};

static sh_sand_man_t dnx_flow_src_address_test_man = {
    "Validate correct behavior of src address profile special fields"
};

static sh_sand_man_t dnx_flow_egress_last_layer_test_man = {
    "Validate correct behavior of egress last layer flag (semantic)"
};

static sh_sand_man_t dnx_flow_lif_delete_all_test_man = {
    "Validate correct behavior of delete all (semantic)"
};

static sh_sand_man_t dnx_flow_lif_per_core_test_man = {
    "Validate correct behavior of LIF per core support (semantic)"
};

static sh_sand_man_t dnx_flow_bud_test_man = {
    "Validate correct behavior of second pass aka BUD (semantic)"
};

static sh_sand_enum_t ctest_flow_app_type_enum_table[] = {
    {"terminator", FLOW_APP_TYPE_TERM, "an application that represents a LIF Terminator"},
    {"initiator", FLOW_APP_TYPE_INIT, "an application that represents a LIF Initiator"},
    {"term-match", FLOW_APP_TYPE_TERM_MATCH, "an application that represents a Terminator LIF match"},
    {"init-match", FLOW_APP_TYPE_INIT_MATCH, "an application that represents an Initiator LIF match"},
    {"all", FLOW_APP_TYPE_NOF_TYPES, "default - any application type"},
    {NULL}
};
static sh_sand_option_t dnx_flow_ut_options[] = {
    {"Application", SAL_FIELD_TYPE_STR, "Flow Application name", ""},
    {"Type", SAL_FIELD_TYPE_ENUM, "Application Type", "ALL", (void *) ctest_flow_app_type_enum_table},
    {"Mode", SAL_FIELD_TYPE_BOOL, "When true, run the test without entries (validate definitions)", "FALSE"},
    {NULL}
};

static sh_sand_man_t dnx_flow_ut_test_man = {
    "Flow application Unit test - unit test for each existing Application that performs set / get /replace /delete",
    "ctest flow ut Application=<app_name>",
    "ctest flow ut Application=IPV4_TUNNEL_INITIATOR",
    "ctest flow ut Application=IPV4_TUNNEL_INITIATOR mode=TRUE"
};

/* *INDENT-OFF* */
sh_sand_cmd_t dnx_flow_test_cmds[] = {
    /*************************************************************************************************************************************
    *   CMD_NAME    *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *               *                           * Level                   *                                *                            *
    *               *                           * CMD                     *                                *                            *
    *************************************************************************************************************************************/
    {"fields",                   ctest_flow_fields_test_cmd,                    NULL, dnx_flow_fields_unit_test_options,            &dnx_flow_fields_unit_test_man, NULL, NULL, CTEST_POSTCOMMIT, ctest_flow_common_fields_unit_test_create},
    {"basic",                    ctest_flow_basic_test_cmd,                     NULL, dnx_flow_basic_or_negative_test_options,      &dnx_flow_basic_test_man,       NULL, NULL, CTEST_POSTCOMMIT, ctest_flow_basic_test_create},
    {"negative",                 ctest_flow_negative_test_cmd,                  NULL, dnx_flow_basic_or_negative_test_options,      &dnx_flow_negative_test_man,    NULL, NULL, CTEST_POSTCOMMIT, ctest_flow_negative_test_create},
    {"special_fields",           ctest_flow_special_fields_test_cmd,            NULL, NULL,                                         &dnx_flow_app_fields_test_man,  NULL, NULL, CTEST_PRECOMMIT, NULL},
    {"common_profiles",          ctest_flow_common_profiles_test_cmd,           NULL, dnx_flow_basic_or_negative_test_options,      &dnx_flow_common_profiles_test_man, NULL, NULL, CTEST_POSTCOMMIT, ctest_flow_common_profiles_test_create},
    {"egress_qos_model",         ctest_flow_egress_qos_model_test_cmd,          NULL, dnx_flow_application_test_options,            &dnx_flow_egress_qos_model_test_man, NULL, NULL, CTEST_POSTCOMMIT, ctest_flow_egress_qos_model_test_create},
    {"special_fields_replace",   ctest_flow_special_fields_replace_test_cmd,    NULL, dnx_flow_application_test_options,            &dnx_flow_special_fields_replace_test_man, NULL, NULL, CTEST_POSTCOMMIT, ctest_flow_special_fields_replace_test_create},
    {"common_fields",            ctest_flow_common_fields_test_cmd,             NULL, NULL,                                         &dnx_flow_common_fields_test_man,          NULL, NULL, CTEST_PRECOMMIT, NULL},
    {"src_address_profile",      ctest_flow_src_address_test_cmd,               NULL, dnx_flow_application_test_options,            &dnx_flow_src_address_test_man,         NULL, NULL, CTEST_POSTCOMMIT, ctest_flow_src_address_profile_test_create},
    {"Ut",                       cmd_dnx_flow_ut,                               NULL, dnx_flow_ut_options,                          &dnx_flow_ut_test_man,          NULL, NULL, CTEST_POSTCOMMIT, ctest_flow_ut_dyn_tests_creation},
    {"egress_last_layer",        ctest_flow_egress_last_layer_test_cmd,         NULL, dnx_flow_egress_last_layer_test_options,      &dnx_flow_egress_last_layer_test_man ,         NULL, NULL, CTEST_POSTCOMMIT, NULL},
    {"lif_delete_all",           ctest_flow_lif_delete_all_test_cmd,            NULL, dnx_flow_application_test_options,            &dnx_flow_lif_delete_all_test_man,  NULL, NULL, CTEST_POSTCOMMIT, ctest_flow_lif_delete_all_test_create},
    {"lif_per_core",             ctest_flow_lif_per_core_test_cmd,              NULL, dnx_flow_lif_per_core_test_options,           &dnx_flow_lif_per_core_test_man,  NULL, NULL, CTEST_POSTCOMMIT, ctest_flow_lif_per_core_test_create},
    {"BUD",                      ctest_flow_lif_bud_test_cmd,                   NULL, dnx_flow_bud_test_options,                    &dnx_flow_bud_test_man ,  NULL, NULL, CTEST_POSTCOMMIT, ctest_flow_bud_test_create},
    {NULL}
};

/* *INDENT-ON* */

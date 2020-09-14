/** \file diag_dnx_dbal.c
 *
 * Main diagnostics for dbal applications All CLI commands, that are related to DBAL, are gathered in this file.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDBALDNX

/*************
 * INCLUDES  *
 *************/
#include <sal/appl/sal.h>

#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <appl/diag/bslenable.h>
#include <soc/mcm/memregs.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/lif_mngr_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ecmp_access.h>

#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_dbal_api.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <sal/core/boot.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/ctest/dnxc/ctest_dnxc_system.h>

#include "src/appl/diag/dnx/dbal/diag_dnx_dbal_internal.h"
#include "ctest_dnx_dbal.h"
/*************
 * TYPEDEFS  *
 *************/

#define CTEST_DBAL_MAX_NOF_ENTRIES_FOR_PERFORMANCE_TEST                 32768

typedef struct
{
    uint32 key_fields[CTEST_DBAL_MAX_NOF_ENTRIES_FOR_PERFORMANCE_TEST][DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS];
    uint32 result_fields[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE];

} dbal_entries_array_t;

extern dbal_tables_e mdb_init_tables_list[];

extern shr_error_e diag_dnx_dbal_table_capacity_estimate(
    int unit,
    dbal_tables_e table_id,
    int result_type,
    int *estimated_nof_entries);

/* *INDENT-OFF* */
    /** this struct consist all dbal tests and related info for the tests. name should be unique and 1:1 mapped to enum
     *  value */

static sh_sand_enum_t dbal_ut_enum_table_dynamic[DBAL_UT_NOF_TESTS] = { {0} };

dbal_ut_test_info_t dnx_dbal_ut_all_tests[] = {
/**     test name                       test enum                    test flags              test callback         */
    {"update_before_commit",        UPDATE_BEFORE_COMMIT,         CTEST_PRECOMMIT, dnx_dbal_ut_update_field_before_commit,   UT_ALL_DEVICES       , {DBAL_TABLE_INGRESS_PP_PORT, DBAL_TABLE_PEMLA_PARSERETH, DBAL_UT_FILL_TABLE_FOR_2}},
    {"handle_operations",           HANDLE_OPERATIONS,            CTEST_PRECOMMIT, dnx_dbal_ut_handle_operations,            UT_ALL_DEVICES       , {DBAL_TABLE_SCH_SE_CL_CONFIG, DBAL_UT_FILL_TABLE_FOR_1}},
    {"partial_access",              PARTIAL_ACCESS,               CTEST_PRECOMMIT, dnx_dbal_ut_partial_access,               UT_ALL_DEVICES       , {DBAL_TABLE_PEMLA_PARSERETH, DBAL_TABLE_EXAMPLE_SW_SUB_FIELD, DBAL_UT_FILL_TABLE_FOR_2}},
    {"group_validations",           GROUP_VALIDATIONS,            CTEST_PRECOMMIT, dnx_dbal_ut_group_definition_validate,    UT_ALL_DEVICES       , {DBAL_UT_NO_TABLE}},
    {"sub_field",                   SUB_FIELD,                    CTEST_PRECOMMIT, dnx_dbal_ut_parent_field_mapping,         UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_SW_SUB_FIELD, DBAL_UT_FILL_TABLE_FOR_1}},
    {"multiple_instances",          MULTIPLE_INSTANCES,           CTEST_PRECOMMIT, dnx_dbal_ut_multiple_instance_field,      UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_TABLE_FOR_MUL_INSTANCES, DBAL_UT_FILL_TABLE_FOR_1}},
    {"rt_access",                   RT_ACCESS,                    CTEST_PRECOMMIT, dnx_dbal_ut_rt_access,                    UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_SW_RESULT_TYPE, DBAL_UT_FILL_TABLE_FOR_1}},
    {"wrong_field_size",            WRONG_FIELD_SIZE,             CTEST_PRECOMMIT, dnx_dbal_ut_wrong_field_size,             UT_ALL_DEVICES       , {DBAL_TABLE_EGRESS_PP_PORT, DBAL_UT_FILL_TABLE_FOR_1}},
    {"wrong_field_access",          WRONG_FIELD_ACCESS,           CTEST_PRECOMMIT, dnx_dbal_ut_wrong_field_access,           UT_ALL_DEVICES       , {DBAL_TABLE_EGRESS_PP_PORT, DBAL_UT_FILL_TABLE_FOR_1}},
    {"superset_res_type",           SUPERSET_RES_TYPE,            CTEST_PRECOMMIT, dnx_dbal_ut_superset_res_type,            UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_SW_RESULT_TYPE, DBAL_TABLE_IN_LIF_IPvX_TUNNELS, DBAL_UT_FILL_TABLE_FOR_2}},
    {"enum_mapping",                ENUM_MAPPING,                 CTEST_PRECOMMIT, dnx_dbal_ut_enum_mapping,                 UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_SW_DIRECT, DBAL_UT_FILL_TABLE_FOR_1}},
    {"enum_mapping_mul_device",     ENUM_MAPPING_MUL_DEVICE,      CTEST_PRECOMMIT, dnx_dbal_ut_enum_mapping_multiple_device, UT_ALL_DEVICES       , {DBAL_TABLE_L2_DMA_DSP_COMMAND, DBAL_UT_FILL_TABLE_FOR_1}},
    {"range_of_entries_set",        RANGE_OF_ENTRIES_SET,         CTEST_PRECOMMIT, dnx_dbal_ut_range_of_entries_set,         UT_ALL_DEVICES       , {DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_MAPPING, DBAL_TABLE_EXAMPLE_SW_RANGE, DBAL_TABLE_FIELD_INGRESS_HEADER_PROFILE, DBAL_UT_FILL_TABLE_FOR_3}},
    {"range_entries_clear",         RANGE_OF_ENTRIES_CLEAR,       CTEST_PRECOMMIT, dnx_dbal_ut_range_of_entries_clear,       UT_ALL_DEVICES       , {DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_MAPPING, DBAL_TABLE_EXAMPLE_SW_RANGE, DBAL_UT_FILL_TABLE_FOR_2}},
    {"range_entries_double",        RANGE_OF_ENTRIES_DOUBLE,      CTEST_PRECOMMIT, dnx_dbal_ut_range_of_entries_double,      UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_SW_RANGE, DBAL_UT_FILL_TABLE_FOR_1}},
    {"all_instances",               ALL_INSTANCES,                CTEST_PRECOMMIT, dnx_dbal_ut_all_instances_set,            UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_TABLE_FOR_MUL_INSTANCES, DBAL_UT_FILL_TABLE_FOR_1}},
    {"update_handle_with_buffers",  UPDATE_HANDLE_WITH_BUFFERS,   CTEST_PRECOMMIT, dnx_dbal_ut_update_handle_with_buffers,   UT_ALL_DEVICES       , {DBAL_TABLE_FWD_MACT, DBAL_UT_FILL_TABLE_FOR_1}},
    {"const_value",                 CONST_VALUE,                  CTEST_PRECOMMIT, dnx_dbal_ut_const_values,                 UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_TABLE_CONST_VALUES, DBAL_UT_FILL_TABLE_FOR_1}},
    {"dpc_core_any",                DPC_CORE_ANY,                 CTEST_PRECOMMIT, dnx_dbal_ut_core_any_dpc,                 UT_DUAL_CORE_DEVICES , {DBAL_TABLE_INGRESS_PORT_STP, DBAL_TABLE_EXAMPLE_SW_DIRECT, DBAL_UT_FILL_TABLE_FOR_2}},
    {"dpc_core_all",                DPC_CORE_ALL,                 CTEST_PRECOMMIT, dnx_dbal_ut_core_all_dpc,                 UT_DUAL_CORE_DEVICES , {DBAL_TABLE_INGRESS_PTC_PORT, DBAL_TABLE_INGRESS_PP_RECYCLE_COMMAND, DBAL_UT_FILL_TABLE_FOR_2}},
    {"uint64_fields",               UINT64_FIELDS,                CTEST_PRECOMMIT, dnx_dbal_ut_uint64_fields,                UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_FOR_UINT64_USAGE_TEST, DBAL_UT_FILL_TABLE_FOR_1}},
    {"both_cores",                  BOTH_CORES,                   CTEST_PRECOMMIT, dnx_dbal_ut_both_cores,                   UT_DUAL_CORE_DEVICES , {DBAL_TABLE_INGRESS_ETHERNET_OAM_OPCODE_MAP, DBAL_TABLE_IN_LIF_PROFILE_TABLE, DBAL_UT_FILL_TABLE_FOR_2}},
    {"mac_encoding",                MAC_ENCODING,                 CTEST_PRECOMMIT, dnx_dbal_ut_mac_encoding,                 UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_SW_EM, DBAL_UT_FILL_TABLE_FOR_1}},
    {"predefined_field_val",        PREDEFINED_FIELD_VAL,         CTEST_PRECOMMIT, dnx_dbal_ut_predefine_value,              UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_FOR_HL_WITH_MEMORY_ALIASING, DBAL_TABLE_INGRESS_ETHERNET_OAM_OPCODE_MAP, DBAL_TABLE_IN_LIF_PROFILE_TABLE, DBAL_UT_FILL_TABLE_FOR_3}},
    {"struct_field_encode_decode",  STRUCT_FIELD_ENCODE_DECODE,   CTEST_PRECOMMIT, dnx_dbal_ut_struct_field_encode_decode,   UT_ALL_DEVICES       , {DBAL_UT_NO_TABLE}},
    {"dynamic_table_validation",    DYNAMIC_TABLE_VALIDATION,     CTEST_PRECOMMIT, dnx_dbal_ut_dynamic_table_validations,    UT_ALL_DEVICES       , {DBAL_UT_NO_TABLE}},
    {"dynamic_res_type",            DYNAMIC_RES_TYPE,             CTEST_PRECOMMIT, dnx_dbal_ut_dynamic_res_type,             UT_ALL_DEVICES       , {DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION, DBAL_UT_FILL_TABLE_FOR_1}},
    {"iterator_with_key_rules",     ITERATOR_WITH_KEY_RULES,      CTEST_PRECOMMIT, dnx_dbal_ut_iterator_with_key_rules,      UT_ALL_DEVICES , {DBAL_TABLE_EXAMPLE_FOR_HL_MULTIPLE_RESULT, DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD, DBAL_TABLE_EXAMPLE_SW_EM_LONG_KEY, DBAL_UT_FILL_TABLE_FOR_3}},
    {"iterator_with_value_rules",   ITERATOR_WITH_VALUE_RULES,    CTEST_PRECOMMIT, dnx_dbal_ut_iterator_with_value_rules,    UT_ALL_DEVICES , {DBAL_TABLE_EXAMPLE_FOR_HL_MULTIPLE_RESULT, DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST, DBAL_TABLE_EXAMPLE_SW_LONG_RES, DBAL_UT_FILL_TABLE_FOR_3}},
    {"iterator_with_rules_actions", ITERATOR_WITH_RULES_ACTIONS,  CTEST_PRECOMMIT, dnx_dbal_ut_iterator_with_rules_actions,  UT_ALL_DEVICES , {DBAL_TABLE_EXAMPLE_FOR_HL_MULTIPLE_RESULT, DBAL_UT_FILL_TABLE_FOR_1}},
    {"iterator_multiple_rt",        ITERATOR_MULTIPLE_RT,         CTEST_PRECOMMIT, dnx_dbal_ut_iterator_mul_result,          UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_FOR_HL_MULTIPLE_RESULT, DBAL_TABLE_L2_GPORT_TO_FORWARDING_SW_INFO, DBAL_TABLE_EXAMPLE_SW_MUL_RESULT, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB, DBAL_TABLE_ING_VSI_INFO_DB, DBAL_UT_FILL_TABLE_FOR_5}},
    {"iterator_multiple_rt_mcdb",   ITERATOR_MULTIPLE_RT_MCDB,    CTEST_POSTCOMMIT,dnx_dbal_ut_iterator_mul_result,          UT_ALL_DEVICES       , {DBAL_TABLE_MCDB, DBAL_UT_FILL_TABLE_FOR_1}},
    {"iterator_hit_bit_rule",       ITERATOR_HIT_BIT_RULE,        CTEST_POSTCOMMIT, dnx_dbal_ut_iterator_hit_bit_rule,        UT_ALL_DEVICES       , {DBAL_TABLE_ING_VSI_INFO_DB, DBAL_UT_FILL_TABLE_FOR_1}},
    {"iterator_age_rule",           ITERATOR_AGE_RULE,            CTEST_PRECOMMIT, dnx_dbal_ut_iterator_age_rule,            UT_ALL_DEVICES       , {DBAL_TABLE_FWD_MACT, DBAL_UT_FILL_TABLE_FOR_1}},
    {"table_hitbit",                TABLE_HITBIT,                 CTEST_PRECOMMIT, dnx_dbal_ut_table_hitbit,                 UT_ALL_DEVICES       , {DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD, DBAL_UT_FILL_TABLE_FOR_1}},
    {"tcam_cs",                     TCAM_CS,                      CTEST_PRECOMMIT, dnx_dbal_ut_tcam_cs_validations,          UT_ALL_DEVICES       , {DBAL_TABLE_TCAM_CS_IPMF3, DBAL_TABLE_TCAM_CS_EPMF, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM, DBAL_UT_FILL_TABLE_FOR_3, }},
    {"commit_forced",               COMMIT_FORCED,                CTEST_PRECOMMIT, dnx_dbal_ut_commit_forced,                UT_ALL_DEVICES       , {DBAL_TABLE_INGRESS_OAM_ACC_MEP_DB, DBAL_TABLE_IPV6_MULTICAST_PRIVATE_LPM_FORWARD, DBAL_UT_FILL_TABLE_FOR_3}},
    {"tcam_basic",                  TCAM_BASIC,                   CTEST_PRECOMMIT, dnx_dbal_ut_tcam_table_test,              UT_ALL_DEVICES       , {DBAL_TABLE_IPV4_MULTICAST_TCAM_FORWARD, DBAL_UT_FILL_TABLE_FOR_1}},
    {"tcam_mdb",                    TCAM_MDB,                     CTEST_PRECOMMIT, dnx_dbal_ut_mdb_tcam_validations,         UT_ALL_DEVICES       , {DBAL_TABLE_IPV4_TT_TCAM_BASIC, DBAL_UT_FILL_TABLE_FOR_1}},
    {"err_code_non_direct",         ERR_CODE_NON_DIRECT,          CTEST_PRECOMMIT, dnx_dbal_ut_non_direct_error_codes,       UT_ALL_DEVICES       , {DBAL_TABLE_BIER_BFER_TERMINATION, DBAL_TABLE_BFD_ENDPOINT_SW_INFO, DBAL_UT_FILL_TABLE_FOR_2}},
    {"valid_ind",                   VALID_IND,                    CTEST_PRECOMMIT, dnx_dbal_ut_field_valid_indication,       UT_ALL_DEVICES       , {DBAL_UT_NO_TABLE}},
    {"table_restore",               TABLE_RESTORE,                CTEST_PRECOMMIT, dnx_dbal_ut_table_restore,                UT_ALL_DEVICES       , {DBAL_UT_NO_TABLE}},
    {"hl_tcam",                     HL_TCAM,                      CTEST_PRECOMMIT, dnx_dbal_ut_hl_tcam,                      UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_HL_TCAM, DBAL_TABLE_EXAMPLE_HL_TCAM_DATA_OFFSET, DBAL_UT_FILL_TABLE_FOR_2}},
    {"lpm_mask",                    LPM_MASK,                     CTEST_PRECOMMIT, dnx_dbal_ut_lpm_mask,                     UT_ALL_DEVICES       , {DBAL_TABLE_IPV4_MULTICAST_PRIVATE_LPM_FORWARD, DBAL_UT_FILL_TABLE_FOR_1}},
    {"merge_entries",               MERGE_ENTRIES,                CTEST_PRECOMMIT, dnx_dbal_ut_merge_entries,                UT_ALL_DEVICES , {DBAL_TABLE_EGR_VSI_INFO_DB, DBAL_TABLE_INGRESS_OAM_ACC_MEP_DB/** MDB EM */, DBAL_TABLE_EXAMPLE_TABLE_FOR_MUL_INSTANCES, DBAL_TABLE_L3_VRRP_PROTOCOL_GROUP_SW /** SW direct*/, DBAL_TABLE_BFD_ENDPOINT_SW_INFO/** SW EM */, DBAL_UT_FILL_TABLE_FOR_5 }},
    {"entries_counter",             ENTRIES_COUNTER,              CTEST_PRECOMMIT, dnx_dbal_ut_entries_counter,              UT_ALL_DEVICES , {DBAL_TABLE_EXAMPLE_SW_EM, DBAL_TABLE_EXAMPLE_SW_DIRECT, DBAL_TABLE_EXAMPLE_HL_TCAM, DBAL_TABLE_TCAM_CS_EPMF, DBAL_UT_FILL_TABLE_FOR_4}},
    {"field_allocator",             FIELD_ALLOCATOR,              CTEST_PRECOMMIT, dnx_dbal_ut_field_allocator,              UT_ALL_DEVICES       , {DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_INFO, DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_TO_INTERNAL_MAP_ID, DBAL_TABLE_EXAMPLE_SW_ALLOC, DBAL_UT_FILL_TABLE_FOR_3}},
    {"read_only_field",             READONLY_FIELD,               CTEST_PRECOMMIT, dnx_dbal_ut_readonly_field,               UT_ALL_DEVICES , {DBAL_TABLE_NIF_DEBUG_SIGNALS, DBAL_UT_FILL_TABLE_FOR_1}},
    {"write_only_field",            WRITEONLY_FIELD,              CTEST_PRECOMMIT, dnx_dbal_ut_read_not_readable_field,      UT_ALL_DEVICES       , {DBAL_TABLE_L2_AGE_SCAN, DBAL_UT_FILL_TABLE_FOR_1}},
    {"trigger_field",               TRIGGER_FIELD,                CTEST_PRECOMMIT, dnx_dbal_ut_read_not_readable_field,      UT_ALL_DEVICES       , {DBAL_TABLE_HCC_BIST_RUN, DBAL_UT_FILL_TABLE_FOR_1}},
    {"arr_prefix",                  ARR_PREFIX,                   CTEST_PRECOMMIT, dnx_dbal_ut_arr_prefix,                   UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_TABLE_CONST_VALUES, DBAL_UT_FILL_TABLE_FOR_1}},
    {"struct_arr_prefix",           STRUCT_ARR_PREFIX,            CTEST_PRECOMMIT, dnx_dbal_ut_struct_arr_prefix,            UT_ALL_DEVICES       , {DBAL_UT_NO_TABLE}},
    {"mact_iterator",               MACT_ITERATOR,                CTEST_POSTCOMMIT, dnx_dbal_ut_mact_iterator,                UT_ALL_DEVICES       , {DBAL_TABLE_FWD_MACT, DBAL_UT_FILL_TABLE_FOR_1}},
    {"mact_iterator_key_only",      MACT_ITERATOR_KEY_ONLY,       CTEST_POSTCOMMIT, dnx_dbal_ut_mact_iterator_key_rule_only, UT_ALL_DEVICES       , {DBAL_TABLE_FWD_MACT, DBAL_UT_FILL_TABLE_FOR_1}},
    {"mact_iterator_key_and_dest",  MACT_ITERATOR_KEY_DEST_ONLY,  CTEST_POSTCOMMIT, dnx_dbal_ut_mact_iterator_key_and_dest,  UT_ALL_DEVICES       , {DBAL_TABLE_FWD_MACT, DBAL_UT_FILL_TABLE_FOR_1}},
    {"default_value",               DEFAULT_VALUE_TEST,           CTEST_PRECOMMIT, dnx_dbal_ut_default_values,               UT_DUAL_CORE_DEVICES , {DBAL_TABLE_NIF_ILE_CONNECTIVITY, DBAL_TABLE_ECGM_CORE_TC_MULTICAST_SP_THRESHOLDS, DBAL_UT_FILL_TABLE_FOR_3}},
    {"disabled_result_type",        DISABLED_RESULT_TYPE,         CTEST_PRECOMMIT, dnx_dbal_ut_disabled_result_type,         UT_ALL_DEVICES       , {DBAL_TABLE_EXAMPLE_TABLE_FOR_HL_WITH_MUL_SW_FIELD, DBAL_UT_FILL_TABLE_FOR_1}},
    {"update_result_type",          UPDATE_RESULT_TYPE,           CTEST_PRECOMMIT, dnx_dbal_ut_update_result_type,           UT_ALL_DEVICES       , {DBAL_TABLE_ING_VSI_INFO_DB, DBAL_UT_FILL_TABLE_FOR_1}},
    {"none_direct_mode",            NONE_DIRECT_MODE,             CTEST_PRECOMMIT, dnx_dbal_ut_none_direct_mode,             UT_ALL_DEVICES       , {DBAL_TABLE_BIER_BFER_TERMINATION, DBAL_TABLE_EXAMPLE_SW_SUB_FIELD, DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD, DBAL_UT_FILL_TABLE_FOR_3}},
    {"table_protection",            TABLE_PROTECTION,             CTEST_PRECOMMIT, dnx_dbal_ut_table_protection_validate,    UT_ALL_DEVICES       , {DBAL_TABLE_BIER_BFER_TERMINATION, DBAL_TABLE_EXAMPLE_SW_SUB_FIELD, DBAL_UT_FILL_TABLE_FOR_2}},
    {"table_no_validations",        TABLE_NO_VALIDATIONS,         CTEST_PRECOMMIT, dnx_dbal_ut_table_no_validations,         UT_ALL_DEVICES       , {DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD, DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY, DBAL_TABLE_ECMP_TABLE, DBAL_TABLE_INGRESS_LBP_GLOBAL, DBAL_UT_FILL_TABLE_FOR_4}},
};


/** This array holds all the table that are tested in the capacity test */
static dbal_tables_e dbal_capacity_test_list[] = {DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD,
                                                  DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST,
                                                  DBAL_TABLE_GLOBAL_LIF_EM,
                                                  DBAL_TABLE_RCH_TERM_BFIR_2ND_PASS,
                                                  DBAL_TABLE_ING_VSI_INFO_DB,
                                                  DBAL_TABLE_FWD_MACT,
                                                  DBAL_TABLE_IPV6_MULTICAST_EM_FORWARD};


/** This array holds all the table that are tested in the performance test */
static dbal_tables_e dbal_performance_test_list_mdb[] = {

    /** MDB tables */
    DBAL_TABLE_BIER_BFER_TERMINATION,
    DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SOURCE_SPECIFIC_SVL,
    DBAL_TABLE_EGRESS_OAM_ACC_MEP_DB,
    DBAL_TABLE_EGR_VSI_INFO_DB,
    DBAL_TABLE_ESEM_DUAL_HOMING_DB,
    DBAL_TABLE_EXEM_PER_PORT_MY_MAC,
    DBAL_TABLE_EXEM_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_0,
    DBAL_TABLE_FWD_MACT,
    DBAL_TABLE_GLOBAL_LIF_EM,
    DBAL_TABLE_ING_VSI_INFO_DB,
    DBAL_TABLE_IN_AC_C_C_VLAN_DB,
    DBAL_TABLE_IPV4_MULTICAST_EM_FORWARD,
    DBAL_TABLE_IPV4_MULTICAST_PRIVATE_LPM_FORWARD,
    DBAL_TABLE_IPV4_TT_P2P_EM_BASIC,
    DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD,
    DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST,
    DBAL_TABLE_MPLS_FWD,
    DBAL_TABLE_VNI2VRF,
    DBAL_TABLE_VNI2VSI};

static dbal_tables_e dbal_performance_test_list_hl[] = {

    /** Harad logic tables */
    DBAL_TABLE_QOS_INGRESS_PHB_INTERNAL_MAP_ENTRIES,
    DBAL_TABLE_INGRESS_PP_PORT,
    DBAL_TABLE_EGRESS_PP_PORT,
    DBAL_TABLE_EGRESS_OAM_MP_TYPE_MAP,
    DBAL_TABLE_EGRESS_PORT_STP,
    DBAL_TABLE_EGRESS_SAME_INTERFACE_FILTER,
    DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_MAPPING,
    DBAL_TABLE_FABRIC_CGM_ING_TRANSMIT_PER_CORE_SHAPERS_CONFIG,
    DBAL_TABLE_FABRIC_CGM_RCI_THRESHOLDS,
    DBAL_TABLE_MACT_VSI_COUNTERS,
    DBAL_TABLE_MACT_AGING_CONFIGURATION,
    DBAL_TABLE_FIELD_PMF_A_FEM_MAP_INDEX,
    DBAL_TABLE_FC_GEN_OOB_CAL_MAP,
    DBAL_TABLE_FABRIC_MIN_LINKS_TO_DEST,
    DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_MAPPING,
    DBAL_TABLE_KLEAP_E_PMF_FFC_INSTRUCTION};

static dbal_tables_e dbal_performance_test_list_sw[] = {

    /** SW tables */

    DBAL_TABLE_SAT_STATE_SW,
    DBAL_TABLE_VSI_STG_INFO_SW,
    DBAL_TABLE_TAG_STRUCT_EGRESS_HW_ID_TO_SW_ID,
    DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO,
    DBAL_TABLE_EGRESS_LIF_TO_EGRESS_POINTED_MAP_SW,
    DBAL_TABLE_INGRESS_GPORT_TO_LOCAL_IN_LIF_SW,
    DBAL_TABLE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW};

/* *INDENT-ON* */
/*************
* FUNCTIONS *
*************/

/**
* \brief
* Return the next table id that matches to the input string, (next refers to the curr_table_id)
* if the substring is fully match to the table name  than is_full_match=1
* use curr_table_id = -1 to start form the first existing table
*/
static cmd_result_t
dbal_string_to_next_table_id_get(
    int unit,
    char *substr_match,
    dbal_tables_e curr_table_id,
    dbal_tables_e * table_id,
    int *is_full_match)
{
    int iter;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    *table_id = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);
    *is_full_match = 0;

    curr_table_id++;

    if ((substr_match == NULL) || (curr_table_id > dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input substr_match not exists or table_id too big\n");
    }

    for (iter = curr_table_id; iter < dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit); iter++)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, iter, &table));

        if (!sal_strcasecmp(substr_match, table->table_name))
        {
            *is_full_match = 1;
            *table_id = iter;
            break;
        }

        if (strcaseindex(table->table_name, substr_match))
        {
            *table_id = iter;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Print table names fit current substring
*/
shr_error_e
dbal_print_table_names(
    int unit,
    char *table_name)
{
    CONST dbal_logical_table_t *table;
    int is_full_match = 0;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    dbal_string_to_next_table_id_get(unit, table_name, -1, &table_id, &is_full_match);

    if (table_id != dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        while (table_id != dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
        {
            SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
            LOG_CLI((BSL_META("\t%s \n"), table->table_name));

            dbal_string_to_next_table_id_get(unit, table_name, table_id, &table_id, &is_full_match);
        }
    }
    else
    {
        LOG_CLI((BSL_META("\tNo related tables found\n")));
    }
exit:
    SHR_FUNC_EXIT;
}

static uint8
dbal_ltt_full_unitest_table_list(
    int unit,
    dbal_tables_e table_id)
{
    int ii, nof_tables;

    dbal_tables_e table_list[] = {

        /** Specific tables */
        DBAL_NOF_TABLES,
        DBAL_TABLE_EXAMPLE_SW_DIRECT,
        DBAL_TABLE_EXAMPLE_SW_SUB_FIELD,
        DBAL_TABLE_EXAMPLE_SW_RANGE,
        DBAL_TABLE_EXAMPLE_SW_LONG_RES,
        DBAL_TABLE_EXAMPLE_SW_RESULT_TYPE,
        DBAL_TABLE_EXAMPLE_SW_EM,
        DBAL_TABLE_EXAMPLE_TABLE_REGISTER_GROUP,
        DBAL_TABLE_EXAMPLE_TABLE_FOR_HL_WITH_MUL_SW_FIELD,
        DBAL_TABLE_EXAMPLE_TABLE_FOR_MUL_INSTANCES,
        DBAL_TABLE_EXAMPLE_FOR_HL_MULTIPLE_RESULT,
        DBAL_TABLE_EXAMPLE_FOR_HL_WITH_MEMORY_ALIASING,
        DBAL_TABLE_EXAMPLE_FOR_UINT64_USAGE_TEST,
        DBAL_TABLE_EXAMPLE_FOR_DNX_DATA_REF,
        DBAL_TABLE_INGRESS_OAM_LIF_DB,
        DBAL_TABLE_IN_AC_S_C_VLAN_DB,
        DBAL_TABLE_IPV4_TT_P2P_EM_BASIC,
        DBAL_TABLE_MPLS_FWD,
        DBAL_TABLE_IPV4_MULTICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_GLOBAL_LIF_EM,
        DBAL_TABLE_EXAMPLE_TABLE_ILLEGAL_VALUES,
        DBAL_TABLE_EXAMPLE_TABLE_CONST_VALUES,
        DBAL_TABLE_EGRESS_ADDITIONAL_HEADERS_MAP_TABLE,
        DBAL_TABLE_TCAM_CS_IPMF3,
        DBAL_TABLE_EXAMPLE_MDB_WITH_RESULT_TYPE_IN_SW_DIRECT,
        DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB,
        DBAL_TABLE_NIF_ETHU_RX_RMC_CTRL,
        DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_DB,
        DBAL_TABLE_IN_LIF_FORMAT_LSP,
        DBAL_TABLE_CRPS_METER_BIG_ENGINE_SHARED_MEM,
        DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_NIF_PRD_ETHU_IP_PRIORITY_MAP,
        DBAL_TABLE_EEDB_ARP,
        DBAL_TABLE_EEDB_IPV4_TUNNEL,
        DBAL_TABLE_EEDB_IPV6_TUNNEL,
        DBAL_TABLE_EEDB_MPLS_TUNNEL,
        DBAL_TABLE_EEDB_OUT_AC,
        DBAL_TABLE_EEDB_PWE,
        DBAL_TABLE_EEDB_RCH,
        DBAL_TABLE_EEDB_SRV6,
        DBAL_TABLE_EEDB_DATA_ENTRY,
        DBAL_TABLE_PEMLA_TRAP
    };

    nof_tables = sizeof(table_list) / sizeof(dbal_tables_e);

    for (ii = 0; ii < nof_tables; ii++)
    {
        if (table_id == table_list[ii])
        {
            if (SAL_BOOT_PLISIM)
            {
                return TRUE;
            }
            else
            {
                /**in real device KBP tables cannot be supported unless KBP is enabled */
                CONST dbal_logical_table_t *table;

                dbal_tables_table_get(unit, ii, &table);

                if (table->access_method == DBAL_ACCESS_METHOD_KBP)
                {
                    return FALSE;
                }
                else
                {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

static uint8
dbal_ltt_mdb_cfg_table_list(
    int unit,
    dbal_tables_e table_id)
{
    int ii;

    for (ii = 0; ii < DBAL_NOF_TABLES; ii++)
    {
        if (table_id == mdb_init_tables_list[ii])
        {
            return TRUE;
        }
        if (mdb_init_tables_list[ii] == DBAL_NOF_TABLES)
        {
            break;
        }
    }
    return FALSE;
}

static shr_error_e
dbal_ltt_measurements_init(
    int unit,
    prt_control_t * prt_ctr,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /** if measurements set, init timers  */
    if (flags & LTT_FLAGS_MEASUREMENTS)
    {
        LTT_TIMERS_ALLOC();
        PRT_COLUMN_ADD("Total[us]");
        PRT_COLUMN_ADD("Entry Commit[us]");
        PRT_COLUMN_ADD("Entry Get[us]");

        if ((flags & LTT_FLAGS_FULL_ITERATOR_TABLE_CLEAR_TEST) || (flags & LTT_FLAGS_RUN_ITERATOR))
        {
            PRT_COLUMN_ADD("Iterator[us]");
        }

        if ((flags & LTT_FLAGS_FULL_ITERATOR_TABLE_CLEAR_TEST) || (flags & LTT_FLAGS_TABLE_CLEAR_END_OF_TEST))
        {
            PRT_COLUMN_ADD("Table Clear[us]");
            PRT_COLUMN_ADD("Iterator Empty[us]");
        }
        else
        {
            PRT_COLUMN_ADD("Entry Clear[us]");
        }
        PRT_COLUMN_ADD("Time Limit");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_ltt_measurements_pre_test(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (flags & LTT_FLAGS_MEASUREMENTS)
    {
        LTT_TIMERS_CLEAR_ALL();
        LTT_TIMERS_TIMER_SET("Entire Test Timer", LTT_TIMER_ENTIRE_LTT_TEST);
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_ltt_measurements_after_test(
    int unit,
    prt_control_t * prt_ctr,
    dbal_tables_e table_id,
    uint32 flags)
{
    int curr_res;
    SHR_FUNC_INIT_VARS(unit);

    if (flags & LTT_FLAGS_MEASUREMENTS)
    {
        LTT_TIMERS_TIMER_STOP(LTT_TIMER_ENTIRE_LTT_TEST);
        LTT_TIMERS_ALL_GET_INFO();

        PRT_CELL_SET("%d", LTT_TIMERS_TIMER_GET_TIME_US(LTT_TIMER_ENTIRE_LTT_TEST));
        PRT_CELL_SET("%d", LTT_TIMERS_TIMER_GET_TIME_US(LTT_TIMER_SET));
        PRT_CELL_SET("%d", LTT_TIMERS_TIMER_GET_TIME_US(LTT_TIMER_GET));

        if ((flags & LTT_FLAGS_FULL_ITERATOR_TABLE_CLEAR_TEST) || (flags & LTT_FLAGS_RUN_ITERATOR))
        {
            PRT_CELL_SET("%d", LTT_TIMERS_TIMER_GET_TIME_US(LTT_TIMER_ITERATOR));
        }

        if ((flags & LTT_FLAGS_FULL_ITERATOR_TABLE_CLEAR_TEST) || (flags & LTT_FLAGS_TABLE_CLEAR_END_OF_TEST))
        {
            PRT_CELL_SET("%d", LTT_TIMERS_TIMER_GET_TIME_US(LTT_TIMER_TABLE_CLEAR));
            PRT_CELL_SET("%d", LTT_TIMERS_TIMER_GET_TIME_US(LTT_TIMER_ITERATOR_EMPTY_TABLE));
        }
        else
        {
            PRT_CELL_SET("%d", LTT_TIMERS_TIMER_GET_TIME_US(LTT_TIMER_ENTRY_CLEAR));
        }
        curr_res = LTT_TIMERS_CHECK_FOR_EXCEPTIONS(unit, table_id);
        if (curr_res != _SHR_E_NONE)
        {
            PRT_CELL_SET("%s", "TIMEOUT FAILURE");
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        }
        else
        {
            PRT_CELL_SET("%s", "IN RANGE");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_ltt_measurements_finish(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & LTT_FLAGS_MEASUREMENTS)
    {
        LTT_TIMERS_FREE();
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * Check if a table has at least one readable result field
 */
static uint8
dbal_ltt_is_readable_result_field(
    int unit,
    CONST dbal_logical_table_t * table)
{
    int result_type_id;
    for (result_type_id = 0; result_type_id < table->nof_result_types; result_type_id++)
    {
        int result_id;
        multi_res_info_t result_set = table->multi_res_info[result_type_id];
        for (result_id = 0; result_id < result_set.nof_result_fields; result_id++)
        {
            dbal_table_field_info_t field_info = result_set.results_info[result_id];
            /**
             * readable field
             */
            if (DBAL_PERMISSION_WRITEONLY != field_info.permission
                && (DBAL_PERMISSION_TRIGGER != field_info.permission))
                return TRUE;
        }
    }
    /**
     * No readable field
     */
    return FALSE;
}

/**
* \brief
*  Utility function for parsing the input arguments and validating them: table_name, access_method, table_type, flags.
*   \param [in] unit         - Relevant unit.
*   \param [in] args         - The ctest framework arguments.
*   \param [in] sand_control - The shell command control structure.
*   \param [in] test_name    - The name of the test.
*   \param [in] flags        - Flags provided by the specific test.
*   \param [out] ltt_input   - Utility structure in which the parsed arguments are returned.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
static shr_error_e
dbal_diag_ltt_prepare_inputs(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    char *test_name,
    dbal_ltt_flags_e flags,
    dnx_dbal_ltt_input_t * ltt_input)
{
    char *table_name = NULL;
    char *flag = NULL;
    char *access = NULL;
    char *type = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_STR("flags", flag);
    SH_SAND_GET_STR("access", access);
    SH_SAND_GET_STR("type", type);
    SH_SAND_GET_UINT32("EntryNum", ltt_input->nof_entries);

    ltt_input->dbal_table = DBAL_TABLE_EMPTY;
    ltt_input->access_method = DBAL_NOF_ACCESS_METHODS;
    ltt_input->table_type = DBAL_NOF_TABLE_TYPES;
    ltt_input->flags = flags;

    if (ltt_input->nof_entries > DIAG_DBAL_MAX_NOF_ENTRIES_PER_TABLE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The number of entries requested is %d, and should not be "
                     "more than %d\n", ltt_input->nof_entries, DIAG_DBAL_MAX_NOF_ENTRIES_PER_TABLE);
    }

    if (ISEMPTY(table_name))
    {
        if (ISEMPTY(access) && ISEMPTY(type))
        {
            LOG_CLI((BSL_META("Example: ctest dbal %s Table=<table_name>\n"), test_name));
            LOG_CLI((BSL_META("Example: ctest dbal %s Access=<table_access> Type=<table_type>"), test_name));
            SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            SHR_EXIT();
        }

        /** Instead of a table name we have filters, check which filters we have specified */
        if (!ISEMPTY(access)
            && (dbal_access_method_string_to_id(unit, access, &ltt_input->access_method) == _SHR_E_NOT_FOUND))
        {
            ltt_input->access_method = DBAL_NOF_ACCESS_METHODS;
        }

        if (!ISEMPTY(type)
            && (dbal_logical_table_type_string_to_id(unit, type, &ltt_input->table_type) == _SHR_E_NOT_FOUND))
        {
            ltt_input->table_type = DBAL_NOF_TABLE_TYPES;
        }
        LOG_CLI((BSL_META("Running test with filters applied to all existing logical tables...\n\n")));
    }
    else
    {
        /** table name was specified */
        if (dbal_logical_table_string_to_id_no_error(unit, table_name, &ltt_input->dbal_table) == _SHR_E_NOT_FOUND)
        {
            /** Table was not found */
            LOG_CLI((BSL_META("No matching tables found\nRelated tables:\n")));
            SHR_IF_ERR_EXIT(dbal_print_table_names(unit, table_name));
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }
    if (!ISEMPTY(flag))
    {
        ltt_input->flags = sal_ctoi(flag, 0);
        if (ltt_input->flags >= LTT_FLAGS_NOF_FLAGS)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal LTT flags, flags=0x%x\n", ltt_input->flags);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Utility function for deciding whether a table should be skipped, determined by the provided inputs.
 */
static uint8
dbal_logical_table_test_skip(
    int unit,
    CONST dbal_logical_table_t * table,
    dnx_dbal_ltt_input_t * ltt_input)
{
    /*
     * 1. run only high maturity level tables
     * 2. check if we should filter by access method
     * 3. check if we should filter by type
     * 4. check if we should filter out tables that have multiple results
     * 5. check if we should filter out tables that contain sw fields
     */
    if ((table->maturity_level != DBAL_MATURITY_HIGH)
        || ((DBAL_NOF_ACCESS_METHODS != ltt_input->access_method) && (table->access_method != ltt_input->access_method))
        || ((DBAL_NOF_TABLE_TYPES != ltt_input->table_type) && (table->table_type != ltt_input->table_type))
        || ((_SHR_IS_FLAG_SET(ltt_input->flags, LTT_FLAGS_MULTIPLE_RESULTS)) && (table->nof_result_types <= 1))
        || ((_SHR_IS_FLAG_SET(ltt_input->flags, LTT_FLAGS_SW_FIELDS)) &&
            (table->sw_access_info.sw_payload_length_bytes == 0)))
    {
        return TRUE;
    }

    return FALSE;
}

/**
* \brief
* Run logical test for specific/filtered table(s) according to the framework arguments and test specific flags
*/
shr_error_e
dbal_logical_tables_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    char *test_name,
    dbal_ltt_flags_e test_specific_flags)
{
    int ii, iter_start_pos = DBAL_TABLE_EMPTY, iter_end_pos = DBAL_NOF_TABLES;
    shr_error_e curr_res = _SHR_E_NONE, err_rec_test_ltt_res = _SHR_E_NONE, result = _SHR_E_NONE;
    uint8 filtered_tables = FALSE;
    dnx_dbal_ltt_input_t ltt_input;
    dbal_table_status_e table_status;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    /** Parse the LTT inputs */
    SHR_SET_CURRENT_ERR(dbal_diag_ltt_prepare_inputs
                        (unit, args, sand_control, test_name, test_specific_flags, &ltt_input));

    if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
    {
        /*
         * Table name was provided, but table was not found.
         * The suggested tables have been printed.
         * Exit without error.
         */
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        SHR_EXIT();
    }
    else if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
    {
        /** In case of other error, it's a real failure */
        SHR_EXIT();
    }

    if ((ltt_input.dbal_table == DBAL_TABLE_EMPTY)
        || (_SHR_IS_FLAG_SET(ltt_input.flags, LTT_FLAGS_MULTIPLE_RESULTS))
        || (_SHR_IS_FLAG_SET(ltt_input.flags, LTT_FLAGS_SW_FIELDS)))
    {
        filtered_tables = TRUE;
    }
    else
    {
        iter_start_pos = ltt_input.dbal_table;
        iter_end_pos = ltt_input.dbal_table + 1;
    }

    /** Set the Result PRT structure */
    PRT_TITLE_SET("Logical Table Test (Flags=0x%x) Summary", ltt_input.flags);
    PRT_COLUMN_ADD("Table Name");
    PRT_COLUMN_ADD("Access Method");
    PRT_COLUMN_ADD("Result");

    SHR_IF_ERR_EXIT(dbal_ltt_measurements_init(unit, prt_ctr, ltt_input.flags));

    for (ii = iter_start_pos; ii < iter_end_pos; ii++)
    {
        CONST dbal_logical_table_t *table;
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, ii, &table));

        /** maturity table low table are not loaded to dbal DB cannot perform test for them */
        if (table->maturity_level == DBAL_MATURITY_LOW)
        {
            continue;
        }

        if (filtered_tables)
        {
            /** check if the table should be skipped */
            if (dbal_logical_table_test_skip(unit, table, &ltt_input))
            {
                continue;
            }
        }

        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, ii, &table_status));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", table->table_name);
        PRT_CELL_SET("%s", dbal_access_method_to_string(unit, table->access_method));

        SHR_IF_ERR_EXIT(dbal_ltt_measurements_pre_test(unit, ltt_input.flags));

        if (table_status == DBAL_TABLE_HW_ERROR)
        {
            curr_res = _SHR_E_INTERNAL;
        }
        else if (_SHR_IS_FLAG_SET(ltt_input.flags, LTT_FLAGS_ERR_RECOVERY_CLEAN_UP)
                 || _SHR_IS_FLAG_SET(ltt_input.flags, LTT_FLAGS_SNAPSHOT_NEGATIVE))
        {
            /*
             * for dbal error recovery tests first the regular ltt test is being run first
             * if the dbal test fails the error recovery test is considered to pass
             */
            dnx_dbal_ltt_input_t local_ltt_input = ltt_input;
            local_ltt_input.flags = LTT_FLAGS_DEFAULT;
            local_ltt_input.nof_entries = DIAG_DBAL_DEFAULT_NOF_ENTRIES;
            err_rec_test_ltt_res = ctest_dbal_table_test_run(unit, ii, &local_ltt_input);
            curr_res =
                (_SHR_E_NONE == err_rec_test_ltt_res) ? ctest_dbal_table_test_run(unit, ii, &ltt_input) : _SHR_E_NONE;
        }
        else
        {
            curr_res = ctest_dbal_table_test_run(unit, ii, &ltt_input);
        }

        if (curr_res == _SHR_E_UNAVAIL)
        {
            PRT_CELL_SET("%s", "NOT_RUN");
        }
        else if (curr_res == _SHR_E_NONE)
        {
            PRT_CELL_SET("%s", "PASS");
            result |= dbal_ltt_measurements_after_test(unit, prt_ctr, ii, ltt_input.flags);
        }
        else
        {
            if (table_status == DBAL_TABLE_HW_ERROR)
            {
                PRT_CELL_SET("%s", "FAIL - Mapping err");
            }
            else
            {
                PRT_CELL_SET("%s", "FAIL");
            }
            result |= _SHR_E_FAIL;
        }
    }

    LOG_CLI((BSL_META("\n")));
    SHR_IF_ERR_EXIT(dbal_ltt_measurements_finish(unit, ltt_input.flags));

    /** print test summary only if logger severity is high enough */
    if (bslenable_get(bslLayerSocdnx, bslSourceDiagdbaldnx) > bslSeverityInfo)
    {
        PRT_COMMITX;
    }

    SHR_SET_CURRENT_ERR(result);

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief run logical table test in ctest with flags = 0
 */
static shr_error_e
cmd_dnx_dbal_ctest_ltt(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_ltt_flags_e flags = 0x0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logical_tables_test(unit, args, sand_control, "ltt", flags));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief run logical table test in ctest with flags = 80
 * full test + timers
 */
static shr_error_e
cmd_dnx_dbal_ctest_ltt_full(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_ltt_flags_e flags = LTT_FLAGS_FULL_ITERATOR_TABLE_CLEAR_TEST | LTT_FLAGS_MEASUREMENTS;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logical_tables_test(unit, args, sand_control, "ltt_full", flags));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief run logical table test in ctest with flags = 0 and skip shadow = TRUE, relevant for MDB only
 */
static shr_error_e
cmd_dnx_dbal_ctest_ltt_skip_shadow(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_ltt_flags_e flags = 0x0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_db.mdb_access.skip_read_from_shadow.set(unit, TRUE));
    SHR_IF_ERR_EXIT(dbal_logical_tables_test(unit, args, sand_control, "ltt", flags));
    SHR_IF_ERR_EXIT(dbal_db.mdb_access.skip_read_from_shadow.set(unit, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief fill an array of entries with values to be set. if one of the fields is allocator, handling allocations.
 */

static shr_error_e
dbal_performance_entries_prepare(
    int unit,
    CONST dbal_logical_table_t * table,
    dbal_tables_e table_id,
    uint32 nof_entries,
    dbal_entries_array_t * entries_array)
{
    int field_iter, entry_iter;
    uint32 cur_values[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { 0 };
    uint32 min_values[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { 0 };
    uint32 max_values[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { 0 };
    int possible_nof_entries = 1;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Preparing the [%d] entries for test...\n"), nof_entries));

    for (field_iter = table->nof_key_fields - 1; field_iter >= 0; field_iter--)
    {
        min_values[field_iter] = table->keys_info[field_iter].min_value;
        cur_values[field_iter] = table->keys_info[field_iter].min_value;
        if (table->keys_info[field_iter].field_nof_bits > 32)
        {
            /** firld that is bigger than 32bit, will use it to inc, so there is enough entries... */
            max_values[field_iter] = UTILEX_U32_MAX;
            possible_nof_entries = UTILEX_U32_MAX;
        }
        else
        {
            max_values[field_iter] = table->keys_info[field_iter].max_value;
        }

        if (possible_nof_entries != UTILEX_U32_MAX)
        {
            if (possible_nof_entries > possible_nof_entries * (max_values[field_iter] - min_values[field_iter]))
            {
                /** overflow.. reached to the desired nof entries, no need more calulations */
                possible_nof_entries = UTILEX_U32_MAX;
            }
            else
            {
                possible_nof_entries = possible_nof_entries * (max_values[field_iter] - min_values[field_iter]);
            }
        }
    }

    if (possible_nof_entries < nof_entries)
    {
        /** reached to the desired nof entries, no need more calulations */
        SHR_ERR_EXIT(_SHR_E_PARAM, "not enough different entries to add to the table %d out of %d table %s\n",
                     possible_nof_entries, nof_entries, table->table_name);
    }

    for (entry_iter = 0; entry_iter < nof_entries; entry_iter++)
    {
        uint8 is_illegal_value_for_field;
        for (field_iter = table->nof_key_fields - 1; field_iter >= 0; field_iter--)
        {
            if (cur_values[field_iter] == max_values[field_iter])
            {
                cur_values[field_iter] = min_values[field_iter];
            }
            else
            {
                is_illegal_value_for_field = FALSE;
                cur_values[field_iter] = cur_values[field_iter] + 1;

                SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value
                                (unit, table->keys_info[field_iter].field_id, cur_values[field_iter],
                                 &is_illegal_value_for_field));
                if (is_illegal_value_for_field)
                {
                    entry_iter--;
                }

                break;
            }
        }

        if (!is_illegal_value_for_field)
        {
            for (field_iter = 0; field_iter < table->nof_key_fields; field_iter++)
            {
                entries_array->key_fields[entry_iter][field_iter] = cur_values[field_iter];
                if (SHR_BITGET(table->keys_info[field_iter].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR)
                    && (!dbal_table_is_allcator_not_needed(unit, table_id)))
                {
                    SHR_IF_ERR_EXIT(diag_dbal_alloc_dealloc_value(unit,
                                                                  table->keys_info[field_iter].field_id,
                                                                  table_id, 0, DBAL_CORE_ALL, cur_values[field_iter],
                                                                  1));
                }
            }
        }
    }

    /** using same payload for all entries */
    for (field_iter = 0; field_iter < table->multi_res_info[0].nof_result_fields; field_iter++)
    {
        if (table->multi_res_info[0].results_info[field_iter].field_id == DBAL_FIELD_RESULT_TYPE)
        {
            entries_array->result_fields[field_iter] = 0;
        }
        else
        {
            uint32 field_value = field_iter;
            if ((table->multi_res_info[0].results_info[field_iter].max_value < field_value))
            {
                field_value = table->multi_res_info[0].results_info[field_iter].max_value;
            }
            else if (table->multi_res_info[0].results_info[field_iter].min_value > field_value)
            {
                field_value = table->multi_res_info[0].results_info[field_iter].min_value;
            }
            entries_array->result_fields[field_iter] = field_value;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

typedef enum
{
    HADNLE_ALLOC = 0,
    KEY_FIELD_SET,
    RESULT_FIELD_SET,
    ENTRY_COMMIT,
    COMMIT_ACCESS,
    ENTRY_GET_ALL_FIELDS,
    RESULT_FIELD_GET,
    NOF_TIMERS,
} dbal_tests_timerse;

/** run the performance tests according input parametes. this tests doesn't handle  */
static shr_error_e
ctest_dnx_dbal_performance_test(
    int unit,
    dbal_tables_e table_id,
    int nof_entries,
    int is_commit_update,
    int *entries_per_sec)
{
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    uint32 timers_group;
    CONST dbal_logical_table_t *table;
    uint32 entry_handle;
    uint32 commit_flag = DBAL_COMMIT;
    uint32 timer_idx[NOF_TIMERS];
    dbal_entries_array_t entries_array = { {{0}}, {0} };
    int field_iter, entry_iter, ii, nof_entries_to_prepare = nof_entries, estimated_nof_entries = 0;
    int is_timer_alloc = 0, rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (table_id == DBAL_TABLE_FWD_MACT)
    {
        /** for MACT, commit forced is optimized */
        commit_flag = DBAL_COMMIT_FORCE;
    }
    (*entries_per_sec) = 0;

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (dbal_table_is_out_lif(table) || (table_id == DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION))
    {
        LOG_CLI((BSL_META("test not supported for table %s\n"), table->table_name));
        SHR_EXIT();
    }

    if (DBAL_TABLE_IS_TCAM(table))
    {
        LOG_CLI((BSL_META("test not supported tcam tables \n")));
        SHR_EXIT();
    }

    if (is_commit_update && !(DBAL_TABLE_IS_NONE_DIRECT(table)))
    {
        LOG_CLI((BSL_META("update mode is not supported for direct tables, working with commit mode \n")));
    }

    if (!(DBAL_TABLE_IS_NONE_DIRECT(table)) || is_commit_update)
    {
        nof_entries_to_prepare = 1;
    }

    if (nof_entries > CTEST_DBAL_MAX_NOF_ENTRIES_FOR_PERFORMANCE_TEST)
    {
        LOG_CLI((BSL_META("nof entries for test %d exceeded Max nof entries possible using Max %d\n"), nof_entries,
                 CTEST_DBAL_MAX_NOF_ENTRIES_FOR_PERFORMANCE_TEST));
        nof_entries = CTEST_DBAL_MAX_NOF_ENTRIES_FOR_PERFORMANCE_TEST;
        nof_entries_to_prepare = nof_entries;
    }

    SHR_IF_ERR_EXIT(diag_dnx_dbal_table_capacity_estimate(unit, table_id, 0, &estimated_nof_entries));
    if ((nof_entries_to_prepare == 1) && (nof_entries > estimated_nof_entries))
    {
        if (estimated_nof_entries > 0)
        {
            LOG_CLI((BSL_META
                     ("estimated table capacity is smaller than the nof entries, using estimated capactiy %d\n"),
                     estimated_nof_entries));
            nof_entries = estimated_nof_entries;
        }
    }

    for (ii = 0; ii < NOF_TIMERS; ii++)
    {
        timer_idx[ii] = ii + 1;
    }

    sal_strncpy(group_name, "dbal performance", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    SHR_IF_ERR_EXIT(utilex_ll_timer_group_allocate(group_name, &timers_group));
    is_timer_alloc = 1;

    utilex_ll_timer_clear_all(timers_group);

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
    rv = dbal_performance_entries_prepare(unit, table, table_id, nof_entries_to_prepare, &entries_array);
    if (rv)
    {
        LOG_CLI((BSL_META("preparation failed, test skipped\n")));
        SHR_EXIT();
    }

    /** handle alloca-dealloc check. */
    for (entry_iter = 0; entry_iter < nof_entries; entry_iter++)
    {
        utilex_ll_timer_set("handle alloc-clear", timers_group, timer_idx[HADNLE_ALLOC]);
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle));
        utilex_ll_timer_stop(timers_group, timer_idx[HADNLE_ALLOC]);
    }

    /** entry commit check   */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle));

    LOG_CLI((BSL_META("Entry commit test start (%d entries to table %s %d result fields) \n"), nof_entries,
             table->table_name, table->multi_res_info[0].nof_result_fields));
    for (entry_iter = 0; entry_iter < nof_entries; entry_iter++)
    {
        int entry_array_id = entry_iter;
        if (nof_entries_to_prepare == 1)
        {
            entry_array_id = 0;
        }
        for (field_iter = 0; field_iter < table->nof_key_fields; field_iter++)
        {
            utilex_ll_timer_set("key field set", timers_group, timer_idx[KEY_FIELD_SET]);
            dbal_entry_key_field_arr32_set(unit, entry_handle,
                                           table->keys_info[field_iter].field_id,
                                           &(entries_array.key_fields[entry_array_id][field_iter]));
            utilex_ll_timer_stop(timers_group, timer_idx[KEY_FIELD_SET]);
        }

        for (field_iter = 0; field_iter < table->multi_res_info[0].nof_result_fields; field_iter++)
        {
            if (table->multi_res_info[0].results_info[field_iter].permission != DBAL_PERMISSION_READONLY)
            {
                if (table->multi_res_info[0].results_info[field_iter].arr_prefix_size == 0)
                {
                    utilex_ll_timer_set("key field set", timers_group, timer_idx[KEY_FIELD_SET]);
                    dbal_entry_value_field_arr32_set(unit, entry_handle,
                                                     table->multi_res_info[0].results_info[field_iter].field_id,
                                                     INST_SINGLE, &(entries_array.result_fields[field_iter]));
                    utilex_ll_timer_stop(timers_group, timer_idx[KEY_FIELD_SET]);
                }
                else
                {
                    if (SHR_BITGET
                        (table->multi_res_info[0].results_info[field_iter].field_indication_bm,
                         DBAL_FIELD_IND_IS_FIELD_ENUM))
                    {
                        utilex_ll_timer_set("key field set", timers_group, timer_idx[KEY_FIELD_SET]);
                        dbal_entry_value_field_arr32_set(unit, entry_handle,
                                                         table->multi_res_info[0].results_info[field_iter].field_id,
                                                         INST_SINGLE, &(entries_array.result_fields[field_iter]));
                        utilex_ll_timer_stop(timers_group, timer_idx[KEY_FIELD_SET]);
                    }
                    else
                    {
                        utilex_ll_timer_set("key field set", timers_group, timer_idx[KEY_FIELD_SET]);
                        dbal_entry_value_field_predefine_value_set(unit, entry_handle,
                                                                   table->multi_res_info[0].
                                                                   results_info[field_iter].field_id, INST_SINGLE,
                                                                   DBAL_PREDEF_VAL_MIN_VALUE);
                        utilex_ll_timer_stop(timers_group, timer_idx[KEY_FIELD_SET]);
                    }
                }
            }
        }

        utilex_ll_timer_set("entry commit ", timers_group, timer_idx[ENTRY_COMMIT]);
        rv = dbal_entry_commit(unit, entry_handle, commit_flag);
        utilex_ll_timer_stop(timers_group, timer_idx[ENTRY_COMMIT]);
        if (rv)
        {
            LOG_CLI((BSL_META("test fail skipping the table \n")));
            SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle));
        if (is_commit_update && DBAL_TABLE_IS_NONE_DIRECT(table))
        {
            commit_flag = DBAL_COMMIT_UPDATE;
        }
    }

    /** getting all fields */
    for (entry_iter = 0; entry_iter < nof_entries; entry_iter++)
    {
        uint32 field_val;
        int entry_array_id = entry_iter;

        if (nof_entries_to_prepare == 1)
        {
            entry_array_id = 0;
        }

        for (field_iter = 0; field_iter < table->nof_key_fields; field_iter++)
        {
            dbal_entry_key_field_arr32_set(unit, entry_handle,
                                           table->keys_info[field_iter].field_id,
                                           &(entries_array.key_fields[entry_array_id][field_iter]));
        }

        utilex_ll_timer_set("entry get ", timers_group, timer_idx[ENTRY_GET_ALL_FIELDS]);
        rv = dbal_entry_get(unit, entry_handle, DBAL_GET_ALL_FIELDS);
        utilex_ll_timer_stop(timers_group, timer_idx[ENTRY_GET_ALL_FIELDS]);
        if (rv)
        {
            LOG_CLI((BSL_META("test fail skipping the table \n")));
            SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
            SHR_EXIT();
        }

        for (field_iter = 0; field_iter < table->multi_res_info[0].nof_result_fields; field_iter++)
        {
            if ((table->multi_res_info[0].results_info[field_iter].field_nof_bits < 32) &&
                (table->multi_res_info[0].results_info[field_iter].permission != DBAL_PERMISSION_WRITEONLY))
            {
                utilex_ll_timer_set("result field get", timers_group, timer_idx[RESULT_FIELD_GET]);
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle,
                                                                    table->multi_res_info[0].
                                                                    results_info[field_iter].field_id, INST_SINGLE,
                                                                    &field_val));
                utilex_ll_timer_stop(timers_group, timer_idx[RESULT_FIELD_GET]);
            }
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle));
    }

    {
        char *name;
        uint32 total_time, hits;
        uint32 total_time_get, hits_get;
        uint32 total_time_field_set, hits_field_set;
        int is_active;
        double avgr, avgr_get, avgr_field_set;
        int nof_field_set_per_commit = 0;

        SHR_IF_ERR_EXIT(utilex_ll_timer_get_info
                        (timers_group, timer_idx[ENTRY_COMMIT], &is_active, &name, &hits, &total_time));
        avgr = (double) total_time / (double) hits;

        SHR_IF_ERR_EXIT(utilex_ll_timer_get_info
                        (timers_group, timer_idx[KEY_FIELD_SET], &is_active, &name, &hits_field_set,
                         &total_time_field_set));
        avgr_field_set = (double) total_time_field_set / (double) hits_field_set;

        nof_field_set_per_commit = hits_field_set / hits;

        SHR_IF_ERR_EXIT(utilex_ll_timer_get_info
                        (timers_group, timer_idx[ENTRY_GET_ALL_FIELDS], &is_active, &name, &hits_get, &total_time_get));

        avgr_get = (double) total_time_get / (double) hits_get;

        LOG_CLI((BSL_META
                 ("\n### Tests completed, Commit Results: total time - %d, nof entries - %d, per entry - %.2f[us] Rate - %.0f[E/s] Rate with field set - %.0f[E/s]###\n\n"),
                 total_time, hits, avgr, (1000 * 1000) / avgr,
                 (1000 * 1000) / (avgr + avgr_field_set * nof_field_set_per_commit)));

        /**
        LOG_CLI((BSL_META
                 ("\n### Tests completed, Commit with field set: total time - %d, nof entries - %d, per entry - %.2f[us] Rate - %.0f[E/s] ###\n\n"),
                 total_time_field_set, hits_field_set, avgr_field_set, (1000 * 1000) / avgr_field_set)); */

        LOG_CLI((BSL_META
                 ("\n### Tests completed, Get Results: total time - %d, nof entries - %d, per entry - %.2f[us] Rate - %.0f[E/s] ###\n\n"),
                 total_time_get, hits_get, avgr_get, (1000 * 1000) / avgr_get));

        (*entries_per_sec) = (1000 * 1000) / avgr;
    }

    utilex_ll_timer_stop_all(timers_group);
    /*
     * utilex_ll_timer_print_all(timers_group);
     */
    utilex_ll_timer_clear_all(timers_group);

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
    /** de-allocating all the resources if needed */
    for (entry_iter = 0; entry_iter < nof_entries; entry_iter++)
    {
        for (field_iter = 0; field_iter < table->nof_key_fields; field_iter++)
        {
            if (SHR_BITGET(table->keys_info[field_iter].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR)
                && (!dbal_table_is_allcator_not_needed(unit, table_id)))
            {
                SHR_IF_ERR_EXIT(diag_dbal_alloc_dealloc_value(unit,
                                                              table->keys_info[field_iter].field_id, table_id,
                                                              0,
                                                              DBAL_CORE_ALL,
                                                              entries_array.key_fields[entry_iter][field_iter], 0));
            }
        }
    }

exit:
    if (is_timer_alloc)
    {
        SHR_IF_ERR_CONT(utilex_ll_timer_group_free(timers_group));
    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** performance test using one counter that surrounds the entire flow (all entries) unlike
 *  ctest_dnx_dbal_performance_test that has timers for each field set and entry commit. this test also supports
 *  collect mode with or without waiting for HW to complete process */
static shr_error_e
ctest_dnx_dbal_performance_one_counter_test(
    int unit,
    dbal_tables_e table_id,
    int nof_entries,
    int wait_for_hw,
    uint8 is_collect_mode_enabled,
    int *entries_per_sec)
{
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    uint32 timers_group;
    CONST dbal_logical_table_t *table;
    uint32 entry_handle;
    uint32 commit_flag = DBAL_COMMIT;
    uint32 timer_idx[NOF_TIMERS];
    dbal_entries_array_t entries_array = { {{0}}, {0} };
    int field_iter, entry_iter, ii, nof_entries_to_prepare = nof_entries, estimated_nof_entries = 0;
    int is_timer_alloc = 0, rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (table_id == DBAL_TABLE_FWD_MACT)
    {
        /** for MACT, commit forced is optimized use it */
        commit_flag = DBAL_COMMIT_FORCE;
    }
    (*entries_per_sec) = 0;

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (dbal_table_is_out_lif(table) || (table_id == DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION))
    {
        LOG_CLI((BSL_META("test not supported for table %s\n"), table->table_name));
        SHR_EXIT();
    }

    if (DBAL_TABLE_IS_TCAM(table))
    {
        LOG_CLI((BSL_META("test not supported tcam tables \n")));
        SHR_EXIT();
    }

    if (!(DBAL_TABLE_IS_NONE_DIRECT(table)))
    {
        /** direct tables we are updating the same entry multiple times */
        nof_entries_to_prepare = 1;
    }

    if (nof_entries > CTEST_DBAL_MAX_NOF_ENTRIES_FOR_PERFORMANCE_TEST)
    {
        LOG_CLI((BSL_META("nof entries for test %d exceeded Max nof entries possible using Max %d\n"), nof_entries,
                 CTEST_DBAL_MAX_NOF_ENTRIES_FOR_PERFORMANCE_TEST));
        nof_entries = CTEST_DBAL_MAX_NOF_ENTRIES_FOR_PERFORMANCE_TEST;
        nof_entries_to_prepare = nof_entries;
    }

    SHR_IF_ERR_EXIT(diag_dnx_dbal_table_capacity_estimate(unit, table_id, 0, &estimated_nof_entries));
    if ((nof_entries_to_prepare != 1) && (nof_entries > estimated_nof_entries))
    {
        if (estimated_nof_entries > 0)
        {
            LOG_CLI((BSL_META
                     ("estimated table capacity is smaller than the nof entries, using estimated capactiy %d\n"),
                     estimated_nof_entries));
            nof_entries = estimated_nof_entries;
        }
    }

    for (ii = 0; ii < NOF_TIMERS; ii++)
    {
        timer_idx[ii] = ii + 1;
    }

    sal_strncpy(group_name, "dbal performance", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    SHR_IF_ERR_EXIT(utilex_ll_timer_group_allocate(group_name, &timers_group));
    is_timer_alloc = 1;

    utilex_ll_timer_clear_all(timers_group);

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    if (is_collect_mode_enabled)
    {
        SHR_IF_ERR_EXIT(dbal_tables_indication_set(unit, table_id, DBAL_TABLE_IND_COLLECT_MODE, 1));
    }

    rv = dbal_performance_entries_prepare(unit, table, table_id, nof_entries_to_prepare, &entries_array);
    if (rv)
    {
        LOG_CLI((BSL_META("preparation failed, test skipped\n")));
        SHR_EXIT();
    }

    LOG_CLI((BSL_META
             ("Test started table %s %d entries (%d result fields) collect_mode_enabled = %d waiting for HW %d\n"),
             table->table_name, nof_entries, table->multi_res_info[0].nof_result_fields, is_collect_mode_enabled,
             wait_for_hw));
    utilex_ll_timer_set("entry commit ", timers_group, timer_idx[ENTRY_COMMIT]);

    if (!is_collect_mode_enabled)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle));
    }
    for (entry_iter = 0; entry_iter < nof_entries; entry_iter++)
    {
        int entry_array_id = entry_iter;

        if (nof_entries_to_prepare == 1)
        {
            entry_array_id = 0;
        }
        if (is_collect_mode_enabled)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle));
        }

        for (field_iter = 0; field_iter < table->nof_key_fields; field_iter++)
        {
            dbal_entry_key_field_arr32_set(unit, entry_handle,
                                           table->keys_info[field_iter].field_id,
                                           &(entries_array.key_fields[entry_array_id][field_iter]));
        }

        for (field_iter = 0; field_iter < table->multi_res_info[0].nof_result_fields; field_iter++)
        {
            if (table->multi_res_info[0].results_info[field_iter].permission != DBAL_PERMISSION_READONLY)
            {
                if (table->multi_res_info[0].results_info[field_iter].arr_prefix_size == 0)
                {
                    dbal_entry_value_field_arr32_set(unit, entry_handle,
                                                     table->multi_res_info[0].results_info[field_iter].field_id,
                                                     INST_SINGLE, &(entries_array.result_fields[field_iter]));
                }
                else
                {
                    if (SHR_BITGET
                        (table->multi_res_info[0].results_info[field_iter].field_indication_bm,
                         DBAL_FIELD_IND_IS_FIELD_ENUM))
                    {
                        dbal_entry_value_field_arr32_set(unit, entry_handle,
                                                         table->multi_res_info[0].results_info[field_iter].field_id,
                                                         INST_SINGLE, &(entries_array.result_fields[field_iter]));
                    }
                    else
                    {
                        dbal_entry_value_field_predefine_value_set(unit, entry_handle,
                                                                   table->multi_res_info[0].
                                                                   results_info[field_iter].field_id, INST_SINGLE,
                                                                   DBAL_PREDEF_VAL_MIN_VALUE);
                    }
                }
            }
        }
        rv = dbal_entry_commit(unit, entry_handle, commit_flag);

        if (is_collect_mode_enabled && (entry_iter == nof_entries - 1) && wait_for_hw)
        {
            /** last entry wait for the pool to be processed */
            SHR_IF_ERR_EXIT(dbal_collection_trigger_set(unit, DBAL_COLLECTION_TRIGGER_PROCESS_AND_WAIT));
        }
    }

    utilex_ll_timer_stop(timers_group, timer_idx[ENTRY_COMMIT]);

    {
        char *name;
        uint32 total_time, hits;
        int is_active;
        double avgr;

        SHR_IF_ERR_EXIT(utilex_ll_timer_get_info
                        (timers_group, timer_idx[ENTRY_COMMIT], &is_active, &name, &hits, &total_time));
        avgr = (double) total_time / (double) nof_entries;
        (*entries_per_sec) = (1000 * 1000) / avgr;

        LOG_CLI((BSL_META
                 ("\n### Tests completed, Total time - %d, nof entries - %d, per entry - %.2f[us] Rate - %.0f[E/s] ###\n\n"),
                 total_time, nof_entries, avgr, (1000 * 1000) / avgr));
    }

    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);

    if (is_collect_mode_enabled)
    {
        SHR_IF_ERR_EXIT(dbal_tables_indication_set(unit, table_id, DBAL_TABLE_IND_COLLECT_MODE, 0));
    }

    if (!wait_for_hw)
    {
        /** if we didn't wait for HW we cannot perform table clear at the end of the test */
        SHR_IF_ERR_EXIT(dbal_collection_trigger_set(unit, DBAL_COLLECTION_TRIGGER_PROCESS_AND_WAIT));
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
    }

    /** de-allocating all the resources if needed */
    for (entry_iter = 0; entry_iter < nof_entries; entry_iter++)
    {
        for (field_iter = 0; field_iter < table->nof_key_fields; field_iter++)
        {
            if (SHR_BITGET(table->keys_info[field_iter].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR)
                && (!dbal_table_is_allcator_not_needed(unit, table_id)))
            {
                SHR_IF_ERR_EXIT(diag_dbal_alloc_dealloc_value(unit,
                                                              table->keys_info[field_iter].field_id, table_id,
                                                              0,
                                                              DBAL_CORE_ALL,
                                                              entries_array.key_fields[entry_iter][field_iter], 0));
            }
        }
    }

exit:
    if (is_timer_alloc)
    {
        SHR_IF_ERR_CONT(utilex_ll_timer_group_free(timers_group));
    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief test that check the performance of entry commit/get/table clear. this test receives dbal table as a
 * paramter and nof entries (optional, default 4k) and perform commit get table clear. prints at the end of the test the
 * times.
 */

static shr_error_e
cmd_dnx_dbal_performance(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 nof_entries;
    char *table_name = NULL;
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    int pre_process_done = 0, entries_per_sec = 0;
    uint8 is_commit_update = 0;
    uint32 mode = 0, collect_mode = 0;
    int nof_tables_mdb = (sizeof(dbal_performance_test_list_mdb)) / (sizeof(dbal_tables_e));
    int nof_tables_hl = (sizeof(dbal_performance_test_list_hl)) / (sizeof(dbal_tables_e));
    int nof_tables_sw = (sizeof(dbal_performance_test_list_sw)) / (sizeof(dbal_tables_e));
    CONST dbal_logical_table_t *table;
    char *access = NULL;
    dbal_access_method_e access_method = DBAL_NOF_ACCESS_METHODS;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_UINT32("EntryNum", nof_entries);
    SH_SAND_GET_BOOL("update", is_commit_update);
    SH_SAND_GET_UINT32("mode", mode);
    SH_SAND_GET_BOOL("collect", collect_mode);

    if (mode > 2)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "ilegal valu for mode should be 0/1/2 \n");
    }

    if (collect_mode && (DBAL_COLLECTION_POOL_SIZE == 0))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "cannot work in collect mode since DBAL_COLLECTION_POOL_SIZE is 0\n");
    }

    if (!ISEMPTY(table_name))
    {
        if (dbal_logical_table_string_to_id_no_error(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(dbal_print_table_names(unit, table_name));
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }
    else
    {
        table_id = DBAL_TABLE_EMPTY;
        if (SAL_BOOT_PLISIM)
        {
            SHR_ERR_EXIT(_SHR_E_NONE, "Test is not supported in simulation \n");
        }
        if ((dnx_data_device.general.nof_cores_get(unit) == 1))
        {
            SHR_ERR_EXIT(_SHR_E_NONE, "Test not supported for 1 core devices\n");
        }
        SH_SAND_GET_STR("access", access);

        if (!ISEMPTY(access))
        {
            SHR_IF_ERR_EXIT(dbal_access_method_string_to_id(unit, access, &access_method));
            if ((access_method == DBAL_ACCESS_METHOD_TCAM_CS) ||
                (access_method == DBAL_ACCESS_METHOD_KBP) || (access_method == DBAL_ACCESS_METHOD_PEMLA))
            {
                LOG_CLI((BSL_META("unsupported access method %s\n"), access));
                SHR_EXIT();
            }
        }
    }

    /** disable all threads */
    CTEST_DNX_DBAL_SH_CMD_EXEC("linkscan off");
    CTEST_DNX_DBAL_SH_CMD_EXEC("counter off");
    CTEST_DNX_DBAL_SH_CMD_EXEC("deinit rx_los");
    CTEST_DNX_DBAL_SH_CMD_EXEC("deinit interrupt");
    CTEST_DNX_DBAL_SH_CMD_EXEC("crps detach");
    CTEST_DNX_DBAL_SH_CMD_EXEC("ctrp detach");

    /** disable error recovery   */
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    pre_process_done = 1;

    PRT_TITLE_SET("DBAL performance test result");
    PRT_COLUMN_ADD("Table Name");
    PRT_COLUMN_ADD("Access Method");
    PRT_COLUMN_ADD("Table type");
    PRT_COLUMN_ADD("Nof Result fields");
    PRT_COLUMN_ADD("Payload Size in bits");
    PRT_COLUMN_ADD("Pntries per sec");
    PRT_COLUMN_ADD("Time per entry (usec)");

    if (table_id == DBAL_TABLE_EMPTY)
    {
        dbal_tables_e table_iter;

        if ((access_method == DBAL_ACCESS_METHOD_MDB) || (access_method == DBAL_NOF_ACCESS_METHODS))
        {
            for (table_iter = 0; table_iter < nof_tables_mdb; table_iter++)
            {
                /** running over list of validated tables, dont allow tests to fail */
                SHR_IF_ERR_EXIT(ctest_dnx_dbal_performance_test
                                (unit, dbal_performance_test_list_mdb[table_iter], nof_entries, is_commit_update,
                                 &entries_per_sec));
                SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_performance_test_list_mdb[table_iter], &table));
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", table->table_name);
                PRT_CELL_SET("%s", dbal_access_method_to_string(unit, table->access_method));
                PRT_CELL_SET("%s", dbal_table_type_to_string(unit, table->table_type));
                PRT_CELL_SET("%d", table->multi_res_info[0].nof_result_fields);
                PRT_CELL_SET("%d", table->multi_res_info[0].entry_payload_size);
                PRT_CELL_SET("%d", entries_per_sec);
                PRT_CELL_SET("%.2f", ((double) (1000000 / entries_per_sec)));
            }
        }

        if ((access_method == DBAL_ACCESS_METHOD_HARD_LOGIC) || (access_method == DBAL_NOF_ACCESS_METHODS))
        {
            for (table_iter = 0; table_iter < nof_tables_hl; table_iter++)
            {
                /** running over list of validated tables, dont allow tests to fail */
                SHR_IF_ERR_EXIT(ctest_dnx_dbal_performance_test
                                (unit, dbal_performance_test_list_hl[table_iter], nof_entries, is_commit_update,
                                 &entries_per_sec));
                SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_performance_test_list_hl[table_iter], &table));
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", table->table_name);
                PRT_CELL_SET("%s", dbal_access_method_to_string(unit, table->access_method));
                PRT_CELL_SET("%s", dbal_table_type_to_string(unit, table->table_type));
                PRT_CELL_SET("%d", table->multi_res_info[0].nof_result_fields);
                PRT_CELL_SET("%d", table->multi_res_info[0].entry_payload_size);
                PRT_CELL_SET("%d", entries_per_sec);
                PRT_CELL_SET("%.2f", ((double) (1000000 / entries_per_sec)));
            }
        }

        if ((access_method == DBAL_ACCESS_METHOD_SW_STATE) || (access_method == DBAL_NOF_ACCESS_METHODS))
        {
            for (table_iter = 0; table_iter < nof_tables_sw; table_iter++)
            {
                /** running over list of validated tables, dont allow tests to fail */
                SHR_IF_ERR_EXIT(ctest_dnx_dbal_performance_test
                                (unit, dbal_performance_test_list_sw[table_iter], nof_entries, is_commit_update,
                                 &entries_per_sec));
                SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_performance_test_list_sw[table_iter], &table));
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", table->table_name);
                PRT_CELL_SET("%s", dbal_access_method_to_string(unit, table->access_method));
                PRT_CELL_SET("%s", dbal_table_type_to_string(unit, table->table_type));
                PRT_CELL_SET("%d", table->multi_res_info[0].nof_result_fields);
                PRT_CELL_SET("%d", table->multi_res_info[0].entry_payload_size);
                PRT_CELL_SET("%d", entries_per_sec);
                PRT_CELL_SET("%.2f", ((double) (1000000 / entries_per_sec)));
            }
        }
    }
    else
    {
        if (mode == 0)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_dbal_performance_one_counter_test
                            (unit, table_id, nof_entries, mode, collect_mode, &entries_per_sec));
        }
        else
        {
            SHR_IF_ERR_EXIT(ctest_dnx_dbal_performance_test
                            (unit, table_id, nof_entries, is_commit_update, &entries_per_sec));
        }
    }

    PRT_COMMITX;

exit:
    if (pre_process_done)
    {
        /** Enable error recovery */
        DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
        CTEST_DNX_DBAL_SH_CMD_EXEC("crps attach");
        CTEST_DNX_DBAL_SH_CMD_EXEC("ctrp attach");
        CTEST_DNX_DBAL_SH_CMD_EXEC("init interrupt");
        CTEST_DNX_DBAL_SH_CMD_EXEC("init rx_los");
        CTEST_DNX_DBAL_SH_CMD_EXEC("counter on");
        CTEST_DNX_DBAL_SH_CMD_EXEC("linkscan on");
    }

    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** trying to fill a table according to the nof_entries, if nof_entries=0 used the table capacity if test_mode=1
 *  tring to fill the table to maximum */
static shr_error_e
ctest_dbal_capacity_test(
    int unit,
    dbal_tables_e table_id,
    int result_type,
    int test_mode,
    int nof_entries)
{
    CONST dbal_logical_table_t *table;
    uint32 entry_handle;
    int entries_iter, field_iter;
    int nof_committed_entries_sw;
    dbal_fields_e field_to_inc_id = DBAL_FIELD_EMPTY;
    int field_to_inc_size = 0;
    uint32 field_to_inc_val = 0;
    uint32 key_fields_val[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { 0 };
    int estimated_nof_entries = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    if (table->allocator_field_id != DBAL_FIELD_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table with allocator field (%s)are not supported \n",
                     dbal_field_to_string(unit, table->allocator_field_id));
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    if (!test_mode)
    {
        /** checking what is the estimated capacity */
        SHR_IF_ERR_EXIT(diag_dnx_dbal_table_capacity_estimate(unit, table_id, result_type, &estimated_nof_entries));

        /** nof_entries is an input from user, if 0 it will choose calc_nof_entries, if user gives value too high */
        if (nof_entries == 0)
        {
            nof_entries = estimated_nof_entries;
        }
        else if (nof_entries > estimated_nof_entries)
        {
            LOG_CLI((BSL_META("nof entries received is too big (%d), using (%d) entries instead \n"), nof_entries,
                     estimated_nof_entries));
            nof_entries = estimated_nof_entries;
        }

        if (nof_entries == 0)
        {
            LOG_CLI((BSL_META("essitmated capacity for the table is 0 cannot process test \n")));
            SHR_EXIT();
        }

        LOG_CLI((BSL_META("Starting Capcity test for table %s, adding %d entries\n"),
                 dbal_logical_table_to_string(unit, table_id), nof_entries));
    }
    else
    {
        /** nof_entries in test mode is 2,000,000 this is a teorethical number.. there is no table that can reach this amount
         *  the iteration should stop before reach to this number... */
        nof_entries = 2000000;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle));

    /** prepare the result fields values, all entries will be the same but not default entries.. */
    for (field_iter = 0; field_iter < table->multi_res_info[0].nof_result_fields; field_iter++)
    {
        dbal_fields_e curr_field_id = table->multi_res_info[0].results_info[field_iter].field_id;
        if (curr_field_id == DBAL_FIELD_RESULT_TYPE)
        {
            dbal_entry_value_field32_set(unit, entry_handle, curr_field_id, INST_SINGLE, 0);
        }
        else
        {
            int32 field_value = field_iter;
            if (table->multi_res_info[0].results_info[field_iter].arr_prefix_size != 0)
            {
                dbal_entry_value_field_predefine_value_set(unit, entry_handle, curr_field_id, INST_SINGLE,
                                                           DBAL_PREDEF_VAL_MIN_VALUE);
            }
            else
            {
                if (table->multi_res_info[0].results_info[field_iter].max_value < field_value)
                {
                    field_value = table->multi_res_info[0].results_info[field_iter].max_value;
                }
                if (table->multi_res_info[0].results_info[field_iter].min_value > field_value)
                {
                    field_value = table->multi_res_info[0].results_info[field_iter].min_value;
                }
                dbal_entry_value_field32_set(unit, entry_handle, curr_field_id, INST_SINGLE, field_value);
            }
        }
    }

    /** looking for the biggest key field currently this logic is not good for direct tables since it is
     *  working only on one key field (direct tables all renage is valid). */
    for (field_iter = 0; field_iter < table->nof_key_fields; field_iter++)
    {
        int field_size;
        dbal_fields_e curr_field_id = table->keys_info[field_iter].field_id;
        SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_id, curr_field_id, TRUE, 0, INST_SINGLE, &field_size));
        if (field_to_inc_size < field_size)
        {
            field_to_inc_id = curr_field_id;
            field_to_inc_size = field_size;
            field_to_inc_val = table->keys_info[field_iter].min_value;
            key_fields_val[0] = field_to_inc_val;
        }
        /** setting all key fields to the min value */
        dbal_entry_key_field_predefine_value_set(unit, entry_handle, curr_field_id, DBAL_PREDEF_VAL_MIN_VALUE);

    }

    if (!test_mode && (utilex_max_value_by_size(field_to_inc_size) < nof_entries))
    {
        if (field_to_inc_size < 32)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "not enough different entries to add to the table %d out of %d \n",
                         utilex_max_value_by_size(field_to_inc_size), nof_entries);
        }
    }

    /** commiting the first entry */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle, DBAL_COMMIT));

    /** adding the entries */
    for (entries_iter = 1; entries_iter < nof_entries; entries_iter++)
    {
        int rv;
        uint8 is_illegal_value_for_field;

        field_to_inc_val++;
        SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value
                        (unit, field_to_inc_id, field_to_inc_val, &is_illegal_value_for_field));
        if (is_illegal_value_for_field)
        {
            continue;
        }

        key_fields_val[0] = field_to_inc_val;
        dbal_entry_key_field_arr32_set(unit, entry_handle, field_to_inc_id, key_fields_val);

        rv = dbal_entry_commit(unit, entry_handle, DBAL_COMMIT);
        if (rv != _SHR_E_NONE)
        {
            if (test_mode)
            {
                LOG_CLI((BSL_META("\nTest mode - Nof entries added to table until reach error %d\n"), entries_iter));
                SHR_EXIT();
            }
            else
            {
                LOG_CLI((BSL_META("Failed to commit entry %d capacity was not reached to expected %d\n"), entries_iter,
                         nof_entries));
                SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
                SHR_IF_ERR_EXIT(rv);
            }
        }
    }

    /****** test outputs ******/
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.get(unit, table_id, &nof_committed_entries_sw));

    if (nof_committed_entries_sw != nof_entries)
    {
        if (DBAL_TABLE_IS_NONE_DIRECT(table))
        {
            LOG_CLI((BSL_META("WARNING: nof_entries %d != nof_committed_entries_sw = %d\n"), nof_entries,
                     nof_committed_entries_sw));
        }
    }

    LOG_CLI((BSL_META
             ("Capcity test completed table %s nof_entries added %d nof_entries in SW = %d\n\n"),
             dbal_logical_table_to_string(unit, table_id), nof_entries, nof_committed_entries_sw));

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnx_dbal_capacity(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *table_name = NULL;
    uint32 nof_entries = 0;
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    uint8 test_mode = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_UINT32("EntryNum", nof_entries);
    SH_SAND_GET_BOOL("test", test_mode);

    if (!ISEMPTY(table_name))
    {
        if (dbal_logical_table_string_to_id_no_error(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(dbal_print_table_names(unit, table_name));
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
        /** running the test for specific table */
        SHR_IF_ERR_EXIT(ctest_dbal_capacity_test(unit, table_id, 0, test_mode, nof_entries));
    }
    else
    {
        /** no specific table, run over list of pre defined tables */
        int rv = _SHR_E_NONE;
        int nof_tables = (sizeof(dbal_capacity_test_list)) / (sizeof(dbal_tables_e));
        int ii;
        for (ii = 0; ii < nof_tables; ii++)
        {
            rv |= ctest_dbal_capacity_test(unit, dbal_capacity_test_list[ii], 0, test_mode, nof_entries);
        }

        if (rv)
        {
            SHR_ERR_EXIT(rv, "test failed\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief run logical table test in ctest with error recovery cleanup
 */
static shr_error_e
cmd_dnx_dbal_ctest_err(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_ltt_flags_e flags = LTT_FLAGS_ERR_RECOVERY_CLEAN_UP | LTT_FLAGS_TABLE_CLEAR_END_OF_TEST;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logical_tables_test(unit, args, sand_control, "error_recovery", flags));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief run logical table negative test for snapshot manager
 */
static shr_error_e
cmd_dnx_dbal_ctest_snapshot(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_ltt_flags_e flags = LTT_FLAGS_SNAPSHOT_NEGATIVE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_logical_tables_test(unit, args, sand_control, "snapshot_manager", flags));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
cmd_dbal_dyn_ut_test_names_creation(
    int unit)
{
    int test_idx, nof_tests;

    SHR_FUNC_INIT_VARS(unit);

    nof_tests = sizeof(dnx_dbal_ut_all_tests) / sizeof(dbal_ut_test_info_t);

    /** crteating the enum to string mapping */
    for (test_idx = 0; test_idx < nof_tests; test_idx++)
    {
        dbal_ut_enum_table_dynamic[test_idx].value = dnx_dbal_ut_all_tests[test_idx].test_idx;
        dbal_ut_enum_table_dynamic[test_idx].string = dnx_dbal_ut_all_tests[test_idx].test_name;
    }

    SHR_FUNC_EXIT;
}

/** this function run a ut test */
static shr_error_e
cmd_dnx_dbal_ut(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    char *table_name;
    dbal_ut_tests_e input_test_indx;
    char *dbal_ut_empty_table = "DBAL_TABLE_EMPTY";
    int nof_tests = sizeof(dnx_dbal_ut_all_tests) / sizeof(dbal_ut_test_info_t);
    int test_pos;

    SHR_FUNC_INIT_VARS(unit);

    /** Get input parameters */
    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_ENUM("Name", input_test_indx);

    if (!ISEMPTY(table_name))
    {
        if (sal_strcmp(table_name, dbal_ut_empty_table) != 0)
        {
            if (dbal_logical_table_string_to_id_no_error(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
            {
                SHR_IF_ERR_EXIT(dbal_print_table_names(unit, table_name));
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }
    }

    for (test_pos = 0; test_pos < nof_tests; test_pos++)
    {
        if (dnx_dbal_ut_all_tests[test_pos].test_idx == input_test_indx)
        {
            SHR_IF_ERR_EXIT(dnx_dbal_ut_all_tests[test_pos].test_cb(unit, table_id));
            LOG_CLI((BSL_META("test %s table %s completed\n"), dnx_dbal_ut_all_tests[test_pos].test_name, table_name));
            break;
        }
    }

    if (test_pos == nof_tests)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Test case %d is not exists\n", input_test_indx);
    }

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_dbal_test_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "DBAL logical table name", ""},
    {"flags", SAL_FIELD_TYPE_STR, "DBAL logical table name flags", ""},
    {"EntryNum", SAL_FIELD_TYPE_UINT32, "Number of entries to run the test. 0 means internal default value", "0"},
    {"ACCess", SAL_FIELD_TYPE_STR, "DBAL logical table access", ""},
    {"TYpe", SAL_FIELD_TYPE_STR, "DBAL logical table type", ""},
    {NULL}
};

static sh_sand_option_t dnx_dbal_performance_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "DBAL logical table name", ""},
    {"EntryNum", SAL_FIELD_TYPE_UINT32, "Number of entries", "16384"},
    {"ACCess", SAL_FIELD_TYPE_STR, "DBAL logical table access", ""},
    {"update", SAL_FIELD_TYPE_BOOL, "if update = true, will perform commit_update (for non-direct tables only)", "No"},
    {"mode", SAL_FIELD_TYPE_UINT32,
     " mode=0, (one timer without waiting for HW), mode=1 (one timer wait to HW complete), mode=2 will use measurements with profiling (old) otherwise ",
     "0"},
    {"collect", SAL_FIELD_TYPE_BOOL, "collect mode, if == 1, dbal works in multithread ", "No"},
    {NULL}
};

static sh_sand_option_t dnx_dbal_capacity_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "DBAL logical table name", ""},
    {"EntryNum", SAL_FIELD_TYPE_UINT32, "Number of entries", "0"},
    {"TeST", SAL_FIELD_TYPE_BOOL, "test mode on (adding entries until exposed", "No"},
    {NULL}
};

/**
 * \brief consist a mapping between the name of the test and the corresponding enum value. it is initialized dynamically
 *        according to the ut struct.
 */

static sh_sand_option_t dnx_dbal_ut_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "DBAL logical table name", ""},
    {"Name", SAL_FIELD_TYPE_ENUM, "DBAL unit test name", "update_before_commit", (void *) dbal_ut_enum_table_dynamic},
    {NULL}
};

static sh_sand_man_t dnx_dbal_ut_test_man = {
    "DBAL logical table test - unit tests for DBAL module",
    "ctest dbal ut name=<test_name> table=<table_name>",
    "ctest dbal ut name=lpm_mask table=IPV4_MULTICAST_PRIVATE_LPM_FORWARD"
};

static sh_sand_man_t dnx_dbal_performance_man = {
    "DBAL performance test for table, priori to the mesurments closing error recovery and threads",
    "ctest dbal performance table=<table_name>"
};

static sh_sand_man_t dnx_dbal_capacity_man = {
    "DBAL capacity test for table, validate that the table can be populate according to capacity estimation. "
        "can be run over specific table or over the default list of tables (when not passing the table argument). "
        "Normaly this test should test non-dirct tables.\n",
    "ctest dbal capacity table=<table_name>" "ctest dbal capacity"
};

static sh_sand_man_t dnx_dbal_test_man = {
    "DBAL logical table test - a basic set/get/iterator logical test per table (with flags)",
    "Flags Usage:flags=<flags_value> (parsed as bitmap)\n"
        "Flags Options:\n"
        "\tFlags=0  - default. set/get/delte up to 32 entries\n"
        "\tFlags=1  - Perform iterator test\n"
        "\tFlags=2  - Clear table and verify table clear at the end of the test\n"
        "\tFlags=4  - Keep entries in table, skip delete\n"
        "\tFlags=8  - Regression mode (Low severity, only matured tables)\n"
        "\tFlags=16 - Iterator + Table Clear full test (In regression mode - only tables from list)\n"
        "\tFlags=64 - Perform measurements (performance test)\n"
        "\tFlags=128 - Use error recovery for cleanup after test\n"
        "\tFlags=256 - Tables containing multiple results only\n"
        "\tFlags=512 - Tables containing sw fields only\n"
        "\tFlags=1024 - Run snapshot manager negative testing before test clear\n"
        "dbal TaBLe TaBLe=<tabel name> FLaGs=<flags_value>",
    "tbl=EXAMPLE_TABLE_ILLEGAL_VALUES flags=0x10"
};

/** dynamic init of dbal ut tests */
static shr_error_e
cmd_dbal_dyn_ut_test_creation(
    int unit,
    rhlist_t * test_list)
{
    int test_idx, table_idx, nof_tests;
    char *dbal_ut_empty_table = "DBAL_TABLE_EMPTY";
    int is_internal_tables_valid = 0;

    SHR_FUNC_INIT_VARS(unit);


    nof_tests = sizeof(dnx_dbal_ut_all_tests) / sizeof(dbal_ut_test_info_t);

    /** creating the test list from the dnx_dbal_ut_all_tests */
    for (test_idx = 0; test_idx < nof_tests; test_idx++)
    {
        for (table_idx = 0; table_idx < DBAL_MAX_NUM_OF_TABLES_PER_TEST; table_idx++)
        {
            char ut_name[DBAL_MAX_STRING_LENGTH + 1] = "";
                                                              /** consist the test name only */
            char ut_args[DBAL_MAX_STRING_LENGTH * 2 + 14] = "name="; /** consist the full args to run the test name=xxx table=yyy*/
            char ut_table[DBAL_MAX_STRING_LENGTH + 7] = " table="; /** consist the table to run the test table=yyy */
            int ut_flags;
            int source_len, dest_len;
            char *source_str;

            source_len =
                sal_strnlen(dnx_dbal_ut_all_tests[test_idx].test_name,
                            sizeof(dnx_dbal_ut_all_tests[test_idx].test_name));
            dest_len = sal_strnlen(ut_name, sizeof(ut_name));
            if ((source_len + dest_len) > (sizeof(ut_name) - 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Size of container (ut_name %d) is too small to contain both source (%d) and destination (%d). Quit.\n",
                             (int) sizeof(ut_name), source_len, dest_len);
            }
            sal_strncat_s(ut_name, dnx_dbal_ut_all_tests[test_idx].test_name, sizeof(ut_name));
            source_len =
                sal_strnlen(dnx_dbal_ut_all_tests[test_idx].test_name,
                            sizeof(dnx_dbal_ut_all_tests[test_idx].test_name));
            dest_len = sal_strnlen(ut_args, sizeof(ut_args));
            if ((source_len + dest_len) > (sizeof(ut_args) - 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "A. Size of container (ut_args %d) is too small to contain both source (%d) and destination (%d). Quit.\n",
                             (int) sizeof(ut_args), source_len, dest_len);
            }
            sal_strncat_s(ut_args, dnx_dbal_ut_all_tests[test_idx].test_name, sizeof(ut_args));

            if (dnx_dbal_ut_all_tests[test_idx].table_ids[table_idx] != DBAL_TABLE_EMPTY)
            {
                source_str = dbal_logical_table_to_string(unit, dnx_dbal_ut_all_tests[test_idx].table_ids[table_idx]);
                source_len = sal_strnlen(source_str, sizeof(ut_table));
                dest_len = sal_strnlen(ut_table, sizeof(ut_table));
                if ((source_len + dest_len) > (sizeof(ut_table) - 1))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Size of container (ut_table %d) is too small to contain both source (%d) and destination (%d). Quit.\n",
                                 (int) sizeof(ut_table), source_len, dest_len);
                }
                sal_strncat_s(ut_table, source_str, sizeof(ut_table));
            }
            else
            {
                if (table_idx != 0)
                {
                    break;
                }
                sal_strncat_s(ut_table, dbal_ut_empty_table, sizeof(ut_table));
            }

            /** incase we have more then 1 table for a specific test, adding prefix to the name of the test ( test names should be
             *  uniqe) */
            if (table_idx != 0)
            {
                sal_snprintf(ut_name, DBAL_MAX_STRING_LENGTH * 2, "%s_%d ", ut_name, table_idx);
            }

            ut_flags = dnx_dbal_ut_all_tests[test_idx].ctest_flags;

            source_len = sal_strnlen(ut_table, sizeof(ut_table));
            dest_len = sal_strnlen(ut_args, sizeof(ut_args));
            if ((source_len + dest_len) > (sizeof(ut_args) - 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "B. Size of container (ut_args %d) is too small to contain both source (%d) and destination (%d). Quit.\n",
                             (int) sizeof(ut_args), source_len, dest_len);
            }
            sal_strncat_s(ut_args, ut_table, sizeof(ut_args));

            /** this part removing all example tables after senitization */
            if (!is_internal_tables_valid)
            {
                if (strcaseindex(ut_table, "EXAMPLE_"))
                {
                    continue;
                }
            }

            /** skip tables by maturity level */
            if (dnx_dbal_ut_all_tests[test_idx].table_ids[table_idx] != DBAL_TABLE_EMPTY)
            {
                CONST dbal_logical_table_t *table;
                SHR_IF_ERR_EXIT(dbal_tables_table_get
                                (unit, dnx_dbal_ut_all_tests[test_idx].table_ids[table_idx], &table));
                if (!CTEST_DNX_DBAL_IS_MATURITY_LEVEL_HIGH(table->maturity_level))
                {
                    LOG_CLI((BSL_META("test %s table %s skipped because of ML\n"),
                             dnx_dbal_ut_all_tests[test_idx].test_name, dbal_logical_table_to_string(unit,
                                                                                                     dnx_dbal_ut_all_tests
                                                                                                     [test_idx].table_ids
                                                                                                     [table_idx])));
                    continue;
                }
            }

            if ((dnx_dbal_ut_all_tests[test_idx].device_type == UT_DUAL_CORE_DEVICES)
                && (dnx_data_device.general.nof_cores_get(unit) == 1))
            {
                LOG_CLI((BSL_META("test %s table %s skipped because of it only supported for Dual core devices\n"),
                         dnx_dbal_ut_all_tests[test_idx].test_name, dbal_logical_table_to_string(unit,
                                                                                                 dnx_dbal_ut_all_tests
                                                                                                 [test_idx].table_ids
                                                                                                 [table_idx])));
                continue;
            }

            SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, ut_name, ut_args, ut_flags), "Add test failed");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** in init of dbal */
static shr_error_e
cmd_dbal_dyn_ltt_test_creation_internal(
    int unit,
    dbal_ltt_dyn_flags_e dynamic_flags,
    rhlist_t * test_list)
{
    CONST dbal_logical_table_t *table;
    uint32 table_id;

    SHR_FUNC_INIT_VARS(unit);

    for (table_id = 0; table_id < DBAL_NOF_TABLES; table_id++)
    {
        int flags;
        char params_ltt[DBAL_MAX_STRING_LENGTH + 6] = "table=";

        /*
         * When MDB config init flag is provided run only the tables in the list
         * and skip them if the flag is not provided.
         */
        if (dbal_ltt_mdb_cfg_table_list(unit, table_id) !=
            _SHR_IS_FLAG_SET(dynamic_flags, LTT_DYN_FLAGS_MDB_CONFIG_INIT))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
        {
            if (!CTEST_DNX_DBAL_IS_MATURITY_LEVEL_HIGH(table->maturity_level))
            {
                /** Tables with low and partially functional maturity level */
                continue;
            }
        }
        /** check if the table has at least one readable field */
        if (!dbal_ltt_is_readable_result_field(unit, table))
        {
            continue;
        }
        sal_strcat(params_ltt, dbal_logical_table_to_string(unit, table_id));

        if (_SHR_IS_FLAG_SET(dynamic_flags, LTT_DYN_FLAGS_FULL_TEST))
        {
            /** Set the precommit flag according to the full table list */
            flags = (dbal_ltt_full_unitest_table_list(unit, table_id) ? CTEST_PRECOMMIT : 0);
        }
        else
        {
            /** No precommit testing for KBP tables on real device */
            flags = (((table->access_method == DBAL_ACCESS_METHOD_KBP) && !SAL_BOOT_PLISIM) ? 0 : CTEST_PRECOMMIT);

            /** No precommit testing for MDB tables */
            if (_SHR_IS_FLAG_SET(dynamic_flags, LTT_DYN_FLAGS_MDB_CONFIG_INIT))
            {
                flags = CTEST_POSTCOMMIT;
            }
        }

        if (_SHR_IS_FLAG_SET(dynamic_flags, LTT_DYN_FLAGS_RANDOM_INIT))
        {
            flags |= CTEST_RANDOM;
        }
        SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, dbal_logical_table_to_string(unit, table_id), params_ltt,
                                             flags), "Add test failed");
    }

    /** after the tlist created need to disable all threads to run the tlist */
#if !defined(ADAPTER_SERVER_MODE)
    CTEST_DNX_DBAL_SH_CMD_EXEC("linkscan off");
    CTEST_DNX_DBAL_SH_CMD_EXEC("counter off");
    CTEST_DNX_DBAL_SH_CMD_EXEC("deinit rx_los");
    CTEST_DNX_DBAL_SH_CMD_EXEC("deinit interrupt");
    CTEST_DNX_DBAL_SH_CMD_EXEC("crps detach");
#endif

exit:
    SHR_FUNC_EXIT;
}

/** in init of dbal   */
static shr_error_e
cmd_dbal_dyn_ltt_test_creation(
    int unit,
    rhlist_t * test_list)
{
    dbal_ltt_dyn_flags_e flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(cmd_dbal_dyn_ltt_test_creation_internal(unit, flags, test_list));

exit:
    SHR_FUNC_EXIT;
}

/** in init of dbal   */
static shr_error_e
cmd_dbal_dyn_ltt_full_test_creation(
    int unit,
    rhlist_t * test_list)
{
    dbal_ltt_dyn_flags_e flags = LTT_DYN_FLAGS_FULL_TEST;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(cmd_dbal_dyn_ltt_test_creation_internal(unit, flags, test_list));

exit:
    SHR_FUNC_EXIT;
}

/** in init of dbal */
static shr_error_e
cmd_dbal_dyn_ltt_skip_shadow_test_creation(
    int unit,
    rhlist_t * test_list)
{
    CONST dbal_logical_table_t *table;
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                    (unit, table_id, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_MDB, DBAL_PHYSICAL_TABLE_NONE,
                     DBAL_TABLE_TYPE_NONE, &table_id));

    while (table_id != DBAL_TABLE_EMPTY)
    {
        char params_ltt[DBAL_MAX_STRING_LENGTH + 6] = "table=";
        int flags;

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
        if (!CTEST_DNX_DBAL_IS_MATURITY_LEVEL_HIGH(table->maturity_level))
        {
            SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                            (unit, table_id, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_MDB, DBAL_PHYSICAL_TABLE_NONE,
                             DBAL_TABLE_TYPE_NONE, &table_id));
            continue;
        }

        /** check if the table has at least one readable field */
        if (!dbal_ltt_is_readable_result_field(unit, table))
        {
            SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                            (unit, table_id, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_MDB, DBAL_PHYSICAL_TABLE_NONE,
                             DBAL_TABLE_TYPE_NONE, &table_id));
            continue;
        }
        sal_strncat_s(params_ltt, dbal_logical_table_to_string(unit, table_id), sizeof(params_ltt));

        flags = CTEST_POSTCOMMIT;
        SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, dbal_logical_table_to_string(unit, table_id), params_ltt,
                                             flags), "Add test failed");

        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                        (unit, table_id, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_MDB, DBAL_PHYSICAL_TABLE_NONE,
                         DBAL_TABLE_TYPE_NONE, &table_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/** in init of dbal */
shr_error_e
cmd_dbal_dyn_ltt_mdb_cfg_test_creation(
    int unit,
    rhlist_t * test_list)
{
    dbal_ltt_dyn_flags_e flags = LTT_DYN_FLAGS_MDB_CONFIG_INIT;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(cmd_dbal_dyn_ltt_test_creation_internal(unit, flags, test_list));

exit:
    SHR_FUNC_EXIT;
}

/** in init of dbal */
shr_error_e
cmd_dbal_dyn_ltt_full_mdb_cfg_test_creation(
    int unit,
    rhlist_t * test_list)
{
    dbal_ltt_dyn_flags_e flags = LTT_DYN_FLAGS_MDB_CONFIG_INIT | LTT_DYN_FLAGS_FULL_TEST;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(cmd_dbal_dyn_ltt_test_creation_internal(unit, flags, test_list));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dbal_dyn_error_recovery_test_creation(
    int unit,
    rhlist_t * test_list)
{
    int flags = 0;
    CONST dbal_logical_table_t *table;
    uint32 table_id;

    SHR_FUNC_INIT_VARS(unit);

    for (table_id = 0; table_id < DBAL_NOF_TABLES; table_id++)
    {
        char params_ltt[DBAL_MAX_STRING_LENGTH + 6] = "table=";

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
        if (!CTEST_DNX_DBAL_IS_MATURITY_LEVEL_HIGH(table->maturity_level))
        {
            continue;
        }
        /** check if the table has at least one readable field */
        if (!dbal_ltt_is_readable_result_field(unit, table))
        {
            continue;
        }
        sal_strcat(params_ltt, dbal_logical_table_to_string(unit, table_id));

        if (dnx_dbal_journal_should_create_test(unit, table_id))
        {
            /*
             * gold tables to pre-commit, rest in post-commit
             */
            flags = (dbal_ltt_full_unitest_table_list(unit, table_id)) ? CTEST_PRECOMMIT : CTEST_POSTCOMMIT;

            SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list,
                                                 dbal_logical_table_to_string(unit, table_id), params_ltt,
                                                 flags), "Add test failed");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dbal_dyn_snapshot_manager_test_creation(
    int unit,
    rhlist_t * test_list)
{
    CONST dbal_logical_table_t *table;
    uint32 table_id;

    SHR_FUNC_INIT_VARS(unit);

    for (table_id = 0; table_id < DBAL_NOF_TABLES; table_id++)
    {
        char params_ltt[DBAL_MAX_STRING_LENGTH + 6] = "table=";

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
        if (!CTEST_DNX_DBAL_IS_MATURITY_LEVEL_HIGH(table->maturity_level))
        {
            continue;
        }
        /** check if the table has at least one readable field */
        if (!dbal_ltt_is_readable_result_field(unit, table))
        {
            continue;
        }
        sal_strcat(params_ltt, dbal_logical_table_to_string(unit, table_id));

        if (dnx_dbal_journal_should_create_test(unit, table_id) && dbal_ltt_full_unitest_table_list(unit, table_id))
        {
            /*
             * snapshot manager tests are in post-commit only
             */
            SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list,
                                                 dbal_logical_table_to_string(unit, table_id), params_ltt,
                                                 CTEST_POSTCOMMIT), "Add test failed");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_cmd_t dnx_dbal_test_cmds[] = {
    {"Ltt",              cmd_dnx_dbal_ctest_ltt,      NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_ltt_test_creation},
    {"Ltt_Full",         cmd_dnx_dbal_ctest_ltt_full, NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_ltt_full_test_creation},
    {"skip_shadow_Ltt",  cmd_dnx_dbal_ctest_ltt_skip_shadow,      NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_ltt_skip_shadow_test_creation},
    {"Ut",               cmd_dnx_dbal_ut,             NULL, dnx_dbal_ut_options,   &dnx_dbal_ut_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_ut_test_creation},
    {"ERRor_RECovery",   cmd_dnx_dbal_ctest_err,      NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_error_recovery_test_creation},
    {"SNAPSHOT_MaNaGeR", cmd_dnx_dbal_ctest_snapshot, NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_snapshot_manager_test_creation},
    /** following tests Must be last in this list */
    {"Capacity",         cmd_dnx_dbal_capacity,       NULL, dnx_dbal_capacity_options, &dnx_dbal_capacity_man, NULL, NULL, CTEST_POSTCOMMIT, NULL},
    {"Performance",      cmd_dnx_dbal_performance,    NULL, dnx_dbal_performance_options, &dnx_dbal_performance_man, NULL, NULL, CTEST_POSTCOMMIT, NULL},
    {"Ltt_mdb_cfg",      cmd_dnx_dbal_ctest_ltt,      NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_ltt_mdb_cfg_test_creation},
    {"Ltt_Full_mdb_cfg", cmd_dnx_dbal_ctest_ltt_full, NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_ltt_full_mdb_cfg_test_creation},
    {NULL}
};
/* *INDENT-ON* */

/** \file diag_dnx_dbal.c
 *
 * Main diagnostics for dbal applications All CLI commands, that are related to DBAL, are gathered in this file.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDBALDNX

/*************
 * INCLUDES  *
 *************/
#include <sal/appl/sal.h>

#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>

#include <soc/mcm/memregs.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/swstate/auto_generated/access/algo_qos_access.h>
#include <soc/dnx/swstate/auto_generated/access/lif_mngr_access.h>
#include <soc/dnx/swstate/auto_generated/access/ecmp_access.h>

#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_dbal_api.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/field/field_entry.h>

#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/ctest/dnxc/ctest_dnxc_system.h>

#include "ctest_dnx_dbal.h"

/*************
 * TYPEDEFS  *
 *************/

/**
 * Global indication for initializing Rand test list
 * It affect the flag of the created list
 */
static int is_rand_init = 0;
/**
 * Global indication for initializing MDB CFG test list
 * It affects the list of tables which goes to the list
 */
static int is_mdb_cfg_init = 0;

/**
 * Global indication for initializing ML1 test list
 * It affects the list of tables which goes to the list
 */
static int is_ml1_init = 0;

extern dbal_tables_e mdb_init_tables_list[];


/* *INDENT-OFF* */
	/** this struct consist all dbal tests and related info for the tests. name should be unique and 1:1 mapped to enum
	 *  value */

static sh_sand_enum_t dbal_ut_enum_table_dynamic[DBAL_UT_NOF_TESTS] = { {0} };

dbal_ut_test_info_t dnx_dbal_ut_all_tests[] = {
	{"update_before_commit",   		UPDATE_BEFORE_COMMIT,    	CTEST_PRECOMMIT, dnx_dbal_ut_update_field_before_commit,   	{DBAL_TABLE_INGRESS_PP_PORT, DBAL_TABLE_PEMLA_PARSERETH, DBAL_UT_FILL_TABLE_FOR_2}},
	{"partial_access", 		   		PARTIAL_ACCESS,          	CTEST_PRECOMMIT, dnx_dbal_ut_partial_access,               	{DBAL_TABLE_PEMLA_PARSERETH, DBAL_TABLE_EXAMPLE_SW_SUB_FIELD, DBAL_UT_FILL_TABLE_FOR_2}},
    {"sub_field", 			   		SUB_FIELD,               	CTEST_PRECOMMIT, dnx_dbal_ut_parent_field_mapping,         	{DBAL_TABLE_EXAMPLE_SW_SUB_FIELD, DBAL_UT_FILL_TABLE_FOR_1}},
	{"multiple_instances",	   		MULTIPLE_INSTANCES,      	CTEST_PRECOMMIT, dnx_dbal_ut_multiple_instance_field, 		{DBAL_TABLE_EXAMPLE_TABLE_FOR_MUL_INSTANCES, DBAL_UT_FILL_TABLE_FOR_1}},
	{"rt_access",	 		   		RT_ACCESS,               	CTEST_PRECOMMIT, dnx_dbal_ut_rt_access, 				   	{DBAL_TABLE_EXAMPLE_SW_RESULT_TYPE, DBAL_UT_FILL_TABLE_FOR_1}},
	{"wrong_field_size",	   		WRONG_FIELD_SIZE,        	CTEST_PRECOMMIT, dnx_dbal_ut_wrong_field_size,			   	{DBAL_TABLE_EGRESS_PP_PORT, DBAL_UT_FILL_TABLE_FOR_1}},
	{"wrong_field_access",	   		WRONG_FIELD_ACCESS,      	CTEST_PRECOMMIT, dnx_dbal_ut_wrong_field_access,		   	{DBAL_TABLE_EGRESS_PP_PORT, DBAL_UT_FILL_TABLE_FOR_1}},
	{"superset_res_type",	   		SUPERSET_RES_TYPE,       	CTEST_PRECOMMIT, dnx_dbal_ut_superset_res_type,				{DBAL_TABLE_EXAMPLE_SW_RESULT_TYPE, DBAL_TABLE_IN_LIF_IPvX_TUNNELS, DBAL_UT_FILL_TABLE_FOR_2}},
	{"enum_mapping",	       		ENUM_MAPPING,            	CTEST_PRECOMMIT, dnx_dbal_ut_enum_mapping,			      	{DBAL_TABLE_EXAMPLE_SW_DIRECT, DBAL_UT_FILL_TABLE_FOR_1}},
	{"range_of_entries_set",   		RANGE_OF_ENTRIES_SET,    	CTEST_PRECOMMIT, dnx_dbal_ut_range_of_entries_set,        	{DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_MAPPING, DBAL_TABLE_EXAMPLE_SW_RANGE, DBAL_UT_FILL_TABLE_FOR_2}},
	{"range_entries_clear", 		RANGE_OF_ENTRIES_CLEAR,  	CTEST_PRECOMMIT, dnx_dbal_ut_range_of_entries_clear,      	{DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_MAPPING, DBAL_TABLE_EXAMPLE_SW_RANGE, DBAL_UT_FILL_TABLE_FOR_2}},
	{"range_entries_double",		RANGE_OF_ENTRIES_DOUBLE, 	CTEST_PRECOMMIT, dnx_dbal_ut_range_of_entries_double,      	{DBAL_TABLE_EXAMPLE_SW_RANGE, DBAL_UT_FILL_TABLE_FOR_1}},
	{"all_instances",          		ALL_INSTANCES,				CTEST_PRECOMMIT, dnx_dbal_ut_all_instances_set,      	    {DBAL_TABLE_EXAMPLE_TABLE_FOR_MUL_INSTANCES, DBAL_UT_FILL_TABLE_FOR_1}},
	{"update_handle_with_buffers",	UPDATE_HANDLE_WITH_BUFFERS,	CTEST_PRECOMMIT, dnx_dbal_ut_update_handle_with_buffers,    {DBAL_TABLE_FWD_MACT, DBAL_UT_FILL_TABLE_FOR_1}},
	{"const_value",					CONST_VALUE,				CTEST_PRECOMMIT, dnx_dbal_ut_const_values,    				{DBAL_TABLE_EXAMPLE_TABLE_CONST_VALUES, DBAL_UT_FILL_TABLE_FOR_1}},
	{"dpc_core_any",				DPC_CORE_ANY,				CTEST_PRECOMMIT, dnx_dbal_ut_core_any_dpc,    				{DBAL_TABLE_INGRESS_PTC_PORT, DBAL_TABLE_EXAMPLE_SW_DIRECT, DBAL_UT_FILL_TABLE_FOR_2}},
	{"uint64_fields",				UINT64_FIELDS,				CTEST_PRECOMMIT, dnx_dbal_ut_uint64_fields,    				{DBAL_TABLE_EXAMPLE_FOR_UINT64_USAGE_TEST, DBAL_UT_FILL_TABLE_FOR_1}},
	{"both_cores",					BOTH_CORES,					CTEST_PRECOMMIT, dnx_dbal_ut_both_cores,    				{DBAL_TABLE_INGRESS_ETHERNET_OAM_OPCODE_MAP, DBAL_TABLE_IN_LIF_PROFILE_TABLE, DBAL_UT_FILL_TABLE_FOR_2}},
	{"mac_encoding",				MAC_ENCODING,				CTEST_PRECOMMIT, dnx_dbal_ut_mac_encoding,    				{DBAL_TABLE_EXAMPLE_SW_EM, DBAL_UT_FILL_TABLE_FOR_1}},
	{"predefined_field_val",		PREDEFINED_FIELD_VAL,		CTEST_PRECOMMIT, dnx_dbal_ut_predefine_value,				{DBAL_TABLE_EXAMPLE_FOR_HL_WITH_MEMORY_ALIASING, DBAL_TABLE_INGRESS_ETHERNET_OAM_OPCODE_MAP, DBAL_TABLE_IN_LIF_PROFILE_TABLE, DBAL_UT_FILL_TABLE_FOR_3}},
    {"struct_field_encode_decode",	STRUCT_FIELD_ENCODE_DECODE,	CTEST_PRECOMMIT, dnx_dbal_ut_struct_field_encode_decode,	{DBAL_UT_NO_TABLE}},
    {"dynamic_table_validation",	DYNAMIC_TABLE_VALIDATION,	CTEST_PRECOMMIT, dnx_dbal_ut_dynamic_table_validations,	    {DBAL_UT_NO_TABLE}},
    {"dynamic_res_type",	        DYNAMIC_RES_TYPE,	        CTEST_PRECOMMIT, dnx_dbal_ut_dynamic_res_type,	            {DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION, DBAL_UT_FILL_TABLE_FOR_1}},
    {"iterator_with_key_rules",     ITERATOR_WITH_KEY_RULES,    CTEST_PRECOMMIT, dnx_dbal_ut_iterator_with_key_rules,       {DBAL_TABLE_EXAMPLE_FOR_HL_MULTIPLE_RESULT, DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD, DBAL_TABLE_EXAMPLE_SW_EM_LONG_KEY, DBAL_UT_FILL_TABLE_FOR_3}},
    {"iterator_with_value_rules",   ITERATOR_WITH_VALUE_RULES,  CTEST_PRECOMMIT, dnx_dbal_ut_iterator_with_value_rules,     {DBAL_TABLE_EXAMPLE_FOR_HL_MULTIPLE_RESULT, DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST, DBAL_TABLE_EXAMPLE_SW_LONG_RES, DBAL_UT_FILL_TABLE_FOR_3}},
    {"iterator_with_rules_actions", ITERATOR_WITH_RULES_ACTIONS,CTEST_PRECOMMIT, dnx_dbal_ut_iterator_with_rules_actions,   {DBAL_TABLE_EXAMPLE_FOR_HL_MULTIPLE_RESULT, DBAL_UT_FILL_TABLE_FOR_1}},
    {"iterator_multiple_rt",		ITERATOR_MULTIPLE_RT,		CTEST_PRECOMMIT, dnx_dbal_ut_iterator_mul_result,           {DBAL_TABLE_EXAMPLE_FOR_HL_MULTIPLE_RESULT, DBAL_TABLE_MCDB, DBAL_TABLE_L2_GPORT_TO_FORWARDING_SW_INFO, DBAL_TABLE_EXAMPLE_SW_MUL_RESULT, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, DBAL_TABLE_ING_VSI_INFO_DB}},
    {"iterator_hit_bit_rule",       ITERATOR_HIT_BIT_RULE,      CTEST_PRECOMMIT, dnx_dbal_ut_iterator_hit_bit_rule,         {DBAL_TABLE_ING_VSI_INFO_DB, DBAL_UT_FILL_TABLE_FOR_1}},
    {"tcam_cs",	                    TCAM_CS,		            CTEST_PRECOMMIT, dnx_dbal_ut_tcam_cs_validations,           {DBAL_TABLE_TCAM_CS_IPMF3, DBAL_TABLE_TCAM_CS_EPMF, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM, DBAL_UT_FILL_TABLE_FOR_3, }},
    {"commit_forced",	            COMMIT_FORCED,		        CTEST_PRECOMMIT, dnx_dbal_ut_commit_forced,                 {DBAL_TABLE_LOCAL_DPC_INLIF_INFO_SW, DBAL_TABLE_INGRESS_OAM_ACC_MEP_DB, DBAL_TABLE_IPV6_MULTICAST_PRIVATE_LPM_FORWARD, DBAL_UT_FILL_TABLE_FOR_3}},
    {"tcam_basic",					TCAM_BASIC,				    CTEST_PRECOMMIT, dnx_dbal_ut_tcam_table_test,    			{DBAL_TABLE_IPV4_MULTICAST_TCAM_FORWARD, DBAL_UT_FILL_TABLE_FOR_1}},
    {"tcam_mdb",					TCAM_MDB,				    CTEST_PRECOMMIT, dnx_dbal_ut_mdb_tcam_validations,    		{DBAL_TABLE_IPV4_TT_TCAM_BASIC, DBAL_UT_FILL_TABLE_FOR_1}},
    {"err_code_non_direct",	        ERR_CODE_NON_DIRECT,		CTEST_PRECOMMIT, dnx_dbal_ut_non_direct_error_codes,        {DBAL_TABLE_BIER_BFER_TERMINATION, DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SOURCE_SPECIFIC_IVL, DBAL_TABLE_BFD_ENDPOINT_SW_INFO, DBAL_UT_FILL_TABLE_FOR_3}},
    {"valid_ind",					VALID_IND,				    CTEST_PRECOMMIT, dnx_dbal_ut_field_valid_indication,    	{DBAL_UT_NO_TABLE}},
    {"table_restore",				TABLE_RESTORE,				CTEST_PRECOMMIT, dnx_dbal_ut_table_restore,    	            {DBAL_UT_NO_TABLE}},
    {"hl_tcam",	                    HL_TCAM,		            CTEST_PRECOMMIT, dnx_dbal_ut_hl_tcam,                       {DBAL_TABLE_EXAMPLE_HL_TCAM, DBAL_TABLE_EXAMPLE_HL_TCAM_DATA_OFFSET, DBAL_UT_FILL_TABLE_FOR_2}},
    {"tcam_kbp",					TCAM_KBP,				    CTEST_PRECOMMIT, dnx_dbal_ut_kbp_tcam,    			        {DBAL_TABLE_EXAMPLE_KBP_ACL, DBAL_UT_FILL_TABLE_FOR_1}},
    {"lpm_mask",					LPM_MASK,				    CTEST_PRECOMMIT, dnx_dbal_ut_lpm_mask,    			        {DBAL_TABLE_IPV4_MULTICAST_PRIVATE_LPM_FORWARD, DBAL_UT_FILL_TABLE_FOR_1}},
    {"merge_entries",               MERGE_ENTRIES,		        CTEST_PRECOMMIT, dnx_dbal_ut_merge_entries,                 {DBAL_TABLE_EGR_VSI_INFO_DB, DBAL_TABLE_EGRESS_OAM_LIF_DB/** MDB EM */, DBAL_TABLE_EXAMPLE_TABLE_FOR_MUL_INSTANCES, DBAL_TABLE_L3_VRRP_PROTOCOL_GROUP_SW /** SW direct*/, DBAL_TABLE_BFD_ENDPOINT_SW_INFO/** SW EM */, DBAL_UT_FILL_TABLE_FOR_5 }},
    {"entries_counter",          	ENTRIES_COUNTER,	        CTEST_PRECOMMIT, dnx_dbal_ut_entries_counter,               {DBAL_TABLE_EXAMPLE_SW_EM, DBAL_TABLE_EXAMPLE_SW_DIRECT, DBAL_TABLE_EXAMPLE_HL_TCAM, DBAL_TABLE_TCAM_CS_EPMF, DBAL_UT_FILL_TABLE_FOR_4}},
    {"field_allocator",             FIELD_ALLOCATOR,		    CTEST_PRECOMMIT, dnx_dbal_ut_field_allocator,               {DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_INFO, DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_TO_INTERNAL_MAP_ID, DBAL_TABLE_EXAMPLE_SW_ALLOC, DBAL_UT_FILL_TABLE_FOR_3}},
    {"read_only_field",			    READONLY_FIELD,				CTEST_PRECOMMIT, dnx_dbal_ut_readonly_field,                {DBAL_TABLE_NIF_DEBUG_SIGNALS, DBAL_UT_FILL_TABLE_FOR_1}},
    {"write_only_field",			WRITEONLY_FIELD,			CTEST_PRECOMMIT, dnx_dbal_ut_read_not_readable_field,       {DBAL_TABLE_L2_AGE_SCAN, DBAL_UT_FILL_TABLE_FOR_1}},
    {"trigger_field",			    TRIGGER_FIELD,				CTEST_PRECOMMIT, dnx_dbal_ut_read_not_readable_field,       {DBAL_TABLE_HCC_BIST_RUN, DBAL_UT_FILL_TABLE_FOR_1}},
    {"arr_prefix",                  ARR_PREFIX,	                CTEST_PRECOMMIT, dnx_dbal_ut_arr_prefix,                    {DBAL_TABLE_EXAMPLE_TABLE_REGISTER_GROUP/**enum*/, DBAL_TABLE_EXAMPLE_TABLE_CONST_VALUES/**regular field*/, DBAL_UT_FILL_TABLE_FOR_2}},
    {"mact_iterator",			    MACT_ITERATOR,				CTEST_PRECOMMIT, dnx_dbal_ut_mact_iterator,                 {DBAL_TABLE_FWD_MACT, DBAL_UT_FILL_TABLE_FOR_1}},
    {"default_value", 		        DEFAULT_VALUE_TEST,         CTEST_PRECOMMIT, dnx_dbal_ut_default_values,                { DBAL_TABLE_NIF_ILE_CONNECTIVITY, DBAL_TABLE_ECGM_CORE_TC_MULTICAST_SP_THRESHOLDS, DBAL_UT_FILL_TABLE_FOR_3}},
    {"disabled_result_type",        DISABLED_RESULT_TYPE,       CTEST_PRECOMMIT, dnx_dbal_ut_disabled_result_type,          {DBAL_TABLE_EXAMPLE_TABLE_FOR_HL_WITH_MUL_SW_FIELD, DBAL_UT_FILL_TABLE_FOR_1}},
    {"update_result_type",          UPDATE_RESULT_TYPE,         CTEST_PRECOMMIT, dnx_dbal_ut_update_result_type,            {DBAL_TABLE_ING_VSI_INFO_DB, DBAL_UT_FILL_TABLE_FOR_1}},
};
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
static shr_error_e
dbal_print_table_names(
    int unit,
    char *table_name)
{
    CONST dbal_logical_table_t *table;
    int is_full_match = 0;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    dbal_string_to_next_table_id_get(unit, table_name, -1, &table_id, &is_full_match);

    while (table_id != dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
        LOG_CLI((BSL_META("\t%s \n"), table->table_name));

        dbal_string_to_next_table_id_get(unit, table_name, table_id, &table_id, &is_full_match);
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
        DBAL_TABLE_EXAMPLE_MDB_WITH_RESULT_TYPE_IN_SW_HASH,
        DBAL_TABLE_OAMP_MEP_STATIC_DATA_DB,
        DBAL_TABLE_NIF_CDU_RX_RMC_CTRL,
        DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_DB,
        DBAL_TABLE_IN_LIF_FORMAT_LSP,
        DBAL_TABLE_LOCAL_SBC_INLIF_INFO_SW,
        DBAL_TABLE_LOCAL_DPC_INLIF_INFO_SW,
        DBAL_TABLE_CRPS_METER_BIG_ENGINE_SHARED_MEM,
        DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_NIF_PRD_CDU_IP_PRIORITY_MAP,
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
    if (flags & LTT_FLAGS_MEASUREMERNTS)
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
    if (flags & LTT_FLAGS_MEASUREMERNTS)
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

    if (flags & LTT_FLAGS_MEASUREMERNTS)
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

    if (flags & LTT_FLAGS_MEASUREMERNTS)
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
* Run logical test for specific/filtered table(s) according to
* input_strings[0] - table name or "FILTERS"
* input_strings[1] - access filter.
* input_strings[2] - type filter.
* input_strings[3] - test flags.
*/
shr_error_e
dbal_logical_tables_test(
    int unit,
    char *input_strings[CMD_MAX_NOF_INPUTS],
    sh_sand_control_t * sand_control)
{
    int ii, iter_start_pos = 0, iter_end_pos = DBAL_NOF_TABLES;
    shr_error_e curr_res = _SHR_E_NONE, err_rec_test_ltt_res = _SHR_E_NONE, result = _SHR_E_NONE;
    uint8 filtered_tables = FALSE;
    uint32 flags = 0;
    dbal_tables_e table_id = DBAL_NOF_TABLES;
    dbal_access_method_e access_method = DBAL_NOF_ACCESS_METHODS;
    dbal_table_type_e table_type = DBAL_NOF_TABLE_TYPES;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    /** 2nd token is the test flags */
    flags = sal_ctoi(input_strings[3], 0);

    if (flags >= LTT_FLAGS_NOF_FLAGS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal LTT flags, flags=0x%x\n", flags);
    }

    if ((sal_strcasecmp(input_strings[0], "FILTERS") == 0)
        || ((flags & LTT_FLAGS_MULTIPLE_RESULTS) != 0) || ((flags & LTT_FLAGS_SW_FIELDS) != 0))
    {
        filtered_tables = TRUE;

        /** check which filters we have set */
        if ((NULL != input_strings[1]) && (sal_strcasecmp(input_strings[1], " ") != 0)
            && (dbal_access_method_string_to_id(unit, input_strings[1], &access_method) == _SHR_E_NOT_FOUND))
        {
            access_method = DBAL_NOF_ACCESS_METHODS;
        }
        if ((NULL != input_strings[2]) && (sal_strcasecmp(input_strings[2], " ") != 0)
            && (dbal_logical_table_type_string_to_id(unit, input_strings[2], &table_type) == _SHR_E_NOT_FOUND))
        {
            table_type = DBAL_NOF_TABLE_TYPES;
        }
        LOG_CLI((BSL_META("Running test with filters applied to all existing logical tables...\n\n")));
    }
    else
    {
        if (dbal_logical_table_string_to_id(unit, input_strings[0], &table_id) == _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(dbal_print_table_names(unit, input_strings[0]));
            SHR_EXIT();
        }
        iter_start_pos = table_id;
        iter_end_pos = table_id + 1;
    }

    /** Set the Result PRT structure */
    PRT_TITLE_SET("Logical Table Test (Flags=0x%x) Summary", flags);
    PRT_COLUMN_ADD("Table Name");
    PRT_COLUMN_ADD("Access Method");
    PRT_COLUMN_ADD("Result");

    SHR_IF_ERR_EXIT(dbal_ltt_measurements_init(unit, prt_ctr, flags));

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
            /** only high maturity level tables */
            if (table->maturity_level != DBAL_MATURITY_HIGH)
            {
                continue;
            }

            /** check if we should filter by access method */
            if ((DBAL_NOF_ACCESS_METHODS != access_method) && (table->access_method != access_method))
            {
                continue;
            }

            /** check if we should filter by type */
            if ((DBAL_NOF_TABLE_TYPES != table_type) && (table->table_type != table_type))
            {
                continue;
            }

            /** check if we should filter out tables that have multiple results */
            if (((flags & LTT_FLAGS_MULTIPLE_RESULTS) != 0) && (table->nof_result_types <= 1))
            {
                continue;
            }

            /** check if we should filter out tables that contain sw fields */
            if (((flags & LTT_FLAGS_SW_FIELDS) != 0) && (table->sw_payload_length_bytes == 0))
            {
                continue;
            }
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", table->table_name);
        PRT_CELL_SET("%s", dbal_access_method_to_string(unit, table->access_method));

        SHR_IF_ERR_EXIT(dbal_ltt_measurements_pre_test(unit, flags));

        /*
         * for dbal error recovery tests first the regular ltt test is being ran first
         * if the dbal test fails the error recovery test is considered to pass
         */
        if ((flags & LTT_FLAGS_ERR_RECOVERY_CLEAN_UP) || (flags & LTT_FLAGS_SNAPSHOT_NEGATIVE))
        {
            err_rec_test_ltt_res = diag_dbal_test_logical_table(unit, ii, 0x0);
            curr_res =
                (_SHR_E_NONE == err_rec_test_ltt_res) ? diag_dbal_test_logical_table(unit, ii, flags) : _SHR_E_NONE;
        }
        else
        {
            curr_res = diag_dbal_test_logical_table(unit, ii, flags);
        }

        if (curr_res == _SHR_E_UNAVAIL)
        {
            PRT_CELL_SET("%s", "NOT_RUN");
        }
        else if (curr_res == _SHR_E_NONE)
        {
            PRT_CELL_SET("%s", "PASS");
            result |= dbal_ltt_measurements_after_test(unit, prt_ctr, ii, flags);
        }
        else
        {
            PRT_CELL_SET("%s", "FAIL");
            result |= _SHR_E_FAIL;
        }
    }

    LOG_CLI((BSL_META("\n")));
    SHR_IF_ERR_EXIT(dbal_ltt_measurements_finish(unit, flags));
    PRT_COMMITX;

    SHR_SET_CURRENT_ERR(result);

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_diag_ltt_prepare_inputs(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    char *test_name,
    char **input_strings)
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

    if (ISEMPTY(table_name))
    {
        if (ISEMPTY(access) && ISEMPTY(type))
        {
            LOG_CLI((BSL_META("Example: ctest dbal %s Table=<table_name>"), test_name));
            LOG_CLI((BSL_META("Example: ctest dbal %s Access=<table_access> Type=<table_type>"), test_name));
            SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            SHR_EXIT();
        }

        /*
         * instead of a table name we have filters,
         * check which filters we have specified
         */
        input_strings[0] = "FILTERS";

        if (!ISEMPTY(access))
        {
            input_strings[1] = access;
        }

        if (!ISEMPTY(type))
        {
            input_strings[2] = type;
        }
    }
    else
    {
        /** table name was specified */
        input_strings[0] = table_name;
    }
    if (!ISEMPTY(flag))
    {
        input_strings[3] = flag;
    }

exit:
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
    /** default flags: cleanup after test and utilize error recovery for cleanup */
    char *input_strings[5] = { " ", " ", " ", "0x0", " " };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_diag_ltt_prepare_inputs(unit, args, sand_control, "ltt", input_strings));
    SHR_IF_ERR_EXIT(dbal_logical_tables_test(unit, input_strings, sand_control));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnx_dbal_ctest_ltt_ml1(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    /** default flags: cleanup after test and utilize error recovery for cleanup */
    char *input_strings[5] = { " ", " ", " ", "0x0", " " };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_diag_ltt_prepare_inputs(unit, args, sand_control, "ML1", input_strings));
    SHR_IF_ERR_EXIT(dbal_logical_tables_test(unit, input_strings, sand_control));

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
    /** default flags: cleanup after test and utilize error recovery for cleanup */
    char *input_strings[5] = { " ", " ", " ", "0x50", " " };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_diag_ltt_prepare_inputs(unit, args, sand_control, "ltt_full", input_strings));
    SHR_IF_ERR_EXIT(dbal_logical_tables_test(unit, input_strings, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief run logical table test in ctest with flags = 0
 */
static shr_error_e
cmd_dnx_dbal_ctest_rand_ltt(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    /** default flags: cleanup after test and utilize error recovery for cleanup */
    char *input_strings[5] = { " ", " ", " ", "0x8", " " };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_diag_ltt_prepare_inputs(unit, args, sand_control, "ltt", input_strings));
    SHR_IF_ERR_EXIT(dbal_logical_tables_test(unit, input_strings, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief run logical table test in ctest with flags = 80
 * full test + timers
 */
static shr_error_e
cmd_dnx_dbal_ctest_rand_ltt_full(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    /** default flags: cleanup after test and utilize error recovery for cleanup */
    char *input_strings[5] = { " ", " ", " ", "0x58", " " };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_diag_ltt_prepare_inputs(unit, args, sand_control, "ltt_full", input_strings));
    SHR_IF_ERR_EXIT(dbal_logical_tables_test(unit, input_strings, sand_control));

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
    /** default flags: cleanup after test and utilize error recovery for cleanup */
    char *input_strings[5] = { " ", " ", " ", "0x82", " " };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_diag_ltt_prepare_inputs(unit, args, sand_control, "error_recovery", input_strings));
    SHR_IF_ERR_EXIT(dbal_logical_tables_test(unit, input_strings, sand_control));

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
    /** default flags: cleanup after test and utilize error recovery for cleanup */
    char *input_strings[5] = { " ", " ", " ", "0x400", " " };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_diag_ltt_prepare_inputs(unit, args, sand_control, "snapshot_manager", input_strings));
    SHR_IF_ERR_EXIT(dbal_logical_tables_test(unit, input_strings, sand_control));

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
            if (dbal_logical_table_string_to_id(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
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

static shr_error_e
cmd_dnx_dbal_rand_ut(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_tables_e table_id;
    char *table_name;
    dbal_ut_tests_e test_name;
    char *dbal_ut_empty_table = "DBAL_TABLE_EMPTY";

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get parameters
     */
    SH_SAND_GET_STR("table", table_name);
    SH_SAND_GET_ENUM("Name", test_name);

    if (ISEMPTY(table_name))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Table name is mandatory field\n");
    }

    if (sal_strcmp(table_name, dbal_ut_empty_table) != 0)
    {
        if (dbal_logical_table_string_to_id(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(dbal_print_table_names(unit, table_name));
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }

    switch (test_name)
    {
        case UPDATE_BEFORE_COMMIT:
            SHR_IF_ERR_EXIT(dnx_dbal_update_field_before_commit(unit, table_id, TRUE));
            break;
        case PARTIAL_ACCESS:
            SHR_IF_ERR_EXIT(test_dnx_dbal_partial_access(unit, table_id, DBAL_UT_RESTORE_TABLE, TRUE));
            break;
        case WRONG_FIELD_SIZE:
            SHR_IF_ERR_EXIT(dnx_dbal_wrong_field_size(unit, table_id, TRUE));
            break;
        case WRONG_FIELD_ACCESS:
            SHR_IF_ERR_EXIT(dnx_dbal_wrong_field_access(unit, table_id, TRUE));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Test case %d is not supported in random unitest\n", test_name);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_dbal_test_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "DBAL logical table name", ""},
    {"flags", SAL_FIELD_TYPE_STR, "DBAL logical table name flags", ""},
    {"ACCess", SAL_FIELD_TYPE_STR, "DBAL logical table access", ""},
    {"TYpe", SAL_FIELD_TYPE_STR, "DBAL logical table type", ""},
    {NULL}
};

/**
 * \brief consist a mapping between the name of the test and thecorresponding enum value. it is initialized dynamicly 
 *  	  according to the ut struct.
 */

static sh_sand_option_t dnx_dbal_ut_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "DBAL logical table name", ""},
    {"Name", SAL_FIELD_TYPE_ENUM, "DBAL unit test name", "update_before_commit", (void *) dbal_ut_enum_table_dynamic},
    {NULL}
};

static sh_sand_man_t dnx_dbal_ut_test_man = {
    "DBAL logical table test - unit tests for DBAL module",
    "ctest dbal ut name=<test_name> table=<table_name>",
    "ctest dbal ut name=tcam_kbp table=example_kbp_acl"
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
        "\tFlags=32 - Run test with maximum of three entries\n"
        "\tFlags=64 - Perform measurements (performence test)\n"
        "\tFlags=128 - Use error recovery for cleaup after test\n"
        "\tFlags=256 - Tables containing multiple results only\n"
        "\tFlags=512 - Tables containing sw fields only\n"
        "\tFlags=1024 - Run snapshot manager negative testing before test clear\n"
        "dbal TaBLe TaBLe=<tabel name> FLaGs=<flags_value>",
    "tbl=EXAMPLE_TABLE_ILLEGAL_VALUES flags=0x10"
};

sh_sand_invoke_t sh_dnx_dbal_rand_ut[] = {
    {"update_before_commit_EGRESS_PP_PORT", "Name=UPDATE_BEFORE_COMMIT table=EGRESS_PP_PORT", CTEST_RANDOM},
    {"update_before_commit_PEMLA_PARSERETH", "Name=UPDATE_BEFORE_COMMIT table=PEMLA_PARSERETH", CTEST_RANDOM},
    {"partial_access_PEMLA_PARSERETH", "Name=PARTIAL_ACCESS table=PEMLA_PARSERETH", CTEST_RANDOM},
    {"partial_access_EXAMPLE_SW_SUB_FIELD", "Name=PARTIAL_ACCESS table=EXAMPLE_SW_SUB_FIELD", CTEST_RANDOM},
    {"wrong_field_size_EGRESS_PP_PORT", "Name=WRONG_FIELD_SIZE table=EGRESS_PP_PORT", CTEST_RANDOM},
    {"wrong_field_access_EGRESS_PP_PORT", "Name=WRONG_FIELD_ACCESS table=EGRESS_PP_PORT", CTEST_RANDOM},
    {NULL}
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
            char ut_name[DBAL_MAX_STRING_LENGTH * 2 + 10] = "";
                                                              /** consist the test name only */
            char ut_args[DBAL_MAX_STRING_LENGTH + 5] = "name="; /** consist the full args to run the test name=xxx table=yyy*/
            char ut_table[DBAL_MAX_STRING_LENGTH + 7] = " table="; /** consist the table to run the test table=yyy */
            int ut_flags;

            sal_strncat(ut_name, dnx_dbal_ut_all_tests[test_idx].test_name, sizeof(ut_name));
            sal_strncat(ut_args, dnx_dbal_ut_all_tests[test_idx].test_name, sizeof(ut_args));

            if (dnx_dbal_ut_all_tests[test_idx].table_ids[table_idx] != DBAL_TABLE_EMPTY)
            {
                sal_strncat(ut_table,
                            dbal_logical_table_to_string(unit, dnx_dbal_ut_all_tests[test_idx].table_ids[table_idx]),
                            sizeof(ut_table));
            }
            else
            {
                if (table_idx != 0)
                {
                    break;
                }
                sal_strncat(ut_table, dbal_ut_empty_table, sizeof(ut_table));
            }

            /** incase we have more then 1 table for a specific test, adding prefix to the name of the test ( test names should be
             *  uniqe) */
            if (table_idx != 0)
            {
                sal_snprintf(ut_name, DBAL_MAX_STRING_LENGTH * 2, "%s_%d ", ut_name, table_idx);
            }

            ut_flags = dnx_dbal_ut_all_tests[test_idx].ctest_flags;
            sal_strncat(ut_args, ut_table, sizeof(ut_args));

                        /** this part removing all example tables after senitization */
            if (!is_internal_tables_valid)
            {
                if (strcaseindex(ut_table, "EXAMPLE_"))
                {
                    continue;
                }
            }


            /** skip tables by maturity level */
            if (dnx_dbal_ut_all_tests[test_idx].table_ids[table_idx] != DBAL_TABLE_EMPTY) {
                CONST dbal_logical_table_t *table;
                SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dnx_dbal_ut_all_tests[test_idx].table_ids[table_idx], &table));
                if (!CTEST_DNX_DBAL_IS_MATURITY_LEVEL_HIGH(table->maturity_level)) {
                    continue; 
                }
            }

            SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, ut_name, ut_args, ut_flags), "Add test failed");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** in init of dbal */
static shr_error_e
cmd_dbal_dyn_ltt_test_creation(
    int unit,
    rhlist_t * test_list)
{
    CONST dbal_logical_table_t *table;
    uint32 table_id;

    SHR_FUNC_INIT_VARS(unit);

    for (table_id = 0; table_id < DBAL_NOF_TABLES; table_id++)
    {
        char params_ltt[DBAL_MAX_STRING_LENGTH + 6] = "table=";
        int flags;

        if (dbal_ltt_mdb_cfg_table_list(unit, table_id))
        {
            if (!is_mdb_cfg_init)
            {
                continue;
            }
        }
        else
        {
            if (is_mdb_cfg_init)
            {
                continue;
            }
        }

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
        if (!CTEST_DNX_DBAL_IS_MATURITY_LEVEL_HIGH(table->maturity_level))
        {
            if (is_ml1_init == 1)
            {
                if (table->maturity_level != DBAL_MATURITY_PARTIALLY_FUNCTIONAL)
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }
        else if (is_ml1_init)
        {
            continue;
        }

        /** check if the table has at least one readable field */
        if (!dbal_ltt_is_readable_result_field(unit, table))
        {
            continue;
        }
        sal_strcat(params_ltt, dbal_logical_table_to_string(unit, table_id));

        flags = CTEST_PRECOMMIT;
        if ((table->access_method == DBAL_ACCESS_METHOD_KBP))
        {
            if (!SAL_BOOT_PLISIM)
            {
                flags = 0;
            }
        }

        if (is_rand_init)
        {
            flags |= CTEST_RANDOM;
        }
        SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, dbal_logical_table_to_string(unit, table_id), params_ltt,
                                             flags), "Add test failed");
    }

exit:
    SHR_FUNC_EXIT;
}

/** in init of dbal   */
static shr_error_e
cmd_dbal_dyn_ltt_full_test_creation(
    int unit,
    rhlist_t * test_list)
{
    CONST dbal_logical_table_t *table;
    uint32 table_id;

    SHR_FUNC_INIT_VARS(unit);

    for (table_id = 0; table_id < DBAL_NOF_TABLES; table_id++)
    {
        int flags;
        char params_ltt[DBAL_MAX_STRING_LENGTH + 6] = "table=";

        if (dbal_ltt_mdb_cfg_table_list(unit, table_id))
        {
            if (!is_mdb_cfg_init)
            {
                continue;
            }
        }
        else
        {
            if (is_mdb_cfg_init)
            {
                continue;
            }
        }

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

        if (dbal_ltt_full_unitest_table_list(unit, table_id))
        {
            flags = CTEST_PRECOMMIT;
        }
        else
        {
            flags = 0;
        }

        if (is_rand_init)
        {
            flags |= CTEST_RANDOM;
        }
        SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, dbal_logical_table_to_string(unit, table_id), params_ltt,
                                             flags), "Add test failed");
    }

exit:
    SHR_FUNC_EXIT;
}
/** in init of dbal */
shr_error_e
cmd_dbal_dyn_rand_ltt_test_creation(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    is_rand_init = 1;
    SHR_IF_ERR_EXIT(cmd_dbal_dyn_ltt_test_creation(unit, test_list));
    is_rand_init = 0;

exit:
    SHR_FUNC_EXIT;
}

/** in init of dbal */
shr_error_e
cmd_dbal_dyn_rand_ltt_full_test_creation(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    is_rand_init = 1;
    SHR_IF_ERR_EXIT(cmd_dbal_dyn_ltt_full_test_creation(unit, test_list));
    is_rand_init = 0;

exit:
    SHR_FUNC_EXIT;
}

/** in init of dbal */
shr_error_e
cmd_dbal_dyn_ltt_mdb_cfg_test_creation(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    is_mdb_cfg_init = 1;
    SHR_IF_ERR_EXIT(cmd_dbal_dyn_ltt_test_creation(unit, test_list));
    is_mdb_cfg_init = 0;

exit:
    SHR_FUNC_EXIT;
}

/** in init of dbal */
shr_error_e
cmd_dbal_dyn_ltt_ml1_test_creation(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    is_ml1_init = 1;
    SHR_IF_ERR_EXIT(cmd_dbal_dyn_ltt_test_creation(unit, test_list));
    is_ml1_init = 0;

exit:
    SHR_FUNC_EXIT;
}

/** in init of dbal */
shr_error_e
cmd_dbal_dyn_ltt_full_mdb_cfg_test_creation(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    is_mdb_cfg_init = 1;
    SHR_IF_ERR_EXIT(cmd_dbal_dyn_ltt_full_test_creation(unit, test_list));
    is_mdb_cfg_init = 0;

exit:
    SHR_FUNC_EXIT;
}

/** in init of dbal */
shr_error_e
cmd_dbal_dyn_rand_ltt_mdb_cfg_test_creation(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    is_rand_init = 1;
    is_mdb_cfg_init = 1;
    SHR_IF_ERR_EXIT(cmd_dbal_dyn_ltt_test_creation(unit, test_list));
    is_rand_init = 0;
    is_mdb_cfg_init = 0;

exit:
    SHR_FUNC_EXIT;
}

/** in init of dbal */
shr_error_e
cmd_dbal_dyn_rand_ltt_full_mdb_cfg_test_creation(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    is_rand_init = 1;
    is_mdb_cfg_init = 1;
    SHR_IF_ERR_EXIT(cmd_dbal_dyn_ltt_full_test_creation(unit, test_list));
    is_rand_init = 0;
    is_mdb_cfg_init = 0;

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

    if (DNX_ERR_RECOVERY_IS_DISABLED)
    {
        SHR_EXIT();
    }

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

        if (!dnx_dbal_journal_is_unsupported_table(unit, table_id))
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

    if (DNX_ERR_RECOVERY_IS_DISABLED)
    {
        SHR_EXIT();
    }

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

        if (!dnx_dbal_journal_is_unsupported_table(unit, table_id) && dbal_ltt_full_unitest_table_list(unit, table_id))
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
sh_sand_cmd_t dnx_dbal_random_test_cmds[] = {
    {"Ltt",              cmd_dnx_dbal_ctest_rand_ltt,      NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_rand_ltt_test_creation},
    {"Ltt_Full",         cmd_dnx_dbal_ctest_rand_ltt_full, NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_rand_ltt_full_test_creation},
    {"Ut",               cmd_dnx_dbal_rand_ut,             NULL, dnx_dbal_ut_options,   &dnx_dbal_ut_test_man, NULL, sh_dnx_dbal_rand_ut, CTEST_PASS},
    {"Ltt_mdb_cfg",      cmd_dnx_dbal_ctest_ltt,           NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_rand_ltt_mdb_cfg_test_creation},
    {"Ltt_Full_mdb_cfg", cmd_dnx_dbal_ctest_ltt_full,      NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_rand_ltt_full_mdb_cfg_test_creation},
    {NULL}
};

sh_sand_cmd_t dnx_dbal_test_cmds[] = {
    {"Ltt",              cmd_dnx_dbal_ctest_ltt,      NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_ltt_test_creation},
    {"Ltt_Full",         cmd_dnx_dbal_ctest_ltt_full, NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_ltt_full_test_creation},
    {"Ut",               cmd_dnx_dbal_ut,             NULL, dnx_dbal_ut_options,   &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_ut_test_creation},
    {"ERRor_RECovery",   cmd_dnx_dbal_ctest_err,      NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_error_recovery_test_creation},
    {"SNAPSHOT_MaNaGeR", cmd_dnx_dbal_ctest_snapshot, NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_snapshot_manager_test_creation},
    {"RaNDom",           NULL,   dnx_dbal_random_test_cmds, NULL, &dnx_dbal_test_man},
	/** this the test should run on all tables that has ML1 and validate that they realy fails.. currently this test should
	 *  not run in regrssion. (there cases that tables pass on cmodel an not on real device (maybe set a exlude list for
	 *  those tables)) missing some implementation in this test: return pass if the test fails and opposite and add
	 *  correct info */
	{"ML1",              cmd_dnx_dbal_ctest_ltt_ml1,  NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, SH_CMD_SKIP_EXEC, cmd_dbal_dyn_ltt_ml1_test_creation},

	/** following tests Must be last in this list */
    {"Ltt_mdb_cfg",      cmd_dnx_dbal_ctest_ltt,      NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_ltt_mdb_cfg_test_creation},
    {"Ltt_Full_mdb_cfg", cmd_dnx_dbal_ctest_ltt_full, NULL, dnx_dbal_test_options, &dnx_dbal_test_man, NULL, NULL, CTEST_PASS, cmd_dbal_dyn_ltt_full_mdb_cfg_test_creation},
    {NULL}
};
/* *INDENT-ON* */

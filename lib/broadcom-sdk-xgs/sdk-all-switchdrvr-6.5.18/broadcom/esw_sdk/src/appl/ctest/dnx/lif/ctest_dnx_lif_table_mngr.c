/** \file diag_dnx_lif_table_mngr.c
 *
 * LIF Table manager unit test.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/**
* INCLUDE FILES:
* {
*/
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm/types.h>
#include <soc/dnx/mdb.h>
/*#include <src/soc/dnx/mdb/mdb_internal.h> */
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <appl/diag/sand/diag_sand_framework.h>

extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

/*
 * DEFINES
 */
#define MAX_NUM_LIF_TABLE_FIELDS    (10)
#define RESULT_TYPE_INVALID         (-1)

/*
 * structs to support bcm shell command
 */
sh_sand_man_t dnx_in_lif_table_unit_test_man = {
    "Unit Test for the In-LIF Table manager",
    "Unit Test for the In-LIF Table manager. Provides basic scenarios"
};

sh_sand_man_t dnx_out_lif_table_unit_test_man = {
    "Unit Test for the Out-LIF Table manager",
    "Unit Test for the Out-LIF Table manager. Provides basic scenarios"
};

sh_sand_man_t dnx_in_lif_table_replace_test_man = {
    "Test the result type Replace functionality for the In-LIF Table manager",
    "Test the result type Replace functionality for the In-LIF Table manager. Provides result-type modification scenarios"
};

sh_sand_man_t dnx_out_lif_table_replace_test_man = {
    "Test the result type Replace functionality for the Out-LIF Table manager",
    "Test the result type Replace functionality for the Out-LIF Table manager. Provides result-type modification scenarios"
};

#define MAX_NUM_LIF_TABLE_FIELDS    (10)
#define NO_SPECIAL_FLAGS            (0)

/*
 * Struct with the required information per test case 
 */
typedef struct
{
    dbal_tables_e dbal_table_id;
    uint32 table_specific_flags;
} lif_table_mngr_test_case_info_t;

/*
 * Struct with the required information an entry set 
 */
typedef struct
{
    uint32 num_of_fields;
    uint32 field_id[MAX_NUM_LIF_TABLE_FIELDS];
    uint32 field_val[MAX_NUM_LIF_TABLE_FIELDS];
    uint32 expected_result_type;
} lif_table_mngr_result_type_set_t;

/*
 * Struct with the required information for In-LIF unit test case: 
 * Info per test case and Info per entry set 
 */
typedef struct
{
    lif_table_mngr_test_case_info_t test_case_info;
    lif_table_mngr_result_type_set_t result_type_set;
} lif_table_mngr_inlif_unit_test_cases_t;

/*
 * Struct with the required information for Out-LIF unit test case: 
 * Info per test case, Out-LIF phase and Info per entry set 
 */
typedef struct
{
    lif_table_mngr_test_case_info_t test_case_info;
    lif_mngr_outlif_phase_e outlif_phase;
    lif_table_mngr_result_type_set_t result_type_set;
} lif_table_mngr_outlif_unit_test_cases_t;

/*
 * Struct with the required information for In-LIF replace test case: 
 * Info per test case and Info per entry set for both
 * the Initial entry and the Replace entry. 
 */
typedef struct
{
    lif_table_mngr_test_case_info_t test_case_info;
    lif_table_mngr_result_type_set_t initial_set;
    lif_table_mngr_result_type_set_t replace_set;
} lif_table_mngr_inlif_replace_test_cases_t;

/*
 * Struct with the required information for Out-LIF replace test case: 
 * Info per test case, Out-LIF phase and Info per entry set for both 
 * the Initial entry and the Replace entry. 
 */
typedef struct
{
    lif_table_mngr_test_case_info_t test_case_info;
    lif_mngr_outlif_phase_e outlif_phase;
    lif_table_mngr_result_type_set_t initial_set;
    lif_table_mngr_result_type_set_t replace_set;
} lif_table_mngr_outlif_replace_test_cases_t;

/*
 * Test cases for the In-LIF unit test 
 */
static lif_table_mngr_inlif_unit_test_cases_t in_lif_table_case[] = {

    /*
     * Step 0 - Get Minimal size In-LIF result type with mandatory fields only.
     * Standard MP result type with mandatory fields only. (not set fields with the following attributes):
     * UNSET_ACTION_IGNORE - DBAL_FIELD_VLAN_EDIT_PROFILE, UNSET_ACTION_SET_TO_DEFAULT - DBAL_FIELD_PROPAGATION_PROF 
     * Selected result type: The minimal size MP result type is IN_LIF_AC_MP
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI},
      {1, 4}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP}},

    /*
     * Steps 1-2 - Verify that fields that exist in different result types enforce the correct result types
     * Standard MP result type with all the mandatory fields and a non-mandatory field -
     * DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC 
     * Selected result type: The minimal size MP result type with the above field is IN_LIF_AC_MP 
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC},
      {1, 4, 14}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP}},

    /*
     * Extended MP result type with all the mandatory fields 
     * Selected result type: The only MP result type with the above field is IN_LIF_AC_MP_LARGE 
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT},
      {2, 4, 15}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE}},

    /*
     * Steps 3-4 - Missing or wrong fields that prevent a valid result type 
     * Standard MP result type with a missing mandatory field (DBAL_FIELD_VSI) 
     * Selected result type: No result type is expected to be found 
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC},
      {1, 13}, RESULT_TYPE_INVALID}},

    /*
     * Non-existing field combination for the table (LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT & LIF_GENERIC_DATA_1)
     * Selected result type: No result type is expected to be found  
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {4, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT,
          DBAL_FIELD_LIF_GENERIC_DATA_1},
      {1, 4, 1, 1}, RESULT_TYPE_INVALID}},

    /*
     * Step 5-6 - Result types affected by a value of an ENUM field with an ARR Prefix
     * Standard MP result type with mandatory fields and an ENUM field with ARR Prefix in range (FODO_ASSIGNMEN_MODE). 
     * Selected result type: The minimal size MP result type that has the above field in any size
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_FODO_ASSIGNMENT_MODE},
      {1, 4, DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_VLAN},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP}},

    /*
     * MP result type with mandatory fields and an ENUM field with ARR Prefix out of range  
     * (FODO_ASSIGNMEN_MODE) for the standard MP result type but in range for the MP-Large result types. 
     * Selected result type: The minimal size MP result type that has the above field in a size that fits a larger converted
     *   ENUM value that isn't small enough for the selected result type in the previous step
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_FODO_ASSIGNMENT_MODE},
      {1, 4, DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA}},

    /*
     * Step 7-8 - Result types affected by a value of an ENUM field without an ARR Prefix with size difference between result types
     * MP result type with mandatory fields and an ENUM field without an ARR Prefix (LEARN_PAYLOAD_CONTEXT), with a value of 1 bit
     * Selected result type: The minimal size MP result type that has the above field in any size
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT},
      {1, 4, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_BASIC}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP}},

    /*
     * MP result type with mandatory fields and an ENUM field without an ARR Prefix (LEARN_PAYLOAD_CONTEXT) with size difference 
     * Mandatory fields for an MP result type with a larger ENUM value.
     * Selected result type: The minimal size MP result type that has the above field in a size that fits a larger converted
     *   ENUM value that isn't small enough for the selected result type in the previous step
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT},
      {1, 4, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_BASIC}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE}},

    /*
     * Step 9 - Get Minimal size result type with mandatory fields only while using LIF Table mngr specific flags
     * P2P result type with mandatory fields. 
     * Selected result type: The minimal size P2P result type is IN_LIF_AC_P2P
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P}},

    /*
     * Steps 10-11 - Result types affected by a value of a non-ENUM field with an ARR Prefix
     * Standard P2P result type with mandatory fields only and a field with ARR Prefix in range for 
     * of the table's results (PROPAGATION_PROF). 
     * Selected result type: The minimal size P2P result type that has the above field in any size 
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_PROPAGATION_PROF},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P, 1}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P}},

    /*
     * P2P result type with mandatory fields and a field with ARR Prefix out of range 
     * (PROPAGATION_PROF) for the standard P2P result type but in range for the P2P-Large result type. 
     * Selected result type: The minimal size MP result type that has the above field in a large enough size
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_PROPAGATION_PROF},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P, 5},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE}},

    /*
     * Steps 12-13 - Result types affected by a value of a non-ENUM field with no ARR Prefix 
     * Large P2P result type with mandatory fields and a field with No-ARR value in range 
     * (LIF_GERNERIC_DATA_0) for the Large-P2P result type but in range for the P2P-Large result type. 
     * Selected result type: The minimal size P2P result type that has the above field in any size
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_LIF_GENERIC_DATA_0},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P, 79},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE}},

    /*
     * Large P2P result type with mandatory fields and a field with No-ARR value out of range 
     * (LIF_GERNERIC_DATA_0) for the Large-P2P result type but in range for the P2P-Large-Generic-Data result type. 
     * Selected result type: The minimal size P2P result type that has the above field in a large enough size 
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_LIF_GENERIC_DATA_0},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P, 279},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE_GENERIC_DATA}},

    /*
     * Steps 14-15 - Result types affected by a value of an ENUM field with an ARR Prefix other than '0' and a const value (7)
     * P2P result type with mandatory fields and a field (LEARN_PAYLOAD_CONTEXT) that is supplied with the expected const value (7) 
     * Selected result type: The minimal size P2P result type that has the above field in any size 
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P,
       DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE}},

    /*
     * P2P result type with mandatory fields and a field (LEARN_PAYLOAD_CONTEXT) that is supplied with a value othet than the expected const value (7) 
     * Selected result type: No result type is expected to be found 
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_BASIC},
      RESULT_TYPE_INVALID}},
};

/*
 * Test cases for the Out-LIF unit test 
 */
static lif_table_mngr_outlif_unit_test_cases_t out_lif_table_case[] = {
    /*
     * Step 0 - Get Minimal size Out-LIF result type with mandatory fields only. 
     * Shortest Out-AC result type with mandatory fields only. (not set fields with the following attributes):
     * UNSET_ACTION_IGNORE - DBAL_FIELD_VLAN_EDIT_PROFILE, UNSET_ACTION_SET_TO_DEFAULT - DBAL_FIELD_PROTECTION_POINTER 
     * Selected result type: The minimal size result type is ETPS_AC_QOS_OAM
     */
    {{DBAL_TABLE_EEDB_OUT_AC, NO_SPECIAL_FLAGS}, LIF_MNGR_OUTLIF_PHASE_AC,
     {1, {DBAL_FIELD_EGRESS_LAST_LAYER},
      {TRUE}, DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_QOS_OAM}},

    /*
     * Steps 1-2 - Verify that fields that exist in different result types enforce the correct result types
     * Out-AC Stats-Protection result type with all the mandatory fields and a non-mandatory field -
     * DBAL_FIELD_PROTECTION_PATH 
     * Selected result type: The minimal size MP result type with the above field is AC_STAT_PROTECTION 
     */
    {{DBAL_TABLE_EEDB_OUT_AC, NO_SPECIAL_FLAGS}, LIF_MNGR_OUTLIF_PHASE_AC,
     {2, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_PROTECTION_PATH},
      {TRUE, 1}, DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_STAT_PROTECTION}},

    /*
     * Out-AC Triple Tag result type with all the mandatory fields and a non-mandatory field - DBAL_ECID 
     * Selected result type: The minimal size MP result type with the above field is ETPS_AC_TRIPLE_TAG 
     */
    {{DBAL_TABLE_EEDB_OUT_AC, NO_SPECIAL_FLAGS}, LIF_MNGR_OUTLIF_PHASE_AC,
     {3, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_SUB_TYPE_FORMAT, DBAL_FIELD_ECID_OR_PON_TAG},
      {TRUE, 0, 2}, DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_TRIPLE_TAG}},

    /*
     * Steps 3 - Verify that L2-Address fields are supported correctly
     *  
     * ARP+AC result type with all the mandatory fields and a MAC Address
     * Selected result type: The minimal size ARP+AC result type
     */
    {{DBAL_TABLE_EEDB_ARP, DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_VLAN_TRANSLATION}, LIF_MNGR_OUTLIF_PHASE_AC,
     {4, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_VSI, DBAL_FIELD_ACTION_PROFILE, DBAL_FIELD_L2_MAC},
      {TRUE, 5, 3, 0x112233FF}, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC}},

    /*
     * Steps 4-5 - Missing or wrong fields that prevent a valid result type 
     * Out-AC result type with a missing mandatory field (DBAL_FIELD_EGRESS_LAST_LAYER) 
     * Selected result type: No result type is expected to be found
     */
    {{DBAL_TABLE_EEDB_OUT_AC, NO_SPECIAL_FLAGS}, LIF_MNGR_OUTLIF_PHASE_AC,
     {1, {DBAL_FIELD_OUT_LIF_PROFILE},
      {1}, RESULT_TYPE_INVALID}},

    /*
     * Non-existing field combination for the table (ECID_OR_PON_TAG & NEXT_OUTLIF_POINTER)
     * Selected result type: No result type is expected to be found 
     */
    {{DBAL_TABLE_EEDB_OUT_AC, NO_SPECIAL_FLAGS}, LIF_MNGR_OUTLIF_PHASE_AC,
     {2, {DBAL_FIELD_ECID_OR_PON_TAG, DBAL_FIELD_NEXT_OUTLIF_POINTER},
      {1, 1}, RESULT_TYPE_INVALID}},
};

/*
 * Test cases for the In-LIF Replace test 
 */
static lif_table_mngr_inlif_replace_test_cases_t in_lif_table_replace_case[] = {

    /*
     * Step 0 - Replace to a field that exists only on a different result type
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI},
      {1, 4}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_STAT_OBJECT_ID},
      {1, 6, 5}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA}},

    /*
     * Step 1 -  Replace to a Non-existing field combination for the table 
     * (LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT & LIF_GENERIC_DATA_1)
     * Selected result type: No result type is expected to be found  
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT},
      {1, 4, 1}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE},
     {4,
      {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT,
       DBAL_FIELD_LIF_GENERIC_DATA_1},
      {1, 4, 1, 5}, RESULT_TYPE_INVALID}},

    /*
     * Step 2 - Replace fields with both used (IN_LIF_PROFILE) and unused (STAT_OBJECT_ID) values 
     * and with valid existence replace from one existing value to another 
     * Expect the result type not to change 
     */

    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {4, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_IN_LIF_PROFILE, DBAL_FIELD_STAT_OBJECT_ID},
      {1, 4, 3, 11}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA},
     {4, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_IN_LIF_PROFILE, DBAL_FIELD_STAT_OBJECT_ID},
      {1, 4, 9, 50}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA}},

    /*
     * Step 3 - Replace fields with both used (IN_LIF_PROFILE) and unused (STAT_OBJECT_ID) values 
     * and with valid existence replace from an existing value to a non-existing value 
     * by not supplying the replaced field and value
     * Expect the result type to change to a smaller result type
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {4, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_IN_LIF_PROFILE, DBAL_FIELD_STAT_OBJECT_ID},
      {1, 4, 3, 11}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI},
      {1, 4}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP}},

    /*
     * Step 4 - Replace fields with both used (IN_LIF_PROFILE) and unused (STAT_OBJECT_ID) values 
     * and with valid existence replace from a non-existing state to an existing value 
     * Expect the result type to change to a larger result type
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI},
      {1, 4}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP},
     {4, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_IN_LIF_PROFILE, DBAL_FIELD_STAT_OBJECT_ID},
      {1, 4, 3, 11}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA}},

    /*
     * Step 5 - Replace fields with a used fields that is mandatory (GLOB_IN_LIF) or not (IN_LIF_PROFILE)
     * with valid replace but invalid existence replace from one existing value to another 
     * Expect the result type not to change
     */
    {{DBAL_TABLE_IN_LIF_IPvX_TUNNELS, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_IN_LIF_PROFILE},
      {1, 9}, DBAL_RESULT_TYPE_IN_LIF_IPvX_TUNNELS_IN_LIF_IPVX_3},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_IN_LIF_PROFILE},
      {2, 11}, DBAL_RESULT_TYPE_IN_LIF_IPvX_TUNNELS_IN_LIF_IPVX_3}},

    /*
     * Step 6 - Replace a non-mandatory field (IN_LIF_PROFILE)
     * with valid replace but invalid existence replace from an existing value to a non-existing 
     * by not supplying the replaced field and value  
     * Expect the operation to fail
     */
    {{DBAL_TABLE_IN_LIF_IPvX_TUNNELS, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_IN_LIF_PROFILE},
      {1, 9}, DBAL_RESULT_TYPE_IN_LIF_IPvX_TUNNELS_IN_LIF_IPVX_3},
     {1, {DBAL_FIELD_GLOB_IN_LIF},
      {1}, RESULT_TYPE_INVALID}},

    /*
     * Step 7 - Replace a non-mandatory field (IN_LIF_PROFILE)
     * with valid replace but invalid existence replace from a non-existing state to an existing value 
     * Expect the operation to fail
     */
    {{DBAL_TABLE_IN_LIF_IPvX_TUNNELS, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_IN_LIF_PROFILE},
      {1, 9}, DBAL_RESULT_TYPE_IN_LIF_IPvX_TUNNELS_IN_LIF_IPVX_3},
     {1, {DBAL_FIELD_GLOB_IN_LIF},
      {1}, RESULT_TYPE_INVALID}},

    /*
     * Step 8 - Replace a non-mandatory field (SERVICE_TYPE) with an invalid replace attribute
     * Expect the operation to fail
     */
    {{DBAL_TABLE_IN_LIF_FORMAT_EVPN, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_SERVICE_TYPE},
      {1, 5, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P},
      DBAL_RESULT_TYPE_IN_LIF_FORMAT_EVPN_IN_LIF_EVPN_EVI_P2P_NO_LEARNING},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_SERVICE_TYPE},
      {1, 5, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2MP}, RESULT_TYPE_INVALID}},

    /*
     * Step 9-10 - Replace an unused field (VLAN_EDIT_VID_1) with an invalid replace attribute but with 
     * valid existence replace 
     * First, just replace the value
     * Expect the same result type due to the fact that the value isn't used
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_VLAN_EDIT_VID_1},
      {1, 4, 100}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_VLAN_EDIT_VID_1},
      {1, 6, 200}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP}},

    /*
     * Second, Add the field only at Replace
     * Expect the Replace to succeed
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI},
      {1, 4}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_VLAN_EDIT_VID_1},
      {1, 6, 200}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP}},

    /*
     * Step 11 - Result types replaced due to a value of an ENUM field with an ARR Prefix
     * Standard MP result type with mandatory fields and an ENUM field with ARR Prefix in range (FODO_ASSIGNMEN_MODE) 
     * is replaced with a value that fits only a larger converted value that fits a different result type
     * Expect the result type to change to the result type that fits the larger FODO_ASSIGNMEN_MODE
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_FODO_ASSIGNMENT_MODE},
      {1, 4, DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_VLAN},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_FODO_ASSIGNMENT_MODE},
      {1, 4, DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA}},

    /*
     * Step 12 - Result types replaced due to a value of an ENUM field without an ARR Prefix
     * Standard MP result type with mandatory fields and an ENUM field without an ARR Prefix (LEARN_PAYLOAD_CONTEXT) 
     * with a value of 1 bit, is replaced with a value that fits only a larger converted value that fits a different result type
     * Expect the result type to change to the result type that fits the larger LEARN_PAYLOAD_CONTEXT
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT},
      {1, 4, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_BASIC},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT},
      {1, 4, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_BASIC},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE}},

    /*
     * Step 13 - Result types replaced due to a value of an ENUM field without an ARR Prefix
     * Standard MP result type with mandatory fields and an ENUM field without an ARR Prefix (LEARN_PAYLOAD_CONTEXT) 
     * with a value larger of 1 bit, is replaced with a value that fits only a shorter converted value that fits a shorter result type
     * Expect the result type to change to the shorter result type that fits the shorter LEARN_PAYLOAD_CONTEXT
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT},
      {1, 4, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_BASIC},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT},
      {1, 4, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_BASIC},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP}},

    /*
     * Step 14 - Result types replaced due to a value of a non-ENUM field with an ARR Prefix
     * Standard P2P result type with mandatory fields and a non-ENUM field with an ARR Prefix (PROPAGATION_PROF) 
     * with a value in range for the short P2P result type, is replaced with a value that fits only a larger converted 
     * value that fits a larger result type.
     * Expect the result type to change to the larger result type that fits the larger PROPAGATION_PROF
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_PROPAGATION_PROF},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P, 1},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_PROPAGATION_PROF},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P, 5},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE}},

    /*
     * Step 15 - Result types replaced due to a value of a non-ENUM field with an ARR Prefix
     * Large P2P result type with mandatory fields and a non-ENUM field with an ARR Prefix (PROPAGATION_PROF) 
     * with a value in range only for the longer P2P result type, is replaced with a value that can fit a
     * shorter converted value that fits a shorter result type.
     * Expect the result type to change to the shorter result type that fits the shorter PROPAGATION_PROF
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_PROPAGATION_PROF},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P, 5},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_PROPAGATION_PROF},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P, 1},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P}},

    /*
     * Step 16 - Result types replaced due to a value of a non-ENUM field with an ARR Prefix
     * Large P2P result type with mandatory fields and a non-ENUM field with no ARR Prefix (LIF_GENERIC_DATA_0) 
     * with a value in range for the Large P2P result type, is replaced with a value that fits only a larger converted 
     * value that fits a different result type.
     * Expect the result type to change to the result type that fits the larger LIF_GENERIC_DATA_0
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_LIF_GENERIC_DATA_0},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P, 79},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_LIF_GENERIC_DATA_0},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P, 279},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE_GENERIC_DATA}},

};

/*
 * Test cases for the Out-LIF Replace test 
 */
static lif_table_mngr_outlif_replace_test_cases_t out_lif_table_replace_case[] = {

    /*
     * Step 0 - Replace to a field that exists only on a different result type
     */
    {{DBAL_TABLE_EEDB_ARP, DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_VLAN_TRANSLATION}, LIF_MNGR_OUTLIF_PHASE_AC,
     {2, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_VSI},
      {TRUE, 5}, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC},
     {3, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_VSI, DBAL_FIELD_STAT_OBJECT_CMD},
      {TRUE, 5, 25}, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC_1TAG_STAT}},

    /*
     * Step 1 -  Replace to a Non-existing field combination for the table 
     * (STAT_OBJECT_CMD & VLAN_EDIT_VID_2)
     * Selected result type: No result type is expected to be found  
     */
    {{DBAL_TABLE_EEDB_ARP, DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_VLAN_TRANSLATION}, LIF_MNGR_OUTLIF_PHASE_AC,
     {3, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_VSI, DBAL_FIELD_STAT_OBJECT_CMD},
      {TRUE, 5, 25}, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC_1TAG_STAT},
     {4, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_VSI, DBAL_FIELD_STAT_OBJECT_CMD, DBAL_FIELD_VLAN_EDIT_VID_2},
      {TRUE, 5, 25, 99}, RESULT_TYPE_INVALID}},

    /*
     * Step 2 - Replace fields with both used (OUT_LIF_PROFILE) and unused (STAT_OBJECT_ID) values 
     * and with valid existence replace from one existing value to another 
     * Expect the result type not to change 
     */
    {{DBAL_TABLE_EEDB_OUT_AC, NO_SPECIAL_FLAGS}, LIF_MNGR_OUTLIF_PHASE_AC,
     {3, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_OUT_LIF_PROFILE, DBAL_FIELD_STAT_OBJECT_ID},
      {1, 4, 150}, DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_STAT_PROTECTION},
     {3, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_OUT_LIF_PROFILE, DBAL_FIELD_STAT_OBJECT_ID},
      {1, 3, 261}, DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_STAT_PROTECTION}},

    /*
     * Step 3 - Replace fields with both used (ENCAP_QOS_MODEL) and unused (STAT_OBJECT_CMD) values 
     * and with valid existence replace from an existing value to a non-existing value 
     * by not supplying the replaced field and value
     * Expect the result type to change to a smaller result type
     */
    {{DBAL_TABLE_EEDB_ARP, DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_VLAN_TRANSLATION}, LIF_MNGR_OUTLIF_PHASE_AC,
     {4, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_VSI, DBAL_FIELD_ENCAP_QOS_MODEL, DBAL_FIELD_STAT_OBJECT_CMD},
      {1, 5, 2, 19}, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC_1TAG_STAT},
     {2, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_VSI},
      {1, 5}, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC}},

    /*
     * Step 4 - Replace fields with both used (ENCAP_QOS_MODEL) and unused (STAT_OBJECT_CMD) values 
     * and with valid existence replace from a non-existing state to an existing value 
     * Expect the result type to change to a larger result type
     */
    {{DBAL_TABLE_EEDB_ARP, DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_VLAN_TRANSLATION}, LIF_MNGR_OUTLIF_PHASE_AC,
     {2, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_VSI},
      {1, 5}, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC},
     {4, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_VSI, DBAL_FIELD_ENCAP_QOS_MODEL, DBAL_FIELD_STAT_OBJECT_CMD},
      {1, 5, 2, 19}, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC_1TAG_STAT}},

    /*
     * Step 5 - Replace fields with a used field that is mandatory (SUB_TYPE_FORMAT & ECID_OR_PON_TAG)
     * with valid replace but invalid existence replace from one existing value to another 
     * Expect the result type not to change
     */
    {{DBAL_TABLE_EEDB_OUT_AC, NO_SPECIAL_FLAGS}, LIF_MNGR_OUTLIF_PHASE_AC,
     {3, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_SUB_TYPE_FORMAT, DBAL_FIELD_ECID_OR_PON_TAG},
      {1, 0, 201}, DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_TRIPLE_TAG},
     {3, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_SUB_TYPE_FORMAT, DBAL_FIELD_ECID_OR_PON_TAG},
      {1, 1, 11}, DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_TRIPLE_TAG}},

    /*
     * Step 6 - Replace fields with a used field that is non-mandatory (ESEM_COMMAND)
     * with valid replace but invalid existence replace from one existing value to another 
     * Expect the result type not to change
     */
    {{DBAL_TABLE_EEDB_MPLS_TUNNEL, NO_SPECIAL_FLAGS},
     LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_1,
     {2, {DBAL_FIELD_MPLS_LABEL, DBAL_FIELD_ESEM_COMMAND},
      {11, 2}, DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_1},
     {2, {DBAL_FIELD_MPLS_LABEL, DBAL_FIELD_ESEM_COMMAND},
      {11, 1}, DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_1}},

};

/**
 * \brief
 * Performs a sequence of an In-LIF entry configuration as part 
 * of a replace test case for the LIF Table manager validation. 
 * The sequence allocates handlers, calls the LIF-Table-Mngr 
 * API, verifies the retrieved field values and deletes the 
 * entry when required. 
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] test_case_info - A pointer to a structure that
 *         holds the test case attribures - DB Table ID and
 *         special flags
 *  \param [in] result_type_set_info - A pointer to a structure that
 *         holds the specific entry configuration - field IDs,
 *         field values, expected result type
 *  \param [in] is_replace - Indication whether the
 *         configuration is a Replace operation (or an Initial
 *         set)
 *  \param [in] is_delete - Indication whether an entry deletion
 *         is required as part of the sequence.
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   dnx_in_lif_table_unit_test, dnx_in_lif_table_replace_test
 */
shr_error_e
dnx_in_lif_table_set_fields_and_verify(
    int unit,
    lif_table_mngr_test_case_info_t * test_case_info,
    lif_table_mngr_result_type_set_t * result_type_set_info,
    int is_replace,
    int is_delete)
{
    uint32 entry_handle_id, get_entry_handle_id;
    uint32 num_of_fields, field_idx;
    uint32 dbal_result_type, ret_field_val;
    dbal_fields_e field_id;
    dbal_physical_tables_e phy_table_id;
    lif_table_mngr_inlif_info_t lif_info;
    dbal_tables_e dbal_table_id;
    int local_lif_id;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    printf("In-LIF Table manager set fields start\n");

    /*
     * Take the table handler
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, test_case_info->dbal_table_id, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    
    num_of_fields = result_type_set_info->num_of_fields;
    for (field_idx = 0; field_idx < num_of_fields; field_idx++)
    {
        field_id = result_type_set_info->field_id[field_idx];
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE,
                                         &(result_type_set_info->field_val[field_idx]));
    }

    /*
     * Get the physical table associated with this lif format. Verify that the physical table is EEDB.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, test_case_info->dbal_table_id,
                                                   DBAL_PHY_DB_DEFAULT_INDEX, &phy_table_id));

    /*
     * Call the LIF Table manager API to set the SW and the HW
     */
    sal_memset(&lif_info, 0, sizeof(lif_info));
    lif_info.flags = LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;
    if (is_replace)
    {
        lif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }
    lif_info.table_specific_flags = test_case_info->table_specific_flags;
    rv = dnx_lif_table_mngr_allocate_local_inlif_and_set_hw(unit, _SHR_CORE_ALL, entry_handle_id, &local_lif_id,
                                                            &lif_info);

    /*
     * Address the result type in case the LIF table manager allocation was successful
     */
    if (SHR_SUCCESS(rv))
    {
        /*
         * Retrieve the allocated result type 
         */
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                        (unit, local_lif_id, _SHR_CORE_ALL, phy_table_id, &dbal_table_id, &dbal_result_type, NULL));
        printf("dbal_result_type - %d, expected_result_type - %d\n", dbal_result_type,
               result_type_set_info->expected_result_type);

        /*
         * Verify the expected result type
         */
        if (result_type_set_info->expected_result_type == RESULT_TYPE_INVALID)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                              "FAILURE: Got result type %d allocated while no-result was expected %s%s\n",
                              dbal_result_type, EMPTY, EMPTY);
        }

        if (dbal_result_type != result_type_set_info->expected_result_type)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL, "FAILURE: Wrong result type, expected - %d, got - %d%s\n",
                              result_type_set_info->expected_result_type, dbal_result_type, EMPTY);
        }

        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, test_case_info->dbal_table_id, &get_entry_handle_id));
        dbal_entry_key_field32_set(unit, get_entry_handle_id, DBAL_FIELD_IN_LIF, local_lif_id);
        dbal_entry_value_field32_set(unit, get_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, dbal_result_type);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, get_entry_handle_id, DBAL_GET_ALL_FIELDS));

        for (field_idx = 0; field_idx < num_of_fields; field_idx++)
        {
            dbal_enum_value_field_lif_table_manager_set_action_e field_set_action;
            dbal_enum_value_field_lif_table_manager_unset_action_e field_not_set_action;
            uint32 field_set_value = 0;

            field_id = result_type_set_info->field_id[field_idx];
            /*
             * Get a field
             */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, get_entry_handle_id, field_id, INST_SINGLE, &ret_field_val));

            /*
             * Decide on the comparison value.
             * If the configured value wasn't used (LIF-Table-Mngr field attribute), 
             * get the predefined default value 
             */
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_field_set_actions_get
                            (unit, test_case_info->dbal_table_id, field_id, &field_set_action, &field_not_set_action));
            if (field_set_action == DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_SET_ACTION_USE_VALUE)
            {
                field_set_value = result_type_set_info->field_val[field_idx];
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                                (unit, test_case_info->dbal_table_id, field_id, FALSE,
                                 dbal_result_type, INST_SINGLE, DBAL_PREDEF_VAL_DEFAULT_VALUE, &field_set_value));
            }

            /*
             * Compare the retrieved value with the configured value
             */
            if (ret_field_val != field_set_value)
            {
                SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                                  "FAILURE: Wrong value retrieved for field %s, expected - %d, got - %d\n",
                                  dbal_field_to_string(unit, field_id), field_set_value, ret_field_val);
            }
        }

        /*
         * Delete the In-LIF through the LIF Table manager and verify 
         */
        if (is_delete)
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_inlif_info_clear
                            (unit, _SHR_CORE_ALL, local_lif_id, test_case_info->dbal_table_id,
                             LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING));
            rv = dbal_entry_get(unit, get_entry_handle_id, DBAL_GET_ALL_FIELDS);
            if (SHR_SUCCESS(rv))
            {
                SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                                  "FAILURE: dnx_lif_table_mngr_inlif_info_clear for lif_id #%d, didn't remove the entry %s%s\n",
                                  local_lif_id, EMPTY, EMPTY);
            }
        }
    }
    else
    {
        if (result_type_set_info->expected_result_type != RESULT_TYPE_INVALID)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                              "FAILURE: Failed to get result type. expected result type - %d %s%s\n",
                              result_type_set_info->expected_result_type, EMPTY, EMPTY);
        }
        printf("=> In-LIF Table manager set fields eneded with no result type!\n");
    }

    printf("In-LIF Table manager set fields end\n");
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Performs a sequence of an Out-LIF entry configuration as part
 * of a replace test case for the LIF Table manager validation. 
 * The sequence allocates handlers, calls the LIF-Table-Mngr 
 * API, verifies the retrieved field values and deletes the 
 * entry when required. 
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] test_case_info - A pointer to a structure that
 *         holds the test case attribures - DB Table ID and
 *         special flags
 *  \param [in] result_type_set_info - A pointer to a structure that
 *         holds the specific entry configuration - field IDs,
 *         field values, expected result type
 *  \param [in] outlif_phase - The Out-LIF phase that is
 *         appropriate for the test's configuration.
 *  \param [in] is_replace - Indication whether the
 *         configuration is a Replace operation (or an Initial
 *         set)
 *  \param [in] is_delete - Indication whether an entry deletion
 *         is required as part of the sequence.
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   dnx_out_lif_table_unit_test
 */
shr_error_e
dnx_out_lif_table_set_fields_and_verify(
    int unit,
    lif_table_mngr_test_case_info_t * test_case_info,
    lif_table_mngr_result_type_set_t * result_type_set_info,
    lif_mngr_outlif_phase_e outlif_phase,
    int is_replace,
    int is_delete)
{
    uint32 entry_handle_id, get_entry_handle_id;
    uint32 num_of_fields, field_idx;
    uint32 dbal_result_type, ret_field_val;
    dbal_fields_e field_id;
    lif_table_mngr_outlif_info_t lif_info;
    dbal_tables_e dbal_table_id;
    int local_lif_id;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    printf("Out-LIF Table manager set fields start\n");

    /*
     * Take the table handler
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, test_case_info->dbal_table_id, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    
    num_of_fields = result_type_set_info->num_of_fields;
    for (field_idx = 0; field_idx < num_of_fields; field_idx++)
    {
        field_id = result_type_set_info->field_id[field_idx];
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE,
                                         &(result_type_set_info->field_val[field_idx]));
    }

    /*
     * Call the LIF Table manager API to set the SW and the HW
     */
    sal_memset(&lif_info, 0, sizeof(lif_info));
    lif_info.flags = LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;
    if (is_replace)
    {
        lif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }
    lif_info.table_specific_flags = test_case_info->table_specific_flags;
    lif_info.outlif_phase = outlif_phase;
    rv = dnx_lif_table_mngr_allocate_local_outlif_and_set_hw(unit, entry_handle_id, &local_lif_id, &lif_info);

    /*
     * Address the result type in case the LIF table manager allocation was successful
     */
    if (SHR_SUCCESS(rv))
    {
        /*
         * Retrieve the allocated result type 
         */
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get
                        (unit, local_lif_id, &dbal_table_id, &dbal_result_type, NULL, NULL, NULL));
        printf("dbal_result_type - %d, expected_result_type - %d\n", dbal_result_type,
               result_type_set_info->expected_result_type);

        /*
         * Verify the expected result type
         */
        if (result_type_set_info->expected_result_type == RESULT_TYPE_INVALID)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                              "FAILURE: Got result type %d allocated while no-result was expected %s%s\n",
                              dbal_result_type, EMPTY, EMPTY);
        }

        if (dbal_result_type != result_type_set_info->expected_result_type)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL, "FAILURE: Wrong result type, expected - %d, got - %d%s\n",
                              result_type_set_info->expected_result_type, dbal_result_type, EMPTY);
        }

        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, test_case_info->dbal_table_id, &get_entry_handle_id));
        dbal_entry_key_field32_set(unit, get_entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif_id);
        dbal_entry_value_field32_set(unit, get_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, dbal_result_type);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, get_entry_handle_id, DBAL_GET_ALL_FIELDS));

        for (field_idx = 0; field_idx < num_of_fields; field_idx++)
        {
            dbal_enum_value_field_lif_table_manager_set_action_e field_set_action;
            dbal_enum_value_field_lif_table_manager_unset_action_e field_not_set_action;
            uint32 field_set_value = 0;

            field_id = result_type_set_info->field_id[field_idx];
            /*
             * Get a field
             */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, get_entry_handle_id, field_id, INST_SINGLE, &ret_field_val));

            /*
             * Decide on the comparison value.
             * If the configured value wasn't used (LIF-Table-Mngr field attribute), 
             * get the predefined default value 
             */
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_field_set_actions_get
                            (unit, test_case_info->dbal_table_id, field_id, &field_set_action, &field_not_set_action));
            if (field_set_action == DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_SET_ACTION_USE_VALUE)
            {
                field_set_value = result_type_set_info->field_val[field_idx];
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                                (unit, test_case_info->dbal_table_id, field_id, FALSE,
                                 dbal_result_type, INST_SINGLE, DBAL_PREDEF_VAL_DEFAULT_VALUE, &field_set_value));
            }

            /*
             * Compare the retrieved value with the configured value
             */
            if (ret_field_val != field_set_value)
            {
                SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                                  "FAILURE: Wrong value retrieved for field %s, expected - %d, got - %d\n",
                                  dbal_field_to_string(unit, field_id), field_set_value, ret_field_val);
            }
        }

        /*
         * Delete the Out-LIF through the LIF Table manager and verify 
         */
        if (is_delete)
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear
                            (unit, local_lif_id, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING));
            rv = dbal_entry_get(unit, get_entry_handle_id, DBAL_GET_ALL_FIELDS);
            if (SHR_SUCCESS(rv))
            {
                SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                                  "FAILURE: dnx_lif_table_mngr_outlif_info_clear for lif_id #%d, didn't remove the entry %s%s\n",
                                  local_lif_id, EMPTY, EMPTY);
            }
        }
    }
    else
    {
        if (result_type_set_info->expected_result_type != RESULT_TYPE_INVALID)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                              "FAILURE: Failed to get result type. expected result type - %d %s%s\n",
                              result_type_set_info->expected_result_type, EMPTY, EMPTY);
        }
        printf("=> Out-LIF Table manager set fields eneded with no result type!\n");
    }

    printf("Out-LIF Table manager set fields end\n");
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * runs unit test for the In-Lif side of the LIF Table manager
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] args - added to match with shell cmds structure
 *  \param [in] sand_control - added to match with shell cmds structure
 * \par DIRECT OUTPUT:
 *    \retval Zero if no error was detected
 *    \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT INPUT
 *  \param [in] *args - see args in direct input above
 *  \param [in] *sand_control - see sand_control in direct input above
 * \par INDIRECT OUTPUT
 *   the executed test
 */
shr_error_e
dnx_in_lif_table_unit_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 test_case;
    uint32 num_of_test_cases = sizeof(in_lif_table_case) / sizeof(in_lif_table_case[0]);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    printf("n\n => In-LIF Table manager unit TEST Started, num_of_test_cases = %d\n", num_of_test_cases);

    /*
     * Traverse all the predefined test cases
     */
    for (test_case = 0; test_case < num_of_test_cases; test_case++)
    {
        /*
         * Set the entry's setting via the LIF Table manager API, 
         * including result type and field values verification
         */
        SHR_IF_ERR_EXIT(dnx_in_lif_table_set_fields_and_verify(unit,
                                                               &(in_lif_table_case[test_case].test_case_info),
                                                               &(in_lif_table_case[test_case].result_type_set), FALSE,
                                                               TRUE));

        printf(" ===> In-LIF Test case #%d PASSED!\n", test_case);
    }

    printf("\n\n ===> In-LIF Table manager unit TEST PASSED!\n");

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * runs unit test for the Out-Lif side of the LIF Table manager
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] args - added to match with shell cmds structure
 *  \param [in] sand_control - added to match with shell cmds structure
 * \par DIRECT OUTPUT:
 *    \retval Zero if no error was detected
 *    \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT INPUT
 *  \param [in] *args - see args in direct input above
 *  \param [in] *sand_control - see sand_control in direct input above
 * \par INDIRECT OUTPUT
 *   the executed test
 */
shr_error_e
dnx_out_lif_table_unit_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 test_case;
    uint32 num_of_test_cases = sizeof(out_lif_table_case) / sizeof(out_lif_table_case[0]);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    printf("n\n => Out-LIF Table manager unit TEST Started, num_of_test_cases = %d\n", num_of_test_cases);

    /*
     * Traverse all the predefined test cases
     */
    for (test_case = 0; test_case < num_of_test_cases; test_case++)
    {
        /*
         * Set the entry's setting via the LIF Table manager API,
         * including result type and field values verification
         */
        SHR_IF_ERR_EXIT(dnx_out_lif_table_set_fields_and_verify(unit,
                                                                &(out_lif_table_case[test_case].test_case_info),
                                                                &(out_lif_table_case[test_case].result_type_set),
                                                                out_lif_table_case[test_case].outlif_phase, FALSE,
                                                                TRUE));

        printf(" ===> Out-LIF Test case #%d PASSED!\n", test_case);
    }

    printf("\n\n => Out-LIF Table manager unit TEST PASSED!\n");

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * runs a replace test for the In-Lif side of the LIF Table 
 * manager 
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] args - added to match with shell cmds structure
 *  \param [in] sand_control - added to match with shell cmds structure
 * \par DIRECT OUTPUT:
 *    \retval Zero if no error was detected
 *    \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT INPUT
 *  \param [in] *args - see args in direct input above
 *  \param [in] *sand_control - see sand_control in direct input above
 * \par INDIRECT OUTPUT
 *   the executed test
 */
shr_error_e
dnx_in_lif_table_replace_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 test_case;
    uint32 num_of_test_cases = sizeof(in_lif_table_replace_case) / sizeof(in_lif_table_replace_case[0]);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    printf("n\n => In-LIF Table manager Replace TEST Started, num_of_test_cases = %d\n", num_of_test_cases);

    /*
     * Traverse all the predefined test cases
     */
    for (test_case = 0; test_case < num_of_test_cases; test_case++)
    {
        /*
         * Set the entry's initial setting via the LIF Table manager API, 
         * Including result type and field values verification
         */
        SHR_IF_ERR_EXIT(dnx_in_lif_table_set_fields_and_verify(unit,
                                                               &(in_lif_table_replace_case[test_case].test_case_info),
                                                               &(in_lif_table_replace_case[test_case].initial_set),
                                                               FALSE, FALSE));

        /*
         * Set the entry's replace setting via the LIF Table manager API, 
         * Including result type and field values verification
         */
        SHR_IF_ERR_EXIT(dnx_in_lif_table_set_fields_and_verify(unit,
                                                               &(in_lif_table_replace_case[test_case].test_case_info),
                                                               &(in_lif_table_replace_case[test_case].replace_set),
                                                               TRUE, TRUE));

        printf(" ===> In-LIF REPLACE Test case #%d PASSED!\n", test_case);
    }

    printf("\n\n => In-LIF Table manager Replace TEST PASSED!\n");

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * runs a replace test for the Out-Lif side of the LIF Table 
 * manager 
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] args - added to match with shell cmds structure
 *  \param [in] sand_control - added to match with shell cmds structure
 * \par DIRECT OUTPUT:
 *    \retval Zero if no error was detected
 *    \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT INPUT
 *  \param [in] *args - see args in direct input above
 *  \param [in] *sand_control - see sand_control in direct input above
 * \par INDIRECT OUTPUT
 *   the executed test
 */
shr_error_e
dnx_out_lif_table_replace_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 test_case;
    uint32 num_of_test_cases = sizeof(out_lif_table_replace_case) / sizeof(out_lif_table_replace_case[0]);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    printf("\n\n => Out-LIF Table manager Replace TEST Started, num_of_test_cases = %d\n", num_of_test_cases);

    /*
     * Traverse all the predefined test cases
     */
    for (test_case = 0; test_case < num_of_test_cases; test_case++)
    {
        /*
         * Set the entry's initial setting via the LIF Table manager API, 
         * Including result type and field values verification
         */
        SHR_IF_ERR_EXIT(dnx_out_lif_table_set_fields_and_verify(unit,
                                                                &(out_lif_table_replace_case[test_case].test_case_info),
                                                                &(out_lif_table_replace_case[test_case].initial_set),
                                                                out_lif_table_replace_case[test_case].outlif_phase,
                                                                FALSE, FALSE));

        /*
         * Set the entry's replace setting via the LIF Table manager API, 
         * Including result type and field values verification
         */
        SHR_IF_ERR_EXIT(dnx_out_lif_table_set_fields_and_verify(unit,
                                                                &(out_lif_table_replace_case[test_case].test_case_info),
                                                                &(out_lif_table_replace_case[test_case].replace_set),
                                                                out_lif_table_replace_case[test_case].outlif_phase,
                                                                TRUE, TRUE));

        printf(" ===> Out-LIF REPLACE Test case #%d PASSED!\n", test_case);
    }

    printf("\n\n => Out-LIF Table manager Replace TEST PASSED!\n");

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

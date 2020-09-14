/** \file diag_dnx_lif_table_mngr.c
 *
 * LIF Table manager unit test.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
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
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <src/soc/dnx/dbal/dbal_internal.h>

extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

/*
 * DEFINES
 */
#define MAX_NUM_LIF_TABLE_FIELDS        (10)

/*
 * Default LIF-Table-Mngr API special flags value when no flags are required
 */
#define NO_SPECIAL_FLAGS                (0)

/*
 * Special expected result type values 
 * INVALID - No result type is found 
 * CHANGED_OR_INVALID - A result type that differs from the deafault is 
 *                      expected, but depending on the table may result in no
 *                      valid result type
 */
#define RESULT_TYPE_INVALID             (-1)
#define RESULT_TYPE_CHANGED_OR_INVALID  (-2)

/*
 * Special expected retrieve field values for Negative testing
 */
#define VALUE_FIELD_NOT_FOUND           (0xFFFE)
#define VALUE_FIELD_ENCODE_ERROR        (0xFFFD)

/*
 * Child field test Set and Retrieve steps 
 * The steps depict the field that is either Set or Retrieved 
 * All the Get step are applied after each Set step 
 * The last step - CHILD_TEST_STEP_OTHER_PARENT, is a Parent field encoded with 
 * the other child field - isn't relevant as a Get step as it's the same as the 
 * Parent step.
 */
#define CHILD_TEST_NOF_SET_STEPS        (4)
#define CHILD_TEST_NOF_GET_STEPS        (3)

#define ST_PARENT          (0)
#define ST_CHILD           (1)
#define ST_OTHER_CHILD     (2)
#define ST_OTHER_PARENT    (3)

#define RETURN_VALUE_NOT_INITIALIZED    (0x01010101)

/*
 * structs to support bcm shell command
 */
sh_sand_man_t dnx_in_lif_table_unit_test_man = {
    "Unit Test for the In-LIF Table manager",
    "Unit Test for the In-LIF Table manager. Provides basic scenarios"
};

sh_sand_man_t dnx_in_lif_child_field_test_man = {
    "Unit Test for the In-LIF Table manager with a child field",
    "Unit Test for the In-LIF Table manager. Validates child field handling"
};

sh_sand_man_t dnx_out_lif_table_unit_test_man = {
    "Unit Test for the Out-LIF Table manager",
    "Unit Test for the Out-LIF Table manager. Provides basic scenarios"
};

sh_sand_man_t dnx_in_lif_table_replace_test_man = {
    "Test the result type Replace functionality for the In-LIF Table manager",
    "Test the result type Replace functionality for the In-LIF Table manager. Provides result-type modification scenarios"
};

sh_sand_man_t dnx_in_lif_child_field_replace_test_man = {
    "Test the result type Replace functionality for the In-LIF Table manager with a child field",
    "Test the result type Replace functionality for the In-LIF Table manager with a child field. Provides result-type modification scenarios"
};

sh_sand_man_t dnx_out_lif_table_replace_test_man = {
    "Test the result type Replace functionality for the Out-LIF Table manager",
    "Test the result type Replace functionality for the Out-LIF Table manager. Provides result-type modification scenarios"
};

/*
 * Struct with the required information per test case
 */
typedef struct
{
    dbal_tables_e dbal_table_id;
    uint32 table_specific_flags;
} lif_table_mngr_test_case_info_t;

/*
 * Struct with the required information for an entry operation
 */
typedef struct
{
    uint32 num_of_fields;
    dbal_fields_e field_id[MAX_NUM_LIF_TABLE_FIELDS];
    uint32 field_val[MAX_NUM_LIF_TABLE_FIELDS];
    int expected_result_type;
} lif_table_mngr_result_type_set_t;

/*
 * Struct with information for child field testing
 */
typedef struct
{
    dbal_fields_e parent_field_id;
    dbal_fields_e child_field_id;
    dbal_fields_e other_child_field_id;
    dbal_fields_e field_id_in_result_type;
    uint32 child_field_val;
} lif_table_mngr_child_field_conf_t;

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
 * Struct with the required information for an In-LIF child field test case:
 * Info per test case, Info per entry set and Info per parent-child fields
 */
typedef struct
{
    lif_table_mngr_test_case_info_t test_case_info;
    lif_table_mngr_result_type_set_t result_type_set;
    lif_table_mngr_child_field_conf_t child_field_conf;
} lif_table_mngr_inlif_child_field_test_cases_t;

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
 * Struct with the required information for In-LIF replace child field test case:
 * Info per test case, Info per entry set for both
 * the Initial entry and the Replace entry and Info per parent-child fields
 */
typedef struct
{
    lif_table_mngr_test_case_info_t test_case_info;
    lif_table_mngr_result_type_set_t initial_set;
    lif_table_mngr_result_type_set_t replace_set;
    lif_table_mngr_child_field_conf_t child_field_replace_set;
} lif_table_mngr_inlif_child_field_replace_test_cases_t;

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
     * Selected result type: The minimal size MP result type is IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI},
      {1, 4}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD}},

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
     * Standard MP result type with mandatory fields and an ENUM field with ARR Prefix in range (FODO_ASSIGNMENT_MODE).
     * Selected result type: The minimal size MP result type that has the above field in any size
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_FODO_ASSIGNMENT_MODE},
      {1, 4, DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_VLAN},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD}},

    /*
     * MP result type with mandatory fields and an ENUM field with ARR Prefix out of range
     * (FODO_ASSIGNMEN_MODE) for the standard MP result type but in range for the MP-Large result types.
     * Selected result type: The minimal size MP result type that has the above field in a size that fits a larger converted
     *   ENUM value that isn't small enough for the selected result type in the previous step
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_FODO_ASSIGNMENT_MODE},
      {1, 4, DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE}},

    /*
     * Step 7-8 - Result types affected by a value of an ENUM field without an ARR Prefix with size difference between result types
     * MP result type with mandatory fields and an ENUM field without an ARR Prefix (LEARN_PAYLOAD_CONTEXT), with a value of 1 bit
     * Selected result type: The minimal size MP result type that has the above field in any size
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {4,
      {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
       DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC},
      {1, 4, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_BASIC, 100},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP}},

    /*
     * MP result type with mandatory fields and an ENUM field without an ARR Prefix (LEARN_PAYLOAD_CONTEXT) with size difference
     * Mandatory fields for an MP result type with a larger ENUM value.
     * Selected result type: The minimal size MP result type that has the above field in a size that fits a larger converted
     * ENUM value that isn't small enough for the selected result type in the previous step
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {4,
      {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
       DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC},
      {1, 4, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED, 100},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA}},

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
 * Test cases for an In-LIF test for fields with different get-set 
 * fields or values, such as child fields
 *  
 */
static lif_table_mngr_inlif_child_field_test_cases_t inlif_child_field_test_case[] = {

    /*
     * Step 0 - Configure a Parent field (DESTINATION) in a table that has 
     * no separetly child fields defined, and the child fields are not supported by the LIF-Table-Mngr
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE}, {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P},
     {DBAL_FIELD_DESTINATION, DBAL_FIELD_PORT_ID, DBAL_FIELD_LAG_ID, DBAL_FIELD_DESTINATION, 13}},

    /*
     * Step 1 - Configure a Child field (VSI) in a table that has 
     * no Parent field defined, only the child field.
     */
    {{DBAL_TABLE_IN_LIF_FORMAT_PWE, NO_SPECIAL_FLAGS},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {1}, DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_P2P_NO_LEARNING_OPTIMIZED},
     {DBAL_FIELD_FODO, DBAL_FIELD_VSI, DBAL_FIELD_VRF, DBAL_FIELD_VSI, 2}},

    /*
     * Step 2 - Configure a Parent field (FODO) in a table that has 
     * both a Parent field (FODO) and a Child field (VRF), and the 
     * Parent field has a lower index in the superset result type.
     */
    

    /*
     * {{DBAL_TABLE_IN_LIF_IPvX_TUNNELS, NO_SPECIAL_FLAGS}, {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_DESTINATION}, {1,
     * 13}, DBAL_RESULT_TYPE_IN_LIF_IPvX_TUNNELS_IN_LIF_IPVX_1}, {DBAL_FIELD_FODO, DBAL_FIELD_VRF, DBAL_FIELD_VSI,
     * DBAL_FIELD_FODO, 5}},
     */

    /*
     * Step 3 - Configure a Child field (VRF) in a table that has 
     * both a Parent field (FODO) and a Child field (VRF) and the 
     * Parent field has a lower index in the superset result type.
     */
    /*
     * {{DBAL_TABLE_IN_LIF_IPvX_TUNNELS, NO_SPECIAL_FLAGS}, {1, {DBAL_FIELD_GLOB_IN_LIF}, {1},
     * DBAL_RESULT_TYPE_IN_LIF_IPvX_TUNNELS_IN_LIF_IPVX_3}, {DBAL_FIELD_FODO, DBAL_FIELD_VRF, DBAL_FIELD_VSI,
     * DBAL_FIELD_VRF, 9}},
     */

    /*
     * Step 4 - Configure a Child field (VSI) in a table that has 
     * both a Parent field (FODO) and a Child field (VSI), and the 
     * Child field has a lower index in the superset result type.
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {1}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD},
     {DBAL_FIELD_FODO, DBAL_FIELD_VSI, DBAL_FIELD_VRF, DBAL_FIELD_VSI, 120}},
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
     * Out-AC Triple Tag result type with all the mandatory fields and a non-mandatory field - STAT_OBJECT_ID
     * Selected result type: The minimal size MP result type with the above field is ETPS_AC_TRIPLE_TAG
     */
    {{DBAL_TABLE_EEDB_OUT_AC, NO_SPECIAL_FLAGS}, LIF_MNGR_OUTLIF_PHASE_AC,
     {4,
      {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_ECID_OR_PON_TAG, DBAL_FIELD_SUB_TYPE_FORMAT, DBAL_FIELD_STAT_OBJECT_ID},
      {TRUE, 2, 0, 19}, DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_TRIPLE_TAG}},

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
      {1, 4}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_STAT_OBJECT_ID},
      {1, 6, 5}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE}},

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
      {1, 4, 3, 11}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE},
     {4, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_IN_LIF_PROFILE, DBAL_FIELD_STAT_OBJECT_ID},
      {1, 4, 9, 50}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE}},

    /*
     * Step 3 - Replace fields with both used (IN_LIF_PROFILE) and unused (STAT_OBJECT_ID) values
     * and with valid existence replace from an existing value to a non-existing value
     * by not supplying the replaced field and value
     * Expect the result type to change to a smaller result type
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {4, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_IN_LIF_PROFILE, DBAL_FIELD_STAT_OBJECT_ID},
      {1, 4, 3, 11}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI},
      {1, 4}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD}},

    /*
     * Step 4 - Replace fields with both used (IN_LIF_PROFILE) and unused (STAT_OBJECT_ID) values
     * and with valid existence replace from a non-existing state to an existing value
     * Expect the result type to change to a larger result type
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI},
      {1, 4}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD},
     {4, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_IN_LIF_PROFILE, DBAL_FIELD_STAT_OBJECT_ID},
      {1, 4, 3, 11}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE}},

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
      {1, 4, 100}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_VLAN_EDIT_VID_1},
      {1, 6, 200}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD}},

    /*
     * Second, Add the field only at Replace
     * Expect the Replace to succeed
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI},
      {1, 4}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_VLAN_EDIT_VID_1},
      {1, 6, 200}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD}},

    /*
     * Step 11 - Result types replaced due to a value of an ENUM field with an ARR Prefix
     * Standard MP result type with mandatory fields and an ENUM field with ARR Prefix in range (FODO_ASSIGNMEN_MODE)
     * is replaced with a value that fits only a larger converted value that fits a different result type
     * Expect the result type to change to the result type that fits the larger FODO_ASSIGNMEN_MODE
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_FODO_ASSIGNMENT_MODE},
      {1, 4, DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_VLAN},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_FODO_ASSIGNMENT_MODE},
      {1, 4, DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE}},

    /*
     * Step 12 - Result types replaced due to a value of an ENUM field without an ARR Prefix
     * Standard MP result type with mandatory fields and an ENUM field without an ARR Prefix (LEARN_PAYLOAD_CONTEXT)
     * with a value of 1 bit, is replaced with a value that fits only a larger converted value that fits a different result type
     * Expect the result type to change to the result type that fits the larger LEARN_PAYLOAD_CONTEXT
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {4,
      {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
       DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC},
      {1, 4, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_BASIC, 50},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP},
     {4,
      {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
       DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC},
      {1, 4, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_OPTIMIZED, 50},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA}},

    
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {4,
      {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
       DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC},
      {1, 4, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_OPTIMIZED, 50},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA},
     {4,
      {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
       DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC},
      {1, 4, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_BASIC, 50},
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
 * Test cases for the In-LIF child field Replace test 
 * The aim is to verify various existing combinations of field attributes for fields that 
 * are either defined in a table as Parent, child, or both exist on different result types: 
 * set_action: SET_ACTION_USE_VALUE or SET_ACTION_DO_NOT_USE_VALUE
 * unset_action: UNSET_ACTION_FORBIDDEN or UNSET_ACTION_IGNORE
 *               (UNSET_ACTION_SET_TO_DEFAULT isn't available on Ingress)
 * value_replace_valid: TRUE or FALSE or NONE(DESTINATION)
 * field_existance_replace_valid: TRUE or FALSE
 *  
 * The Replace operations may modify a value, but can also change to or from a result-type that has 
 * a Parent/Child field and can change between result types that have different limitations on the field's value
 */
static lif_table_mngr_inlif_child_field_replace_test_cases_t in_lif_child_field_replace_case[] = {

    /*
     * Steps 0-1 - Table with only Parent field existing (DESTINATION) 
     * set_action: SET_ACTION_USE_VALUE
     * unset_action: UNSET_ACTION_IGNORE
     * value_replace_valid: TRUE
     * field_existance_replace_valid: TRUE
     * Tested operations: 
     *   1. Change value to the same encoding (FEC) and to a different child encoding (PORT_ID) on the same result-type
     *   2. Change value to the same encoding (FEC) and to a different child encoding (PORT_ID) on a different result-type (PROPAGATION_PROF over 1bit)
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_DESTINATION},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P, 13},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE}, {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P},
     {DBAL_FIELD_DESTINATION, DBAL_FIELD_FEC, DBAL_FIELD_PORT_ID, DBAL_FIELD_DESTINATION, 200}},

    {{DBAL_TABLE_IN_AC_INFO_DB, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_DESTINATION},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P, 13},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_PROPAGATION_PROF},
      {1, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P, 5},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE},
     {DBAL_FIELD_DESTINATION, DBAL_FIELD_FEC, DBAL_FIELD_PORT_ID, DBAL_FIELD_DESTINATION, 100}},

    /*
     * Steps 2-5 - Table with both Parent (FODO) & Child (VSI) 
     * set_action: SET_ACTION_USE_VALUE
     * unset_action: UNSET_ACTION_FORBIDDEN
     * value_replace_valid: TRUE
     * field_existance_replace_valid: TRUE
     * Tested operations: 
     *   1. Change value for a result type with a Parent field (FODO) on the same result type
     *   2. Change value for a result type with a Child field (VSI) on the same result type
     *   3. Change value for a result type with a Child field (VSI) to a different result-type with the same Child field
     *   4. Change value for a result type with a Child field (VSI) to a different result-type without a Parent/Child field
     *   5. Change value for a result type without a Parent/Child field to a different result-type with a Child field (VSI)
     */
    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_IN_LIF_PROFILE, DBAL_FIELD_FODO}, {7, 30}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_RCH_CONTROL_LIF},
     {1, {DBAL_FIELD_IN_LIF_PROFILE}, {4}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_RCH_CONTROL_LIF},
     {DBAL_FIELD_FODO, DBAL_FIELD_VRF, DBAL_FIELD_VSI, DBAL_FIELD_FODO, 50}},

    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI}, {1001, 666},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {1001}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD},
     {DBAL_FIELD_FODO, DBAL_FIELD_VSI, DBAL_FIELD_VRF, DBAL_FIELD_VSI, 101}},

    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI}, {36, 2048},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC}, {36, 201},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP},
     {DBAL_FIELD_FODO, DBAL_FIELD_VSI, DBAL_FIELD_VRF, DBAL_FIELD_VSI, 1024}},

    /*
     * {{DBAL_TABLE_IN_AC_INFO_DB, DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P}, {5,
     * {DBAL_FIELD_GLOB_IN_LIF}, {11}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P}, {1, {DBAL_FIELD_GLOB_IN_LIF},
     * {11}, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_VSI_WO_PROTECTION}, {DBAL_FIELD_FODO, DBAL_FIELD_VSI,
     * DBAL_FIELD_VRF, DBAL_FIELD_VSI, 511}}, 
     */

    {{DBAL_TABLE_IN_AC_INFO_DB, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI}, {36, 2048},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC}, {36, 201},
      DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP},
     {DBAL_FIELD_FODO, DBAL_FIELD_VSI, DBAL_FIELD_VRF, DBAL_FIELD_VSI, 1024}},

    /*
     * Steps 6-8 - Table with only Child field (VSI) with various sizes 
     * set_action: SET_ACTION_USE_VALUE
     * unset_action: UNSET_ACTION_FORBIDDEN
     * value_replace_valid: TRUE
     * field_existance_replace_valid: TRUE
     * Tested operations: 
     *   1. Change value for a result type with a Child field (VSI) on the same result type
     *   2. Change value for a result type with a Child field (VSI) to a different result-type with a smaller Child field
     *   3. Change value for a result type with a Child field (VSI) to a different result-type with a larger Child field
     */
    {{DBAL_TABLE_IN_LIF_FORMAT_PWE, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI}, {4096, 79},
      DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_P2P_NO_LEARNING},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {4096}, DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_P2P_NO_LEARNING},
     {DBAL_FIELD_FODO, DBAL_FIELD_VSI, DBAL_FIELD_VRF, DBAL_FIELD_VSI, 97}},

    {{DBAL_TABLE_IN_LIF_FORMAT_PWE, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI}, {4096, 79},
      DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_P2P_NO_LEARNING},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {4096}, DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_P2P_NO_LEARNING_OPTIMIZED},
     {DBAL_FIELD_FODO, DBAL_FIELD_VSI, DBAL_FIELD_VRF, DBAL_FIELD_VSI, 3}},

    {{DBAL_TABLE_IN_LIF_FORMAT_PWE, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI}, {4096, 1},
      DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_P2P_NO_LEARNING_OPTIMIZED},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {4096}, DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_P2P_NO_LEARNING},
     {DBAL_FIELD_FODO, DBAL_FIELD_VSI, DBAL_FIELD_VRF, DBAL_FIELD_VSI, 2000}},

    /*
     * Steps 9-10 - Table with only Parent field (DESTINATION) not in all result types  
     * set_action: SET_ACTION_USE_VALUE
     * unset_action: UNSET_ACTION_IGNORE
     * value_replace_valid: TRUE
     * field_existance_replace_valid: TRUE
     * Tested operations: 
     *   1. Change value for a result type with a Parent field (DESTINATION) to a different result-type with no Parent or Child fields
     *   2. Change value for a result type with no Parent or Child fields to a different result-type with a Parent field (DESTINATION)
     */
    {{DBAL_TABLE_IN_LIF_FORMAT_EVPN, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_DESTINATION}, {33, 66, 99},
      DBAL_RESULT_TYPE_IN_LIF_FORMAT_EVPN_IN_LIF_EVPN_EVI_P2P_NO_LEARNING},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_LEARN_ENABLE}, {22, 1},
      DBAL_RESULT_TYPE_IN_LIF_FORMAT_EVPN_IN_LIF_EVPN_EVI_MP},
     {DBAL_FIELD_FODO, DBAL_FIELD_VSI, DBAL_FIELD_VRF, DBAL_FIELD_VSI, 66}},

    {{DBAL_TABLE_IN_LIF_FORMAT_EVPN, NO_SPECIAL_FLAGS},
     {3, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_VSI, DBAL_FIELD_LEARN_ENABLE}, {111, 222, 1},
      DBAL_RESULT_TYPE_IN_LIF_FORMAT_EVPN_IN_LIF_EVPN_EVI_MP},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_DESTINATION}, {111, 99},
      DBAL_RESULT_TYPE_IN_LIF_FORMAT_EVPN_IN_LIF_EVPN_EVI_P2P_NO_LEARNING},
     {DBAL_FIELD_FODO, DBAL_FIELD_VSI, DBAL_FIELD_VRF, DBAL_FIELD_VSI, 222}},

    /*
     * Steps 11-12 - Table with only Parent field (FODO) not in all result types and replace disallowed
     * set_action: SET_ACTION_USE_VALUE
     * unset_action: UNSET_ACTION_IGNORE
     * value_replace_valid: FALSE
     * field_existance_replace_valid: FALSE
     * Tested operations: 
     *   1. Change value for a result type with a Parent field (FODO) to a different result-type with no Parent or Child fields - Should FAIL
     *   2. Change value for a result type with no Parent or Child fields to a different result-type with a Parent field (FODO) - Should FAIL
     */
    {{DBAL_TABLE_IN_LIF_FORMAT_L2TP, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_FODO}, {1, 2},
      DBAL_RESULT_TYPE_IN_LIF_FORMAT_L2TP_IN_LIF_TUNNEL_W_L3_PAYLOAD_MP_FORWARD_W_GENERIC_DATA},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {1}, RESULT_TYPE_INVALID},
     {DBAL_FIELD_DESTINATION, DBAL_FIELD_FEC, DBAL_FIELD_PORT_ID, DBAL_FIELD_DESTINATION, 99}},

    {{DBAL_TABLE_IN_LIF_FORMAT_L2TP, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_DESTINATION}, {1, 203},
      DBAL_RESULT_TYPE_IN_LIF_FORMAT_L2TP_IN_LIF_TUNNEL_W_L3_PAYLOAD_P2P_FORWARD_TO_FEC_W_GENERIC_DATA},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {1}, RESULT_TYPE_INVALID},
     {DBAL_FIELD_FODO, DBAL_FIELD_VSI, DBAL_FIELD_VRF, DBAL_FIELD_FODO, 10}},

    /*
     * Steps 13-15 - Table with only Parent field (FODO) not in all result types, replace allowed, but only if the field was already set
     * set_action: SET_ACTION_USE_VALUE
     * unset_action: UNSET_ACTION_IGNORE
     * value_replace_valid: TRUE
     * field_existance_replace_valid: FALSE
     * Tested operations: 
     *   1. Change value for a result type with a Parent field (FODO) to a different value
     *   2. Change value for a result type with a Parent field (FODO) to a different result-type with no Parent or Child fields - Should FAIL
     *   3. Change value for a result type with no Parent or Child fields to a different result-type with a Parent field (FODO) - Should FAIL
     */
    {{DBAL_TABLE_IN_LIF_FORMAT_GTP, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_FODO}, {1, 2},
      DBAL_RESULT_TYPE_IN_LIF_FORMAT_GTP_IN_LIF_TUNNEL_W_L3_PAYLOAD_MP_FORWARD_W_GENERIC_DATA},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {1},
      DBAL_RESULT_TYPE_IN_LIF_FORMAT_GTP_IN_LIF_TUNNEL_W_L3_PAYLOAD_MP_FORWARD_W_GENERIC_DATA},
     {DBAL_FIELD_FODO, DBAL_FIELD_VSI, DBAL_FIELD_VRF, DBAL_FIELD_FODO, 10}},

    {{DBAL_TABLE_IN_LIF_FORMAT_GTP, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_FODO}, {1, 2},
      DBAL_RESULT_TYPE_IN_LIF_FORMAT_GTP_IN_LIF_TUNNEL_W_L3_PAYLOAD_MP_FORWARD_W_GENERIC_DATA},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {1}, RESULT_TYPE_INVALID},
     {DBAL_FIELD_DESTINATION, DBAL_FIELD_FEC, DBAL_FIELD_PORT_ID, DBAL_FIELD_DESTINATION, 99}},

    {{DBAL_TABLE_IN_LIF_FORMAT_GTP, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_DESTINATION}, {1, 203},
      DBAL_RESULT_TYPE_IN_LIF_FORMAT_GTP_IN_LIF_TUNNEL_W_L3_PAYLOAD_P2P_FORWARD_TO_FEC_W_GENERIC_DATA},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {1}, RESULT_TYPE_INVALID},
     {DBAL_FIELD_FODO, DBAL_FIELD_VSI, DBAL_FIELD_VRF, DBAL_FIELD_FODO, 10}},

    /*
     * Steps 16-18 - Table with only Parent field (DESTINATION) not in all result types, value not set to HW so not validated for replace
     * set_action: SET_ACTION_DO_NOT_USE_VALUE
     * unset_action: UNSET_ACTION_IGNORE
     * value_replace_valid: NONE
     * field_existance_replace_valid: FALSE
     * Tested operations: 
     *   1. Change value for a result type with a Parent field (DESTINATION) to a different value
     *   2. Change value for a result type with a Parent field (DESTINATION) to a different result-type with no Parent or Child fields - Should fail
     *   3. Change value for a result type with no Parent or Child fields to a different result-type with a Parent field (DESTINATION) - Should fail
     */
    {{DBAL_TABLE_IN_LIF_IPvX_TUNNELS, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_DESTINATION}, {100, 1000},
      DBAL_RESULT_TYPE_IN_LIF_IPvX_TUNNELS_IN_LIF_IPVX_1},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {100}, DBAL_RESULT_TYPE_IN_LIF_IPvX_TUNNELS_IN_LIF_IPVX_1},
     {DBAL_FIELD_DESTINATION, DBAL_FIELD_FEC, DBAL_FIELD_PORT_ID, DBAL_FIELD_DESTINATION, 1001}},

    {{DBAL_TABLE_IN_LIF_IPvX_TUNNELS, NO_SPECIAL_FLAGS},
     {2, {DBAL_FIELD_GLOB_IN_LIF, DBAL_FIELD_DESTINATION}, {100, 1000},
      DBAL_RESULT_TYPE_IN_LIF_IPvX_TUNNELS_IN_LIF_IPVX_1},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {100}, RESULT_TYPE_INVALID},
     {DBAL_FIELD_FODO, DBAL_FIELD_VRF, DBAL_FIELD_VSI, DBAL_FIELD_VRF, 25}},

    {{DBAL_TABLE_IN_LIF_IPvX_TUNNELS, NO_SPECIAL_FLAGS},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {100},
      DBAL_RESULT_TYPE_IN_LIF_IPvX_TUNNELS_IN_LIF_IPVX_3},
     {1, {DBAL_FIELD_GLOB_IN_LIF}, {100}, RESULT_TYPE_INVALID},
     {DBAL_FIELD_DESTINATION, DBAL_FIELD_FEC, DBAL_FIELD_PORT_ID, DBAL_FIELD_DESTINATION, 1001}},

    

};

/*
 * Test cases for the Out-LIF Replace test
 */
static lif_table_mngr_outlif_replace_test_cases_t out_lif_table_replace_case[] = {

    /*
     * Step 0 - Replace to a field that exists only on a different result type
     */
    {{DBAL_TABLE_EEDB_MPLS_TUNNEL, NO_SPECIAL_FLAGS}, LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_1,
     {1, {DBAL_FIELD_MPLS_LABEL},
      {11}, DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_1},
     {2, {DBAL_FIELD_MPLS_LABEL, DBAL_FIELD_MPLS_LABEL_2},
      {11, 1}, DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2}},

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
     * Step 3 - Replace a field with a used (ENCAP_QOS_MODEL) value
     * and with valid existence replace from an existing value to a non-existing value
     * by not supplying the replaced field and value
     * Expect the result type not to change
     */
    {{DBAL_TABLE_EEDB_ARP, DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_VLAN_TRANSLATION}, LIF_MNGR_OUTLIF_PHASE_AC,
     {3, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_VSI, DBAL_FIELD_ENCAP_QOS_MODEL},
      {1, 5, 2}, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC},
     {2, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_VSI},
      {1, 5}, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC}},

    /*
     * Step 4 - Replace fields with used (ENCAP_QOS_MODEL) value 
     * and with valid existence replace from a non-existing state to an existing value
     * Expect the result type not to change
     */
    {{DBAL_TABLE_EEDB_ARP, DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_VLAN_TRANSLATION}, LIF_MNGR_OUTLIF_PHASE_AC,
     {2, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_VSI},
      {1, 5}, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC},
     {3, {DBAL_FIELD_EGRESS_LAST_LAYER, DBAL_FIELD_VSI, DBAL_FIELD_ENCAP_QOS_MODEL},
      {1, 5, 2}, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC}},

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
 * Compares an original test LIF-Table entry that was set to HW
 * with a retrieved entry handle-ID values.
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] test_case_info - A pointer to a structure that
 *         holds the test case attribures - DB Table ID and
 *         special flags
 *  \param [in] expected_info - A pointer to a structure that
 *         holds the expected entry configuration - field IDs,
 *         field values, expected result type
 *  \param [in] compared_entry_handle_id - The LIF DBAL table
 *         handle ID that stores the retrieved entry.
 * \return DIRECT OUTPUT
 *   shr_error_e an error means a failed comparison
 * \see
 *   dnx_in_lif_table_set_fields_and_verify,
 *   dnx_out_lif_table_set_fields_and_verify
 */
shr_error_e
dnx_compare_lif_table_entry(
    int unit,
    lif_table_mngr_test_case_info_t * test_case_info,
    lif_table_mngr_result_type_set_t * expected_info,
    uint32 compared_entry_handle_id)
{
    uint32 num_of_fields, field_idx, ret_field_val, expected_value = 0;
    dbal_enum_value_field_lif_table_manager_set_action_e field_set_action;
    dbal_enum_value_field_lif_table_manager_unset_action_e field_not_set_action;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    num_of_fields = expected_info->num_of_fields;

    for (field_idx = 0; field_idx < num_of_fields; field_idx++)
    {
        /*
         * Retrieve the value of the validated field from the entry handle 
         * according to the superset attributes
         */
        int rv;
        dbal_fields_e field_id = expected_info->field_id[field_idx];
        rv = dbal_entry_handle_value_field_arr32_get(unit,
                                                     compared_entry_handle_id, field_id, INST_SINGLE, &ret_field_val);
        if (SHR_FAILURE(rv))
        {
            /*
             * Decide whether the field retrieval failure is as expected 
             * Differ betweem a NOT_FOUND error and various encoding issues
             */
            if (rv == _SHR_E_NOT_FOUND)
            {
                if (expected_info->field_val[field_idx] == VALUE_FIELD_NOT_FOUND)
                {
                    LOG_INFO_EX(BSL_LOG_MODULE, "Field %s Not-Found as expected%s%s%s\n",
                                dbal_field_to_string(unit, field_id), EMPTY, EMPTY, EMPTY);
                    continue;
                }
                else
                {
                    SHR_EXIT_WITH_LOG(_SHR_E_FAIL, "FAILURE: Field %s Not-Found%s%s\n",
                                      dbal_field_to_string(unit, field_id), EMPTY, EMPTY);
                }
            }
            else
            {
                if (expected_info->field_val[field_idx] == VALUE_FIELD_ENCODE_ERROR)
                {
                    LOG_INFO_EX(BSL_LOG_MODULE, "Field %s Not Encoded properly as expected%s%s%s\n",
                                dbal_field_to_string(unit, field_id), EMPTY, EMPTY, EMPTY);
                    continue;
                }
                else
                {
                    SHR_EXIT_WITH_LOG(_SHR_E_FAIL, "FAILURE: Field %s Not Encoded properly, rv - %d%s\n",
                                      dbal_field_to_string(unit, field_id), rv, EMPTY);
                }
            }
        }

        /*
         * Decide on the comparison value.
         * If the configured value wasn't used (LIF-Table-Mngr field attribute),
         * get the predefined default value
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_field_set_actions_get
                        (unit, test_case_info->dbal_table_id, field_id, &field_set_action, &field_not_set_action));
        if (field_set_action == SET_ACTION_USE_VALUE)
        {
            expected_value = expected_info->field_val[field_idx];
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                            (unit, test_case_info->dbal_table_id, field_id, FALSE,
                             expected_info->expected_result_type, INST_SINGLE,
                             DBAL_PREDEF_VAL_DEFAULT_VALUE, &expected_value));
        }

        /*
         * Compare the retrieved value with the configured value
         */
        if (ret_field_val != expected_value)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                              "FAILURE: Wrong value retrieved for field %s, expected - %d, got - %d\n",
                              dbal_field_to_string(unit, field_id), expected_value, ret_field_val);
        }
    }

    printf("Compared entry successful\n");
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
 *  \param [in] global_lif - A pointer to a Global LIF value
 *  \param [in] retrieve_info - A pointer to an optional
 *         retrieve info
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
    int *global_lif,
    lif_table_mngr_result_type_set_t * retrieve_info,
    int is_replace,
    int is_delete)
{
    uint32 entry_handle_id, get_entry_handle_id, sw_db_entry_handle_id;
    uint32 num_of_fields, field_idx;
    uint32 dbal_result_type;
    dbal_fields_e field_id;
    dbal_physical_tables_e phy_table_id;
    lif_table_mngr_inlif_info_t lif_info;
    dbal_tables_e dbal_table_id;
    int local_lif_id;
    int rv;
    lif_table_mngr_result_type_set_t *expected_info;
    dbal_entry_handle_t *entry_handle;
    lif_table_mngr_inlif_info_t lif_table_mngr_inlif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * If a retrieve_info supplied use it instead of the set info
     */
    expected_info = (retrieve_info) ? retrieve_info : result_type_set_info;

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
     * Verify that all the requested fields were set properly 
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    if (entry_handle->error_info.error_exists)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                          "FAILURE: Field %s wasn't set properly, error - %d%s\n",
                          dbal_field_to_string(unit, entry_handle->error_info.field_id),
                          entry_handle->error_info.error_exists, EMPTY);
    }

    /*
     * Get the physical table associated with this lif format. Verify that the physical table is EEDB.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, test_case_info->dbal_table_id,
                                                   DBAL_PHY_DB_DEFAULT_INDEX, &phy_table_id));

    if (!is_replace)
    {
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                        (unit, LIF_MNGR_L2_GPORT_GLOBAL_LIF, DNX_ALGO_LIF_INGRESS, global_lif));
    }

    /*
     * Call the LIF Table manager API to set the SW and the HW
     */
    sal_memset(&lif_info, 0, sizeof(lif_info));
    if (is_replace)
    {
        lif_mapping_local_lif_info_t local_lif_info;

        lif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;

        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_global_to_local_get(unit,
                                                                 DNX_ALGO_LIF_INGRESS, *global_lif, &local_lif_info));
        local_lif_id = local_lif_info.local_lif;
    }
    lif_info.global_lif = *global_lif;
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
               expected_info->expected_result_type);

        /*
         * Verify the selected result type meets the expected result type 
         */
        if (expected_info->expected_result_type == RESULT_TYPE_INVALID)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                              "FAILURE: Got result type %d allocated while no-result was expected %s%s\n",
                              dbal_result_type, EMPTY, EMPTY);
        }
        else if ((expected_info->expected_result_type == RESULT_TYPE_CHANGED_OR_INVALID) &&
                 (dbal_result_type == result_type_set_info->expected_result_type))
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                              "FAILURE: Got an unallowed result type %d%s%s\n", dbal_result_type, EMPTY, EMPTY);
        }
        else if ((expected_info->expected_result_type != RESULT_TYPE_CHANGED_OR_INVALID) &&
                 (dbal_result_type != result_type_set_info->expected_result_type))
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL, "FAILURE: Wrong result type, expected - %d, got - %d%s\n",
                              result_type_set_info->expected_result_type, dbal_result_type, EMPTY);
        }

        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, test_case_info->dbal_table_id, &get_entry_handle_id));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                        (unit, _SHR_CORE_ALL, local_lif_id, get_entry_handle_id, &lif_table_mngr_inlif_info));

        /*
         * Compare the retrieved entry with the orginally set entry
         */
        SHR_IF_ERR_EXIT(dnx_compare_lif_table_entry(unit, test_case_info, expected_info, get_entry_handle_id));

        /*
         * A function call that has neither delete nor replace indications,
         * is the first call in the sequence.
         * Update the lookup SW DB for tables that require this for module resolve
         * Only a result-type value is important
         */
        if (!is_replace && !is_delete)
        {
            if (test_case_info->dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW, &sw_db_entry_handle_id));
                dbal_entry_key_field32_set(unit, sw_db_entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_lif_id);
                dbal_entry_value_field32_set(unit, sw_db_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, sw_db_entry_handle_id, DBAL_COMMIT));
            }
        }
    }
    else
    {
        /*
         * In case the LIF-Table-Mngr API failed to select a Result type, 
         * confirm it was expected 
         */
        if ((expected_info->expected_result_type != RESULT_TYPE_INVALID) &&
            (expected_info->expected_result_type != RESULT_TYPE_CHANGED_OR_INVALID))
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                              "FAILURE: Failed to get any result type. expected result type - %d %s%s\n",
                              expected_info->expected_result_type, EMPTY, EMPTY);
        }
        printf("In-LIF Table manager set fields ended with no result type - as expected\n");
    }

    /*
     * Delete the In-LIF through the LIF Table manager and verify
     */
    if (is_delete)
    {
        if (SHR_SUCCESS(rv) || is_replace)
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_inlif_info_clear
                            (unit, _SHR_CORE_ALL, local_lif_id, test_case_info->dbal_table_id, 0));
            rv = dnx_lif_table_mngr_get_inlif_info(unit, _SHR_CORE_ALL, local_lif_id, get_entry_handle_id,
                                                   &lif_table_mngr_inlif_info);

            if (SHR_SUCCESS(rv))
            {
                /*
                 * Compare the cleared entry with the originally set entry.
                 * If equal, the clear operation failed
                 */
                rv = dnx_compare_lif_table_entry(unit, test_case_info, expected_info, get_entry_handle_id);
                if (SHR_SUCCESS(rv))
                {
                    SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                                      "FAILURE: dnx_lif_table_mngr_inlif_info_clear for lif_id #%d, didn't remove the entry %s%s\n",
                                      local_lif_id, EMPTY, EMPTY);
                }
            }
        }

        /*
         * A function call that has both delete and replace indications
         * is the last call in a replace sequence.
         * In this case, there's a need to clear any deleted resources that
         * are handled outside the LIF Table Manager APIs
         */
        if (is_replace)
        {
            if (test_case_info->dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW, &sw_db_entry_handle_id));
                dbal_entry_key_field32_set(unit, sw_db_entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_lif_id);
                dbal_entry_value_field32_set(unit, sw_db_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT);
                SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_db_entry_handle_id, DBAL_COMMIT));
            }
        }
    }

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
 *  \param [in] global_lif - A pointer to a Global LIF value
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
    int *global_lif,
    int is_replace,
    int is_delete)
{
    uint32 entry_handle_id, get_entry_handle_id;
    uint32 num_of_fields, field_idx;
    uint32 dbal_result_type;
    dbal_fields_e field_id;
    lif_table_mngr_outlif_info_t lif_info;
    dbal_tables_e dbal_table_id;
    int local_lif_id;
    int rv;
    int mpls_tunnel_etps_2_result_type_size;
    int mpls_tunnel_etps_2_result_type_idx = 4;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_payload_size_get
                    (unit, DBAL_TABLE_EEDB_MPLS_TUNNEL, mpls_tunnel_etps_2_result_type_idx,
                     &mpls_tunnel_etps_2_result_type_size));

    /*
     * Test is not supported for current device
     */
    if (mpls_tunnel_etps_2_result_type_size > 60)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        SHR_EXIT();
    }

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
     * Allocate the Global-LIF
     */
    if (!is_replace)
    {
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                        (unit, LIF_MNGR_L2_GPORT_GLOBAL_LIF, DNX_ALGO_LIF_EGRESS, global_lif));
    }

    /*
     * Call the LIF Table manager API to set the SW and the HW
     */
    sal_memset(&lif_info, 0, sizeof(lif_info));
    if (is_replace)
    {
        /*
         * In case of REPLACE, retrieve the Local-LIF from the Global-LIF
         */
        lif_mapping_local_lif_info_t local_lif_info;

        lif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;

        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_global_to_local_get(unit,
                                                                 DNX_ALGO_LIF_EGRESS, *global_lif, &local_lif_info));
        local_lif_id = local_lif_info.local_lif;
    }
    lif_info.global_lif = *global_lif;
    lif_info.table_specific_flags = test_case_info->table_specific_flags;
    lif_info.outlif_phase = outlif_phase;

    rv = dnx_lif_table_mngr_allocate_local_outlif_and_set_hw(unit, entry_handle_id, &local_lif_id, &lif_info);

    /*
     * Address the result type in case the LIF table manager allocation was successful
     */
    if (SHR_SUCCESS(rv))
    {
        lif_table_mngr_outlif_info_t lif_table_mngr_outlif_info;

        /*
         * Retrieve the allocated result type
         */
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get
                        (unit, local_lif_id, &dbal_table_id, &dbal_result_type, NULL, NULL, NULL, NULL));
        printf("dbal_result_type - %d, expected_result_type - %d\n", dbal_result_type,
               result_type_set_info->expected_result_type);

        /*
         * Verify the selected result type meets the expected result type 
         */
        if (result_type_set_info->expected_result_type == RESULT_TYPE_INVALID)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                              "FAILURE: Got result type %d allocated while no-result was expected %s%s\n",
                              dbal_result_type, EMPTY, EMPTY);
        }
        if ((result_type_set_info->expected_result_type == RESULT_TYPE_CHANGED_OR_INVALID) &&
            (dbal_result_type == result_type_set_info->expected_result_type))
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                              "FAILURE: Got an unallowed result type %d%s%s\n", dbal_result_type, EMPTY, EMPTY);
        }
        else if ((result_type_set_info->expected_result_type != RESULT_TYPE_CHANGED_OR_INVALID) &&
                 (dbal_result_type != result_type_set_info->expected_result_type))
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL, "FAILURE: Wrong result type, expected - %d, got - %d%s\n",
                              result_type_set_info->expected_result_type, dbal_result_type, EMPTY);
        }

        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, test_case_info->dbal_table_id, &get_entry_handle_id));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                        (unit, local_lif_id, get_entry_handle_id, &lif_table_mngr_outlif_info));

        /*
         * Compare the retrieved entry with the orginally set entry
         */
        SHR_IF_ERR_EXIT(dnx_compare_lif_table_entry(unit, test_case_info, result_type_set_info, get_entry_handle_id));

        /*
         * Delete the Out-LIF through the LIF Table manager and verify
         */
        if (is_delete)
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear(unit, local_lif_id, 0));
            rv = dnx_lif_table_mngr_get_outlif_info(unit, local_lif_id, get_entry_handle_id,
                                                    &lif_table_mngr_outlif_info);
            if (SHR_SUCCESS(rv))
            {
                /*
                 * Compare the cleared entry with the originally set entry.
                 * If equal, the clear operation failed
                 */
                rv = dnx_compare_lif_table_entry(unit, test_case_info, result_type_set_info, get_entry_handle_id);
                if (SHR_SUCCESS(rv))
                {
                    SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                                      "FAILURE: dnx_lif_table_mngr_outlif_info_clear for lif_id #%d, didn't remove the entry %s%s\n",
                                      local_lif_id, EMPTY, EMPTY);
                }
            }
        }
    }
    else
    {
        /*
         * In case the LIF-Table-Mngr API failed to select a Result type, 
         * confirm it was expected 
         */
        if ((result_type_set_info->expected_result_type != RESULT_TYPE_INVALID) &&
            (result_type_set_info->expected_result_type != RESULT_TYPE_CHANGED_OR_INVALID))
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL,
                              "FAILURE: Failed to get any result type. expected result type - %d %s%s\n",
                              result_type_set_info->expected_result_type, EMPTY, EMPTY);
        }
        printf("Out-LIF Table manager set fields eneded with no result type - as expected\n");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Performs a sequence of child field test validation for one 
 * specific Parent or Child value. After the entry set, attempt 
 * to retrieve the Parent/Child value in all possible retrieve 
 * options. 
 * The function is shared both for Unit and Replace Parent/Child 
 * tests. 
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] set_field_idx - The set operation index out of
 *         all supported Parent/Child field set options.
 *  \param [in] test_case_info - A Pointer to the case's static
 *         configuration per case.
 *  \param [in] entry_info - A Pointer to the entry's static
 *         configuration.
 *  \param [in] retrieve_info - A Pointer to the entry's
 *         expected field values after set
 *  \param [in] expected_result_type - A pointer to an Array of
 *         expected Result-Types per Set operation
 *  \param [in] is_field_in_superset - A pointer to an Array of
 *         indication per Set operation, whether the field is
 *         defined in the superset.
 *  \param [in] field_id - A pointer to an Array of Parent/Child
 *         field IDs per Set operation.
 *  \param [in] field_val - A pointer to an Array of
 *         Parent/Child field values per Set operation.
 *  \param [in] is_field_support_by_lif_table_mngr - A pointer
 *         to an Array of indications per Get operation, whether
 *         the field is supported by the LIF-Table-Mngr
 * \param [in] step_rv - A pointer to an Array of validation 
 *        results per Set/Get test step
 * \param [in] global_lif - A pointer to the Global-LIF value 
 * \param [in] is_replace - Indication whether it's a Replace 
 *        operation.
 * \param [in] is_delete - Indication whether there's a need to 
 *        delete the LIF configuration.
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   dnx_in_lif_child_field_verify, 
 */
shr_error_e
dnx_in_lif_child_field_single_verify(
    int unit,
    int set_field_idx,
    lif_table_mngr_test_case_info_t * test_case_info,
    lif_table_mngr_result_type_set_t * entry_info,
    lif_table_mngr_result_type_set_t * retrieve_info,
    int *expected_result_type,
    int *is_field_in_superset,
    dbal_fields_e * field_id,
    uint32 *field_val,
    int *is_field_support_by_lif_table_mngr,
    shr_error_e * step_rv,
    int *global_lif,
    int is_replace,
    int is_delete)
{
    uint32 step_idx, get_field_idx;
    uint32 last_field_idx = entry_info->num_of_fields - 1;
    int step_delete, skipped_step_delete = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (is_field_in_superset[set_field_idx])
    {
        /*
         * Modify the last field in the array for validation function 
         * according to the SET operation 
         */
        entry_info->field_id[last_field_idx] = field_id[set_field_idx];
        entry_info->field_val[last_field_idx] = field_val[set_field_idx];
        retrieve_info->expected_result_type = expected_result_type[set_field_idx];

        /*
         * Validate each field retrieval for each of the SET operations
         */
        for (get_field_idx = 0; get_field_idx < CHILD_TEST_NOF_GET_STEPS; get_field_idx++)
        {
            printf("\nStart step SET-GET [%s-%s]\n\n",
                   dbal_field_to_string(unit, field_id[set_field_idx]),
                   dbal_field_to_string(unit, field_id[get_field_idx]));

            /*
             * Determine when to delete the LIF: 
             * In a Replace scenario - Only at the last Get call option (all options perform a SET replace). 
             * In a Unit scenario - Delete after each set
             */
            if (is_replace)
            {
                step_delete = (get_field_idx == (CHILD_TEST_NOF_GET_STEPS - 1)) ? is_delete : FALSE;
            }
            else
            {
                step_delete = is_delete;
            }

            if (is_field_support_by_lif_table_mngr[get_field_idx])
            {
                /*
                 * Modify the last field in the retrival arrays for 
                 * validation function according to the field retrieval 
                 */
                retrieve_info->field_id[last_field_idx] = field_id[get_field_idx];
                retrieve_info->field_val[last_field_idx] = field_val[get_field_idx];

                /*
                 * Adjust the expected retrieval value in a few cases: 
                 * 1. If the retrieved field or its parent aren't defined in the superset 
                 * 2. If the subfield value differs between the SET and the GET 
                 * 3. If the SET field & value are of the other child In case it's a 
                 *    parent retrieval, expect to retrieve the Parent with the other subfield value
                 */
                if (!is_field_in_superset[get_field_idx])
                {
                    retrieve_info->field_val[last_field_idx] = VALUE_FIELD_NOT_FOUND;
                }
                else if ((((set_field_idx == ST_PARENT) || (set_field_idx == ST_CHILD))
                          && (get_field_idx == ST_OTHER_CHILD)) ||
                         (((set_field_idx == ST_OTHER_CHILD) || (set_field_idx == ST_OTHER_PARENT))
                          && (get_field_idx == ST_CHILD)))
                {
                    retrieve_info->field_val[last_field_idx] = VALUE_FIELD_ENCODE_ERROR;
                }
                else if (((set_field_idx == ST_OTHER_CHILD) || (set_field_idx == ST_OTHER_PARENT))
                         && (get_field_idx == ST_PARENT))
                {
                    retrieve_info->field_val[last_field_idx] = field_val[ST_OTHER_PARENT];
                }

                /*
                 * Call the main validation function and record the return value
                 */
                step_idx = set_field_idx * CHILD_TEST_NOF_GET_STEPS + get_field_idx;
                step_rv[step_idx] = dnx_in_lif_table_set_fields_and_verify(unit,
                                                                           test_case_info, entry_info, global_lif,
                                                                           retrieve_info, is_replace, step_delete);
                if (step_rv[step_idx])
                {
                    return step_rv[step_idx];
                }
            }
            else
            {
                printf("Skip a step that retrieves field %s (Field unsupprted by the LIF-Table-Mngr)\n",
                       dbal_field_to_string(unit, field_id[get_field_idx]));
                if (step_delete && is_replace)
                {
                    skipped_step_delete = TRUE;
                }
            }
        }
    }
    else
    {
        printf("Skip a step that sets field %s (Field undefined in the superset result)\n",
               dbal_field_to_string(unit, field_id[set_field_idx]));
        if (is_replace)
        {
            skipped_step_delete = TRUE;
        }
    }

    /*
     * In case the configuration for the step was skipped and there's a prior configured entry, 
     * delete this entry. 
     */

    if (skipped_step_delete)
    {
        lif_mapping_local_lif_info_t local_lif_info;

        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_global_to_local_get(unit,
                                                                 DNX_ALGO_LIF_INGRESS, *global_lif, &local_lif_info));

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_inlif_info_clear(unit,
                                                            _SHR_CORE_ALL, local_lif_info.local_lif,
                                                            test_case_info->dbal_table_id, 0));

        if (test_case_info->dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB)
        {
            uint32 sw_db_entry_handle_id;

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW, &sw_db_entry_handle_id));
            dbal_entry_key_field32_set(unit, sw_db_entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_lif_info.local_lif);
            dbal_entry_value_field32_set(unit, sw_db_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT);
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_db_entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Performs a sequence of child field test validations by 
 * permuting possible Parent/Child field IDs and values. 
 * The function is shared both for Unit and Replace Parent/Child
 * tests, but in Replace mode performs only a single SET 
 * operation, so that each Replace is done relatively to the 
 * original entry. 
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] test_case_info - A Pointer to the case's static
 *         configuration per case.
 *  \param [in] result_type_set_info - A Pointer to the entry's
 *         static configuration.
 *  \param [in] child_field_info - A Pointer to the entry's
 *         static configuration per case of the Parent/Child
 *         fields.
 * \param [in] global_lif - A pointer to the Global-LIF value 
 * \param [in] is_replace - Indication whether it's a Replace 
 *        operation.
 * \param [in] is_delete - Indication whether there's a need to 
 *        delete the LIF configuration.
 * \param [in] test_case - The test case index for logging 
 * \param [in] set_field_step - The Set permutation to perform. 
 *        Applicable only in REPLACE mode.
 *  
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   dnx_in_lif_child_field_test
 *   dnx_in_lif_child_field_replace_test
 */
shr_error_e
dnx_in_lif_child_field_verify(
    int unit,
    lif_table_mngr_test_case_info_t * test_case_info,
    lif_table_mngr_result_type_set_t * result_type_set_info,
    lif_table_mngr_child_field_conf_t * child_field_info,
    int *global_lif,
    int is_replace,
    int is_delete,
    int test_case,
    int set_field_step)
{
    dbal_fields_e field_id[CHILD_TEST_NOF_SET_STEPS];
    uint32 field_val[CHILD_TEST_NOF_SET_STEPS];
    int is_field_in_superset[CHILD_TEST_NOF_SET_STEPS];
    int expected_result_type[CHILD_TEST_NOF_SET_STEPS];
    int is_field_support_by_lif_table_mngr[CHILD_TEST_NOF_GET_STEPS];
    uint32 set_field_idx, get_field_idx;
    uint32 step_idx, first_step_idx, last_step_idx;
    int superset_result_type;
    lif_table_mngr_result_type_set_t *entry_info = result_type_set_info;
    lif_table_mngr_result_type_set_t retrieve_info;
    shr_error_e rv, step_rv[CHILD_TEST_NOF_SET_STEPS * CHILD_TEST_NOF_GET_STEPS];
    int is_parent_in_result_type;
    int rv_err_count;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Initialize the return value per step to an invalid value
     */
    memset(step_rv, RETURN_VALUE_NOT_INITIALIZED, sizeof(step_rv));

    /*
     * Set the field_id and value for each of the Set iterations. 
     * Encode the Parent field values. 
     */
    field_id[ST_PARENT] = child_field_info->parent_field_id;
    field_id[ST_CHILD] = child_field_info->child_field_id;
    field_id[ST_OTHER_CHILD] = child_field_info->other_child_field_id;
    field_id[ST_OTHER_PARENT] = child_field_info->parent_field_id;

    field_val[ST_CHILD] = child_field_info->child_field_val;
    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set(unit, field_id[ST_PARENT],
                                                         field_id[ST_CHILD], &field_val[ST_CHILD],
                                                         &field_val[ST_PARENT]));
    field_val[ST_OTHER_CHILD] = field_val[ST_CHILD];
    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set(unit, field_id[ST_OTHER_PARENT],
                                                         field_id[ST_OTHER_CHILD], &field_val[ST_OTHER_CHILD],
                                                         &field_val[ST_OTHER_PARENT]));

    /*
     * Check whether the Parent and Child field are supported by the LIF-Table-Mngr as
     * standalone fields. 
     * Those that aren't, can't be retrieved using LIF-Table-Mngr APIs
     */
    for (get_field_idx = 0; get_field_idx < CHILD_TEST_NOF_GET_STEPS; get_field_idx++)
    {
        dbal_enum_value_field_lif_table_manager_set_action_e field_set_action;
        dbal_enum_value_field_lif_table_manager_unset_action_e field_not_set_action;

        rv = dnx_lif_table_mngr_field_set_actions_get(unit,
                                                      test_case_info->dbal_table_id, field_id[get_field_idx],
                                                      &field_set_action, &field_not_set_action);
        is_field_support_by_lif_table_mngr[get_field_idx] = SHR_SUCCESS(rv) ? TRUE : FALSE;
    }

    /*
     * Validate the user supplied parent/child field_id in the expected result type
     */
    if (child_field_info->field_id_in_result_type == field_id[ST_PARENT])
    {
        is_parent_in_result_type = TRUE;
    }
    else if (child_field_info->field_id_in_result_type == field_id[ST_CHILD])
    {
        is_parent_in_result_type = FALSE;
    }
    else
    {
        SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Given Field is not the parent field nor the child field%s%s%s\n",
                          EMPTY, EMPTY, EMPTY);
    }

    /*
     * Initialize the entry retrieve information with entry configuration information
     */
    sal_memcpy(&retrieve_info, entry_info, sizeof(retrieve_info));

    /*
     * Check for each of the optional set fields whether the field or the 
     * Parent are defined in the superset. 
     * Otherwise, the set operation will fail prior to the LIF-Table-Mngr API call. 
     */
    SHR_IF_ERR_EXIT(dbal_tables_table_nof_res_type_get(unit, test_case_info->dbal_table_id, &superset_result_type));
    for (set_field_idx = 0; set_field_idx < CHILD_TEST_NOF_SET_STEPS; set_field_idx++)
    {
        dbal_table_field_info_t table_field_info;

        rv = dbal_tables_field_info_get_no_err(unit, test_case_info->dbal_table_id, field_id[set_field_idx], FALSE,
                                               superset_result_type, 0, &table_field_info);
        is_field_in_superset[set_field_idx] = SHR_SUCCESS(rv) ? TRUE : FALSE;
    }

    /*
     * Determine the expected result type per SET operation
     * For SET operations that use the other child's field and value, it's
     * assumed that in case the expected result type doesn't contain the 
     * parent field - a different result type will be selected, if any is possible. 
     */
    expected_result_type[ST_PARENT] = entry_info->expected_result_type;
    expected_result_type[ST_CHILD] = entry_info->expected_result_type;
    expected_result_type[ST_OTHER_CHILD] = (is_parent_in_result_type) ? entry_info->expected_result_type :
        (is_field_in_superset[ST_PARENT]) ? RESULT_TYPE_CHANGED_OR_INVALID : RESULT_TYPE_INVALID;
    expected_result_type[ST_OTHER_PARENT] = expected_result_type[ST_OTHER_CHILD];

    /*
     * Perform the Set test validation 
     * In REPLACE mode - Call for one of the Parent/Child Set permutations. 
     * In Unit test mode - Perform all permutations.
     */
    if (is_replace)
    {
        SHR_IF_ERR_EXIT(dnx_in_lif_child_field_single_verify(unit,
                                                             set_field_step, test_case_info, entry_info, &retrieve_info,
                                                             expected_result_type, is_field_in_superset, field_id,
                                                             field_val, is_field_support_by_lif_table_mngr, step_rv,
                                                             global_lif, is_replace, is_delete));
        first_step_idx = set_field_step * CHILD_TEST_NOF_GET_STEPS;
        last_step_idx = first_step_idx + CHILD_TEST_NOF_GET_STEPS - 1;
    }
    else
    {
        /*
         * Traverse the SET operations and validate each separately
         */
        for (set_field_idx = 0; set_field_idx < CHILD_TEST_NOF_SET_STEPS; set_field_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_in_lif_child_field_single_verify(unit,
                                                                 set_field_idx, test_case_info, entry_info,
                                                                 &retrieve_info, expected_result_type,
                                                                 is_field_in_superset, field_id, field_val,
                                                                 is_field_support_by_lif_table_mngr, step_rv,
                                                                 global_lif, is_replace, is_delete));
            first_step_idx = 0;
            last_step_idx = sizeof(step_rv) / sizeof(step_rv[0]) - 1;
        }
    }

    /*
     * Report the failure status for the test case
     */
    rv_err_count = 0;
    printf("\nTest case #%d result:\n", test_case);
    for (step_idx = first_step_idx; step_idx <= last_step_idx; step_idx++)
    {
        if (step_rv[step_idx] == RETURN_VALUE_NOT_INITIALIZED)
        {
            printf("STEP [%d] SKIPPED\n", step_idx + 1);
        }
        else if (step_rv[step_idx])
        {
            printf("STEP [%d] FAILED (Set - %s, Get - %s), rv - %d\n", step_idx + 1,
                   dbal_field_to_string(unit, field_id[step_idx / CHILD_TEST_NOF_GET_STEPS]),
                   dbal_field_to_string(unit, field_id[step_idx % CHILD_TEST_NOF_GET_STEPS]), step_rv[step_idx]);
            rv_err_count++;
        }
        else
        {
            printf("STEP [%d] PASSED\n", step_idx + 1);
        }
    }
    if (rv_err_count)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "\nIn-LIF parent-child Test case #%d FAILED!\n\n", test_case);
    }

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
    int global_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    printf("\nIn-LIF Table manager unit TEST Started, num_of_test_cases = %d\n\n", num_of_test_cases);

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
                                                               &(in_lif_table_case[test_case].result_type_set),
                                                               &global_lif, NULL, FALSE, TRUE));

        printf("In-LIF Test case #%d PASSED!\n", test_case);
    }

    printf("\nIn-LIF Table manager unit TEST PASSED!\n\n");

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * runs a test of parent-child field handling for the In-Lif 
 * side of the LIF Table manager. 
 * For each test case, configure the relevant Parent/Child field
 * using a few field_ids & values: Parent field, Child field, 
 * Another child field and as a Parent with other child value 
 * encoded. 
 * For each of the above field set itterations, perform retrieve 
 * operations that vary by field_id: Parent field, Child field &
 * the other child field.
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
dnx_in_lif_child_field_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 test_case;
    uint32 num_of_test_cases = sizeof(inlif_child_field_test_case) / sizeof(inlif_child_field_test_case[0]);
    int global_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    printf("\nIn-LIF Table manager parent-child TEST Started, num_of_test_cases = %d\n\n", num_of_test_cases);

    /*
     * Traverse all the predefined test cases
     */
    for (test_case = 0; test_case < num_of_test_cases; test_case++)
    {
        /*
         * Set the entry's setting via the LIF Table manager API,
         * including result type and field values verification
         */
        inlif_child_field_test_case[test_case].result_type_set.num_of_fields++;
        SHR_IF_ERR_EXIT(dnx_in_lif_child_field_verify(unit,
                                                      &(inlif_child_field_test_case[test_case].test_case_info),
                                                      &(inlif_child_field_test_case[test_case].result_type_set),
                                                      &(inlif_child_field_test_case[test_case].child_field_conf),
                                                      &global_lif, FALSE, TRUE, test_case, 0));
    }

    printf("\nIn-LIF Table manager parent-child FULL TEST PASSED!\n\n");

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
    int global_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    printf("\nOut-LIF Table manager unit TEST Started, num_of_test_cases = %d\n\n", num_of_test_cases);

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
                                                                out_lif_table_case[test_case].outlif_phase,
                                                                &global_lif, FALSE, TRUE));

        printf("Out-LIF Test case #%d PASSED!\n", test_case);
    }

    printf("\nOut-LIF Table manager unit TEST PASSED!\n\n");

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
    int global_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    printf("\nIn-LIF Table manager Replace TEST Started, num_of_test_cases = %d\n\n", num_of_test_cases);

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
                                                               &global_lif, NULL, FALSE, FALSE));

        /*
         * Set the entry's replace setting via the LIF Table manager API,
         * Including result type and field values verification
         */
        SHR_IF_ERR_EXIT(dnx_in_lif_table_set_fields_and_verify(unit,
                                                               &(in_lif_table_replace_case[test_case].test_case_info),
                                                               &(in_lif_table_replace_case[test_case].replace_set),
                                                               &global_lif, NULL, TRUE, TRUE));

        printf("In-LIF REPLACE Test case #%d PASSED!\n", test_case);
    }

    printf("\nIn-LIF Table manager Replace TEST PASSED!\n\n");

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
dnx_in_lif_child_field_replace_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 test_case, set_field_idx;
    uint32 num_of_test_cases = sizeof(in_lif_child_field_replace_case) / sizeof(in_lif_child_field_replace_case[0]);
    int global_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    printf("\nIn-LIF Table manager parent-child Replace TEST Started, num_of_test_cases = %d\n\n", num_of_test_cases);

    /*
     * Traverse all the predefined test cases
     */
    for (test_case = 0; test_case < num_of_test_cases; test_case++)
    {
        printf("\nIn-LIF Table manager parent-child Replace TEST #%d STARTED\n\n", test_case);
        in_lif_child_field_replace_case[test_case].replace_set.num_of_fields++;

        for (set_field_idx = 0; set_field_idx < CHILD_TEST_NOF_SET_STEPS; set_field_idx++)
        {
            /*
             * Set the entry's initial setting via the LIF Table manager API,
             * Including result type and field values verification
             */
            SHR_IF_ERR_EXIT(dnx_in_lif_table_set_fields_and_verify(unit,
                                                                   &(in_lif_child_field_replace_case
                                                                     [test_case].test_case_info),
                                                                   &(in_lif_child_field_replace_case
                                                                     [test_case].initial_set), &global_lif, NULL, FALSE,
                                                                   FALSE));

            /*
             * Set the entry's setting via the LIF Table manager API,
             * including result type and field values verification
             */
            SHR_IF_ERR_EXIT(dnx_in_lif_child_field_verify(unit,
                                                          &(in_lif_child_field_replace_case[test_case].test_case_info),
                                                          &(in_lif_child_field_replace_case[test_case].replace_set),
                                                          &(in_lif_child_field_replace_case
                                                            [test_case].child_field_replace_set), &global_lif, TRUE,
                                                          TRUE, test_case, set_field_idx));
        }

        printf("In-LIF parent-child Replace Test case #%d PASSED!\n", test_case);
    }

    printf("\nIn-LIF Table manager parent-child Replace FULL TEST PASSED!\n\n");

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
    int global_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    printf("\nOut-LIF Table manager Replace TEST Started, num_of_test_cases = %d\n\n", num_of_test_cases);

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
                                                                &global_lif, FALSE, FALSE));

        /*
         * Set the entry's replace setting via the LIF Table manager API,
         * Including result type and field values verification
         */
        SHR_IF_ERR_EXIT(dnx_out_lif_table_set_fields_and_verify(unit,
                                                                &(out_lif_table_replace_case[test_case].test_case_info),
                                                                &(out_lif_table_replace_case[test_case].replace_set),
                                                                out_lif_table_replace_case[test_case].outlif_phase,
                                                                &global_lif, TRUE, TRUE));

        printf("Out-LIF REPLACE Test case #%d PASSED!\n", test_case);
    }

    printf("\nOut-LIF Table manager Replace TEST PASSED!\n\n");

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

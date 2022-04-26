/**
 * $Id: ctest_dnx_field_ffc.c,v 1.20 Broadcom SDK $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * \file ctest_dnx_field_ffc.c
 *
 * FFC Algorithm testing.
 *
 */
/*
 * $Copyright:.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /**
  * Include files.
  * {
  */
#include <soc/dnxc/swstate/sw_state_defs.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_init.h>
#include "ctest_dnx_field_ffc.h"
#include "ctest_dnx_field_utils.h"
#include <appl/diag/dnx/diag_dnx_field.h>

/*
 * }
 */

/* *INDENT-OFF* */

/**
 * \brief
 *   Options list for 'FFC' shell command
 * \remark
 */
sh_sand_option_t Sh_dnx_field_ffc_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'FFC' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_ffc_tests[] = {
    {"FFC_algo", "", CTEST_POSTCOMMIT},
    {NULL}
};

/**
 *  FFC shell command leaf details
 */
sh_sand_man_t Sh_dnx_field_ffc_man = {
    .brief =
        "Field FFC Algorithm tests",
    .full =
        "This Ctest presents different positive and negative scenarios.\n"
        "Creating contexts and field groups with dummy action and\n"
        "different User Define qualifiers to show FFC usage for all different scenarios.\n",
    .synopsis =
        ""
};

/** Context Selection info per field stage (IPMF1-2-3 and EPMF). */
static ctest_dnx_field_ffc_cs_info_t Ctest_dnx_field_ffc_cs_info_array[1] = {
/** context_id  | context_name | presel_id */
    {0, "ffc_algo_cs", 50}
};

/**
 * Following arrays are global, used for all test cases. They contain information for all qualifiers needed for
 * the different test cases (positive and negative).
 */
static ctest_dnx_field_ffc_qual_info_t Ctest_dnx_field_ffc_fg1_common_qual_info_array[CTEST_DNX_FIELD_FFC_COMMON_NOF_QUALS] = {
    {"ffc_algo_fg1_1", 0, 1, {0}}, {"ffc_algo_fg1_2", 0, 1, {0}}, {"ffc_algo_fg1_3", 0, 1, {0}}, {"ffc_algo_fg1_4", 0, 1, {0}}, {"ffc_algo_fg1_5", 0, 1, {0}},
    {"ffc_algo_fg1_6", 0, 1, {0}}, {"ffc_algo_fg1_7", 0, 1, {0}}, {"ffc_algo_fg1_8", 0, 1, {0}}, {"ffc_algo_fg1_9", 0, 1, {0}}, {"ffc_algo_fg1_10", 0, 1, {0}},
    {"ffc_algo_fg1_11", 0, 1, {0}}, {"ffc_algo_fg1_12", 0, 1, {0}}, {"ffc_algo_fg1_13", 0, 1, {0}}, {"ffc_algo_fg1_14", 0, 1, {0}}, {"ffc_algo_fg1_15", 0, 1, {0}},
    {"ffc_algo_fg1_16", 0, 1, {0}}, {"ffc_algo_fg1_17", 0, 1, {0}}, {"ffc_algo_fg1_18", 0, 1, {0}}, {"ffc_algo_fg1_19", 0, 1, {0}}, {"ffc_algo_fg1_20", 0, 1, {0}},
    {"ffc_algo_fg1_21", 0, 1, {0}}, {"ffc_algo_fg1_22", 0, 1, {0}}, {"ffc_algo_fg1_23", 0, 1, {0}}, {"ffc_algo_fg1_24", 0, 1, {0}}, {"ffc_algo_fg1_25", 0, 1, {0}},
    {"ffc_algo_fg1_26", 0, 1, {0}}, {"ffc_algo_fg1_27", 0, 1, {0}},{"ffc_algo_fg1_28", 0, 1, {0}}, {"ffc_algo_fg1_29", 0, 1, {0}}, {"ffc_algo_fg1_30", 0, 1, {0}},
    {"ffc_algo_fg1_31", 0, 1, {0}}, {"ffc_algo_fg1_32", 0, 1, {0}}
};
static ctest_dnx_field_ffc_qual_info_t Ctest_dnx_field_ffc_fg2_common_qual_info_array[CTEST_DNX_FIELD_FFC_COMMON_NOF_QUALS] = {
    {"ffc_algo_fg2_1", 0, 1, {0}}, {"ffc_algo_fg2_2", 0, 1, {0}}, {"ffc_algo_fg2_3", 0, 1, {0}}, {"ffc_algo_fg2_4", 0, 1, {0}}, {"ffc_algo_fg2_5", 0, 1, {0}},
    {"ffc_algo_fg2_6", 0, 1, {0}}, {"ffc_algo_fg2_7", 0, 1, {0}}, {"ffc_algo_fg2_8", 0, 1, {0}}, {"ffc_algo_fg2_9", 0, 1, {0}}, {"ffc_algo_fg2_10", 0, 1, {0}},
    {"ffc_algo_fg2_11", 0, 1, {0}}, {"ffc_algo_fg2_12", 0, 1, {0}}, {"ffc_algo_fg2_13", 0, 1, {0}}, {"ffc_algo_fg2_14", 0, 1, {0}}, {"ffc_algo_fg2_15", 0, 1, {0}},
    {"ffc_algo_fg2_16", 0, 1, {0}}, {"ffc_algo_fg2_17", 0, 1, {0}}, {"ffc_algo_fg2_18", 0, 1, {0}}, {"ffc_algo_fg2_19", 0, 1, {0}}, {"ffc_algo_fg2_20", 0, 1, {0}},
    {"ffc_algo_fg2_21", 0, 1, {0}}, {"ffc_algo_fg2_22", 0, 1, {0}}, {"ffc_algo_fg2_23", 0, 1, {0}}, {"ffc_algo_fg2_24", 0, 1, {0}}, {"ffc_algo_fg2_25", 0, 1, {0}},
    {"ffc_algo_fg2_26", 0, 1, {0}}, {"ffc_algo_fg2_27", 0, 1, {0}},{"ffc_algo_fg2_28", 0, 1, {0}}, {"ffc_algo_fg2_29", 0, 1, {0}}, {"ffc_algo_fg2_30", 0, 1, {0}},
    {"ffc_algo_fg2_31", 0, 1, {0}}, {"ffc_algo_fg2_32", 0, 1, {0}}
};
static ctest_dnx_field_ffc_qual_info_t Ctest_dnx_field_ffc_fg1_header_qual_info_array[CTEST_DNX_FIELD_FFC_HEADER_QUAL_NOF_QUALS] = {
    {"ffc_algo_fg1_1", 0, 1, {0}}, {"ffc_algo_fg1_2", 0, 1, {1}}, {"ffc_algo_fg1_3", 0, 1, {2}}, {"ffc_algo_fg1_4", 0, 1, {3}}, {"ffc_algo_fg1_5", 0, 1, {4}},
    {"ffc_algo_fg1_6", 0, 1, {5}}, {"ffc_algo_fg1_7", 0, 1, {6}}, {"ffc_algo_fg1_8", 0, 1, {7}}, {"ffc_algo_fg1_9", 0, 1, {8}}, {"ffc_algo_fg1_10", 0, 1, {9}},
    {"ffc_algo_fg1_11", 0, 1, {10}}, {"ffc_algo_fg1_12", 0, 1, {11}}, {"ffc_algo_fg1_13", 0, 1, {12}}, {"ffc_algo_fg1_14", 0, 1, {13}}, {"ffc_algo_fg1_15", 0, 1, {14}},
    {"ffc_algo_fg1_16", 0, 1, {15}}
};
static ctest_dnx_field_ffc_qual_info_t Ctest_dnx_field_ffc_fg2_header_qual_info_array[CTEST_DNX_FIELD_FFC_HEADER_QUAL_NOF_QUALS] = {
    {"ffc_algo_fg2_1", 0, 1, {32}}, {"ffc_algo_fg2_2", 0, 1, {33}}, {"ffc_algo_fg2_3", 0, 1, {34}}, {"ffc_algo_fg2_4", 0, 1, {35}}, {"ffc_algo_fg2_5", 0, 1, {36}},
    {"ffc_algo_fg2_6", 0, 1, {37}}, {"ffc_algo_fg2_7", 0, 1, {38}}, {"ffc_algo_fg2_8", 0, 1, {39}}, {"ffc_algo_fg2_9", 0, 1, {40}}, {"ffc_algo_fg2_10", 0, 1, {41}},
    {"ffc_algo_fg2_11", 0, 1, {42}}, {"ffc_algo_fg2_12", 0, 1, {43}}, {"ffc_algo_fg2_13", 0, 1, {44}}, {"ffc_algo_fg2_14", 0, 1, {45}}, {"ffc_algo_fg2_15", 0, 1, {46}},
    {"ffc_algo_fg2_16", 0, 1, {47}}
};
static ctest_dnx_field_ffc_qual_info_t Ctest_dnx_field_ffc_fg3_header_qual_info_array[CTEST_DNX_FIELD_FFC_HEADER_QUAL_NOF_QUALS] = {
    {"ffc_algo_fg3_1", 0, 1, {48}}, {"ffc_algo_fg3_2", 0, 1, {49}}, {"ffc_algo_fg3_3", 0, 1, {50}}, {"ffc_algo_fg3_4", 0, 1, {51}}, {"ffc_algo_fg3_5", 0, 1, {52}},
    {"ffc_algo_fg3_6", 0, 1, {53}}, {"ffc_algo_fg3_7", 0, 1, {54}}, {"ffc_algo_fg3_8", 0, 1, {55}}, {"ffc_algo_fg3_9", 0, 1, {56}}, {"ffc_algo_fg3_10", 0, 1, {57}},
    {"ffc_algo_fg3_11", 0, 1, {58}}, {"ffc_algo_fg3_12", 0, 1, {59}}, {"ffc_algo_fg3_13", 0, 1, {60}}, {"ffc_algo_fg3_14", 0, 1, {61}}, {"ffc_algo_fg3_15", 0, 1, {62}},
    {"ffc_algo_fg3_16", 0, 1, {63}}
};
static ctest_dnx_field_ffc_qual_info_t Ctest_dnx_field_ffc_fg1_layer_record_qual_info_array[CTEST_DNX_FIELD_FFC_LAYER_RECORD_QUAL_NOF_QUALS] = {
    {"ffc_algo_fg1_1", 0, 1, {0}}, {"ffc_algo_fg1_2", 0, 1, {1}}, {"ffc_algo_fg1_3", 0, 1, {2}}, {"ffc_algo_fg1_4", 0, 1, {3}}, {"ffc_algo_fg1_5", 0, 1, {4}},
    {"ffc_algo_fg1_6", 0, 1, {5}}, {"ffc_algo_fg1_7", 0, 1, {6}}, {"ffc_algo_fg1_8", 0, 1, {7}}, {"ffc_algo_fg1_9", 0, 1, {8}}, {"ffc_algo_fg1_10", 0, 1, {9}},
    {"ffc_algo_fg1_11", 0, 1, {10}}, {"ffc_algo_fg1_12", 0, 1, {11}}, {"ffc_algo_fg1_13", 0, 1, {12}}, {"ffc_algo_fg1_14", 0, 1, {13}}, {"ffc_algo_fg1_15", 0, 1, {14}},
    {"ffc_algo_fg1_16", 0, 1, {15}}
};
static ctest_dnx_field_ffc_qual_info_t Ctest_dnx_field_ffc_fg2_layer_record_qual_info_array[CTEST_DNX_FIELD_FFC_LAYER_RECORD_QUAL_NOF_QUALS] = {
    {"ffc_algo_fg2_1", 0, 1, {16}}, {"ffc_algo_fg2_2", 0, 1, {17}}, {"ffc_algo_fg2_3", 0, 1, {18}}, {"ffc_algo_fg2_4", 0, 1, {19}}, {"ffc_algo_fg2_5", 0, 1, {20}},
    {"ffc_algo_fg2_6", 0, 1, {21}}, {"ffc_algo_fg2_7", 0, 1, {22}}, {"ffc_algo_fg2_8", 0, 1, {23}}, {"ffc_algo_fg2_9", 0, 1, {24}}, {"ffc_algo_fg2_10", 0, 1, {25}},
    {"ffc_algo_fg2_11", 0, 1, {26}}, {"ffc_algo_fg2_12", 0, 1, {27}}, {"ffc_algo_fg2_13", 0, 1, {28}}, {"ffc_algo_fg2_14", 0, 1, {29}}, {"ffc_algo_fg2_15", 0, 1, {30}},
    {"ffc_algo_fg2_16", 0, 1, {31}}
};
static ctest_dnx_field_ffc_qual_info_t Ctest_dnx_field_ffc_fg1_diff_sizes_qual_info_array[CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_QUALS] = {
    {"ffc_algo_fg1_1", 0, 144, {0, 1, 2, 3, 4}}
};
static ctest_dnx_field_ffc_qual_info_t Ctest_dnx_field_ffc_fg2_diff_sizes_qual_info_array[CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_QUALS] = {
    {"ffc_algo_fg2_1", 0, 48, {5, 6}}
};
static ctest_dnx_field_ffc_qual_info_t Ctest_dnx_field_ffc_fg3_diff_sizes_qual_info_array[CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_QUALS] = {
    {"ffc_algo_fg3_1", 0, 80, {7, 8, 9}}
};
static ctest_dnx_field_ffc_qual_info_t Ctest_dnx_field_ffc_fg4_diff_sizes_qual_info_array[CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_QUALS] = {
    {"ffc_algo_fg4_1", 0, 112, {10, 11, 12, 13}}
};
static ctest_dnx_field_ffc_qual_info_t Ctest_dnx_field_ffc_cmp_hash_1_qual_info_array[CTEST_DNX_FIELD_FFC_CMP_HASH_NOF_QUALS] = {
    {"ffc_algo_cmp_h_1_1", 0, 1, {0}}, {"ffc_algo_cmp_h_1_2", 0, 1, {1}}, {"ffc_algo_cmp_h_1_3", 0, 1, {2}},
    {"ffc_algo_cmp_h_1_4", 0, 1, {3}}, {"ffc_algo_cmp_h_1_5", 0, 1, {4}}, {"ffc_algo_cmp_h_1_6", 0, 1, {5}},
    {"ffc_algo_cmp_h_1_7", 0, 1, {6}}, {"ffc_algo_cmp_h_1_8", 0, 1, {7}}, {"ffc_algo_cmp_h_1_9", 0, 1, {8}},
    {"ffc_algo_cmp_h_1_10", 0, 1, {9}}, {"ffc_algo_cmp_h_1_11", 0, 1, {10}}, {"ffc_algo_cmp_h_1_12", 0, 1, {11}},
    {"ffc_algo_cmp_h_1_13", 0, 1, {12}}, {"ffc_algo_cmp_h_1_14", 0, 1, {13}}, {"ffc_algo_cmp_h_1_15", 0, 1, {14}},
    {"ffc_algo_cmp_h_1_16", 0, 1, {15}}
};
static ctest_dnx_field_ffc_qual_info_t Ctest_dnx_field_ffc_cmp_hash_2_qual_info_array[CTEST_DNX_FIELD_FFC_CMP_HASH_NOF_QUALS] = {
    {"ffc_algo_cmp_h_1_17", 0, 2, {32}}, {"ffc_algo_cmp_h_1_18", 0, 2, {33}}, {"ffc_algo_cmp_h_1_19", 0, 2, {34}},
    {"ffc_algo_cmp_h_1_20", 0, 2, {35}}, {"ffc_algo_cmp_h_1_21", 0, 2, {36}}, {"ffc_algo_cmp_h_1_22", 0, 2, {37}},
    {"ffc_algo_cmp_h_1_23", 0, 2, {38}}, {"ffc_algo_cmp_h_1_24", 0, 2, {39}}, {"ffc_algo_cmp_h_1_25", 0, 2, {40}},
    {"ffc_algo_cmp_h_1_26", 0, 2, {41}}, {"ffc_algo_cmp_h_1_27", 0, 2, {42}}, {"ffc_algo_cmp_h_1_28", 0, 2, {43}},
    {"ffc_algo_cmp_h_1_29", 0, 2, {44}}, {"ffc_algo_cmp_h_1_30", 0, 2, {45}}, {"ffc_algo_cmp_h_1_31", 0, 2, {46}},
    {"ffc_algo_cmp_h_1_32", 0, 2, {47}}
};
static ctest_dnx_field_ffc_qual_info_t Ctest_dnx_field_ffc_cmp_hash_3_qual_info_array[CTEST_DNX_FIELD_FFC_CMP_HASH_NOF_QUALS] = {
    {"ffc_algo_cmp_h_1_33", 0, 1, {16}}, {"ffc_algo_cmp_h_1_34", 0, 1, {17}}, {"ffc_algo_cmp_h_1_35", 0, 1, {18}},
    {"ffc_algo_cmp_h_1_36", 0, 1, {19}}, {"ffc_algo_cmp_h_1_37", 0, 1, {20}}, {"ffc_algo_cmp_h_1_38", 0, 1, {21}},
    {"ffc_algo_cmp_h_1_39", 0, 1, {22}}, {"ffc_algo_cmp_h_1_40", 0, 1, {23}}, {"ffc_algo_cmp_h_1_41", 0, 1, {24}},
    {"ffc_algo_cmp_h_1_42", 0, 1, {25}}, {"ffc_algo_cmp_h_1_43", 0, 1, {26}}, {"ffc_algo_cmp_h_1_44", 0, 1, {27}},
    {"ffc_algo_cmp_h_1_45", 0, 1, {28}}, {"ffc_algo_cmp_h_1_46", 0, 1, {29}}, {"ffc_algo_cmp_h_1_47", 0, 1, {30}},
    {"ffc_algo_cmp_h_1_48", 0, 1, {31}}
};

/**
 * Following arrays are global, used for all test cases. They contain information per field group. Like:
 * fg_id, fg_name, nof_qualifiers and qualifiers.
 */
static ctest_dnx_field_ffc_fg_info_t Ctest_dnx_field_ffc_exhaustive_fg_info_array[CTEST_DNX_FIELD_FFC_IPMF123_EXHAUSTIVE_NOF_FGS] = {
/**    Field group ID    |   Field group Name   |  Number of tested qualifiers   |  Qualifier Input Type   |         Qualifiers info        */
    {BCM_FIELD_ID_INVALID, "ffc_algo_fg1", CTEST_DNX_FIELD_FFC_COMMON_NOF_QUALS, bcmFieldInputTypeMetaData, Ctest_dnx_field_ffc_fg1_common_qual_info_array},
    {BCM_FIELD_ID_INVALID, "ffc_algo_fg2", CTEST_DNX_FIELD_FFC_COMMON_NOF_QUALS, bcmFieldInputTypeMetaData, Ctest_dnx_field_ffc_fg2_common_qual_info_array}
};
static ctest_dnx_field_ffc_fg_info_t Ctest_dnx_field_ffc_ipmf1_header_fg_info_array[CTEST_DNX_FIELD_FFC_IPMF1_HEADER_NOF_FGS] = {
/**    Field group ID    |   Field group Name   |  Number of tested qualifiers   |  Qualifier Input Type   |         Qualifiers info        */
    {BCM_FIELD_ID_INVALID, "ffc_algo_fg1", CTEST_DNX_FIELD_FFC_HEADER_QUAL_NOF_QUALS, bcmFieldInputTypeLayerAbsolute, Ctest_dnx_field_ffc_fg1_header_qual_info_array},
    {BCM_FIELD_ID_INVALID, "ffc_algo_fg2", CTEST_DNX_FIELD_FFC_HEADER_QUAL_NOF_QUALS, bcmFieldInputTypeLayerAbsolute, Ctest_dnx_field_ffc_fg2_header_qual_info_array},
    {BCM_FIELD_ID_INVALID, "ffc_algo_fg3", CTEST_DNX_FIELD_FFC_HEADER_QUAL_NOF_QUALS, bcmFieldInputTypeLayerAbsolute, Ctest_dnx_field_ffc_fg3_header_qual_info_array},
};
static ctest_dnx_field_ffc_fg_info_t Ctest_dnx_field_ffc_ipmf1_layer_record_fg_info_array[CTEST_DNX_FIELD_FFC_IPMF1_LR_NOF_FGS] = {
/**    Field group ID    |   Field group Name   |  Number of tested qualifiers   |  Qualifier Input Type   |         Qualifiers info        */
    {BCM_FIELD_ID_INVALID, "ffc_algo_fg1", CTEST_DNX_FIELD_FFC_LAYER_RECORD_QUAL_NOF_QUALS, bcmFieldInputTypeLayerRecordsAbsolute, Ctest_dnx_field_ffc_fg1_layer_record_qual_info_array},
    {BCM_FIELD_ID_INVALID, "ffc_algo_fg2", CTEST_DNX_FIELD_FFC_LAYER_RECORD_QUAL_NOF_QUALS, bcmFieldInputTypeLayerRecordsAbsolute, Ctest_dnx_field_ffc_fg2_layer_record_qual_info_array}
};
static ctest_dnx_field_ffc_fg_info_t Ctest_dnx_field_ffc_diff_sizes_fg_info_array[CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_FGS] = {
/**    Field group ID    |   Field group Name   |  Number of tested qualifiers   |  Qualifier Input Type   |         Qualifiers info        */
    {BCM_FIELD_ID_INVALID, "ffc_algo_fg1", CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_QUALS, bcmFieldInputTypeMetaData, Ctest_dnx_field_ffc_fg1_diff_sizes_qual_info_array},
    {BCM_FIELD_ID_INVALID, "ffc_algo_fg2", CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_QUALS, bcmFieldInputTypeMetaData, Ctest_dnx_field_ffc_fg2_diff_sizes_qual_info_array},
    {BCM_FIELD_ID_INVALID, "ffc_algo_fg3", CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_QUALS, bcmFieldInputTypeMetaData, Ctest_dnx_field_ffc_fg3_diff_sizes_qual_info_array},
    {BCM_FIELD_ID_INVALID, "ffc_algo_fg4", CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_QUALS, bcmFieldInputTypeMetaData, Ctest_dnx_field_ffc_fg4_diff_sizes_qual_info_array}
};
static ctest_dnx_field_ffc_cmp_hash_info_t Ctest_dnx_field_ffc_cmp_hash_info_array[CTEST_DNX_FIELD_FFC_CMP_NOF_KEYS] = {
/**    Field group ID    |   Field group Name   |  Number of tested qualifiers   |  Qualifier Input Type   |         Qualifiers info        */
    {DBAL_ENUM_FVAL_FIELD_KEY_F, CTEST_DNX_FIELD_FFC_CMP_HASH_NOF_QUALS, bcmFieldInputTypeMetaData, Ctest_dnx_field_ffc_cmp_hash_1_qual_info_array},
    {DBAL_ENUM_FVAL_FIELD_KEY_G, CTEST_DNX_FIELD_FFC_CMP_HASH_NOF_QUALS, bcmFieldInputTypeLayerAbsolute, Ctest_dnx_field_ffc_cmp_hash_2_qual_info_array},
    {DBAL_ENUM_FVAL_FIELD_KEY_H, CTEST_DNX_FIELD_FFC_CMP_HASH_NOF_QUALS, bcmFieldInputTypeMetaData, Ctest_dnx_field_ffc_cmp_hash_3_qual_info_array}
};
static ctest_dnx_field_ffc_fg_info_t Ctest_dnx_field_ffc_delete_readd_fg_info_array[CTEST_DNX_FIELD_FFC_IPMF1_LR_NOF_FGS] = {
/**    Field group ID    |   Field group Name   |  Number of tested qualifiers   |  Qualifier Input Type   |         Qualifiers info        */
    {BCM_FIELD_ID_INVALID, "ffc_algo_fg1", CTEST_DNX_FIELD_FFC_LAYER_RECORD_QUAL_NOF_QUALS, bcmFieldInputTypeMetaData, Ctest_dnx_field_ffc_fg1_layer_record_qual_info_array},
    {BCM_FIELD_ID_INVALID, "ffc_algo_fg2", CTEST_DNX_FIELD_FFC_LAYER_RECORD_QUAL_NOF_QUALS, bcmFieldInputTypeMetaData, Ctest_dnx_field_ffc_fg2_layer_record_qual_info_array},
};

/* *INDENT-ON* */

/**
 * \brief
 *  This function is used to reset all FG IDs of the Global arrays, after
 *  finishing all of the scenarios per specific stage.
 *
 * \return
 *  \retval void
 */
static void
ctest_dnx_field_ffc_global_arrays_reset(
    void)
{
    int fg_iter;

    /**
     * Reset the FG ID, in the Global arrays, after finishing all of the scenarios.
     * This should be done in case we want to run the test more then once.
     * Manually reset the Global arrays, because they are reset only during the init.
     */
    for (fg_iter = 0; fg_iter < CTEST_DNX_FIELD_FFC_IPMF123_EXHAUSTIVE_NOF_FGS; fg_iter++)
    {
        Ctest_dnx_field_ffc_exhaustive_fg_info_array[fg_iter].fg_id = BCM_FIELD_ID_INVALID;
    }

    for (fg_iter = 0; fg_iter < CTEST_DNX_FIELD_FFC_IPMF1_HEADER_NOF_FGS; fg_iter++)
    {
        Ctest_dnx_field_ffc_ipmf1_header_fg_info_array[fg_iter].fg_id = BCM_FIELD_ID_INVALID;
    }

    for (fg_iter = 0; fg_iter < CTEST_DNX_FIELD_FFC_IPMF1_LR_NOF_FGS; fg_iter++)
    {
        Ctest_dnx_field_ffc_ipmf1_layer_record_fg_info_array[fg_iter].fg_id = BCM_FIELD_ID_INVALID;
    }

    for (fg_iter = 0; fg_iter < CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_FGS; fg_iter++)
    {
        Ctest_dnx_field_ffc_diff_sizes_fg_info_array[fg_iter].fg_id = BCM_FIELD_ID_INVALID;
    }

    for (fg_iter = 0; fg_iter < CTEST_DNX_FIELD_FFC_IPMF1_LR_NOF_FGS; fg_iter++)
    {
        Ctest_dnx_field_ffc_delete_readd_fg_info_array[fg_iter].fg_id = BCM_FIELD_ID_INVALID;
    }
}

/**
 * \brief
 *  This function is creating compare and hashing for given context.
 *
 * \param [in] unit - Device ID
 * \param [in] is_cmp - Flag to indicate if function was called for CMP
 *  (in case of TRUE), or HASH (in case of FALSE).
 * \param [in] nof_keys - Number of keys, used by HASH or CMP.
 * \param [in] field_stage - Field stage, on which the context
 *  was created (iPMF1/2/3, ePMF.....)
 *  \param [in] context_id - Context ID for which cmp or hash should be done.
 * \param [in] ffc_cmp_hash_info - Array which contains information for cmp and hash.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_ffc_cmp_hash_config(
    int unit,
    uint8 is_cmp,
    int nof_keys,
    bcm_field_stage_t field_stage,
    bcm_field_context_t context_id,
    ctest_dnx_field_ffc_cmp_hash_info_t * ffc_cmp_hash_info)
{
    bcm_field_context_hash_info_t hash_info;
    bcm_field_context_compare_info_t compare_info;
    bcm_field_qualifier_info_create_t qual_info;
    int qual_index;
    int key_index;
    uint32 pair_id;

    SHR_FUNC_INIT_VARS(unit);

    if (field_stage != bcmFieldStageIngressPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Function does not support any stage other than iPMF1. given stage %s\n",
                     dnx_field_bcm_stage_text(field_stage));
    }

    pair_id = 0;

    if (is_cmp)
    {
        bcm_field_context_compare_info_t_init(&compare_info);
        for (key_index = 0; key_index < nof_keys; key_index++)
        {
            compare_info.first_key_info.nof_quals = ffc_cmp_hash_info[key_index].nof_qualifiers;

            for (qual_index = 0; qual_index < ffc_cmp_hash_info[key_index].nof_qualifiers; qual_index++)
            {
                bcm_field_qualifier_info_create_t_init(&qual_info);
                qual_info.size = ffc_cmp_hash_info[key_index].ffc_qual_info[qual_index].qual_size;
                sal_strncpy_s((char *) qual_info.name, ffc_cmp_hash_info[key_index].ffc_qual_info[qual_index].name,
                              sizeof(qual_info.name));
                SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                                (unit, 0, &qual_info,
                                 &(ffc_cmp_hash_info[key_index].ffc_qual_info[qual_index].qual_type)));

                if (key_index == 0)
                {
                    compare_info.first_key_info.qual_types[qual_index] =
                        ffc_cmp_hash_info[key_index].ffc_qual_info[qual_index].qual_type;
                    compare_info.first_key_info.qual_info[qual_index].input_type =
                        ffc_cmp_hash_info[key_index].qual_input_type;
                    if (ffc_cmp_hash_info[key_index].qual_input_type == bcmFieldInputTypeLayerAbsolute
                        || ffc_cmp_hash_info[key_index].qual_input_type == bcmFieldInputTypeLayerRecordsAbsolute)
                    {
                        compare_info.first_key_info.qual_info[qual_index].input_arg = 0;
                    }
                    if (ffc_cmp_hash_info[key_index].qual_input_type == bcmFieldInputTypeMetaData)
                    {
                        compare_info.first_key_info.qual_info[qual_index].offset =
                            dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->pbus_header_length;
                    }
                    else
                    {
                        compare_info.first_key_info.qual_info[qual_index].offset = 0;
                    }
                }
                else if (key_index == 1)
                {
                    compare_info.second_key_info.nof_quals = ffc_cmp_hash_info[key_index].nof_qualifiers;
                    compare_info.second_key_info.qual_types[qual_index] =
                        ffc_cmp_hash_info[key_index].ffc_qual_info[qual_index].qual_type;
                    compare_info.second_key_info.qual_info[qual_index].input_type =
                        ffc_cmp_hash_info[key_index].qual_input_type;
                    if (ffc_cmp_hash_info[key_index].qual_input_type == bcmFieldInputTypeLayerAbsolute
                        || ffc_cmp_hash_info[key_index].qual_input_type == bcmFieldInputTypeLayerRecordsAbsolute)
                    {
                        compare_info.second_key_info.qual_info[qual_index].input_arg = 0;
                    }
                    if (ffc_cmp_hash_info[key_index].qual_input_type == bcmFieldInputTypeMetaData)
                    {
                        compare_info.second_key_info.qual_info[qual_index].offset =
                            dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->pbus_header_length;
                    }
                    else
                    {
                        compare_info.second_key_info.qual_info[qual_index].offset = 0;
                    }
                    pair_id = 0;
                }
                else
                {
                    compare_info.first_key_info.qual_types[qual_index] =
                        ffc_cmp_hash_info[key_index].ffc_qual_info[qual_index].qual_type;
                    compare_info.first_key_info.qual_info[qual_index].input_type =
                        ffc_cmp_hash_info[key_index].qual_input_type;
                    if (ffc_cmp_hash_info[key_index].qual_input_type == bcmFieldInputTypeLayerAbsolute
                        || ffc_cmp_hash_info[key_index].qual_input_type == bcmFieldInputTypeLayerRecordsAbsolute)
                    {
                        compare_info.first_key_info.qual_info[qual_index].input_arg = 0;
                    }
                    if (ffc_cmp_hash_info[key_index].qual_input_type == bcmFieldInputTypeMetaData)
                    {
                        compare_info.first_key_info.qual_info[qual_index].offset =
                            dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->pbus_header_length;
                    }
                    else
                    {
                        compare_info.first_key_info.qual_info[qual_index].offset = 0;
                    }
                    pair_id = 1;
                }
            }

            if (pair_id)
            {
                SHR_IF_ERR_EXIT(bcm_field_context_compare_create
                                (unit, 0, bcmFieldStageIngressPMF1, context_id, pair_id, &compare_info));
                bcm_field_context_compare_info_t_init(&compare_info);
            }
        }

        LOG_INFO_EX(BSL_LOG_MODULE, "CMP was configured for stage(%s) context(%d)! \n%s%s",
                    dnx_field_bcm_stage_text(field_stage), context_id, EMPTY, EMPTY);
    }
    else
    {
        bcm_field_context_hash_info_t_init(&hash_info);
        hash_info.hash_function = bcmFieldContextHashFunctionCrc16Bisync;
        hash_info.order = TRUE;
        hash_info.hash_config.function_select = bcmFieldContextHashActionValueReplaceCrc;
        hash_info.hash_config.action_key = bcmFieldContextHashActionKeyAdditionalLbKey;

        for (key_index = 0; key_index < nof_keys; key_index++)
        {
            hash_info.key_info.nof_quals = ffc_cmp_hash_info[key_index].nof_qualifiers;

            for (qual_index = 0; qual_index < ffc_cmp_hash_info[key_index].nof_qualifiers; qual_index++)
            {
                bcm_field_qualifier_info_create_t_init(&qual_info);
                qual_info.size = ffc_cmp_hash_info[key_index].ffc_qual_info[qual_index].qual_size;
                sal_strncpy_s((char *) qual_info.name, ffc_cmp_hash_info[key_index].ffc_qual_info[qual_index].name,
                              sizeof(qual_info.name));
                SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                                (unit, 0, &qual_info,
                                 &(ffc_cmp_hash_info[key_index].ffc_qual_info[qual_index].qual_type)));

                hash_info.key_info.qual_types[qual_index] =
                    ffc_cmp_hash_info[key_index].ffc_qual_info[qual_index].qual_type;
                hash_info.key_info.qual_info[qual_index].input_type = ffc_cmp_hash_info[key_index].qual_input_type;
                if (ffc_cmp_hash_info[key_index].qual_input_type == bcmFieldInputTypeLayerAbsolute ||
                    ffc_cmp_hash_info[key_index].qual_input_type == bcmFieldInputTypeLayerRecordsAbsolute)
                {
                    hash_info.key_info.qual_info[qual_index].input_arg = 0;
                }
                if (ffc_cmp_hash_info[key_index].qual_input_type == bcmFieldInputTypeMetaData)
                {
                    hash_info.key_info.qual_info[qual_index].offset =
                        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->pbus_header_length;
                }
                else
                {
                    hash_info.key_info.qual_info[qual_index].offset = 0;
                }
            }

            SHR_IF_ERR_EXIT(bcm_field_context_hash_create(unit, 0, bcmFieldStageIngressPMF1, context_id, &hash_info));
        }

        LOG_INFO_EX(BSL_LOG_MODULE, "HASH was configured for stage(%s) context(%d)! \n%s%s",
                    dnx_field_bcm_stage_text(field_stage), context_id, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is creating context and dummy preselector,
 *  used for all different scenarios.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which the context
 *  will be created (iPMF1/2/3, ePMF.....)
 * \param [in] cs_info - Array which contains information per context.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_ffc_cs_config(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_ffc_cs_info_t * cs_info)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_stage_t field_stage_internal;

    SHR_FUNC_INIT_VARS(unit);

    /** In case of IPMF2 stage the context was created in IPMF1 stage. */
    if (field_stage == bcmFieldStageIngressPMF2)
    {
        field_stage_internal = bcmFieldStageIngressPMF1;
    }
    else
    {
        field_stage_internal = field_stage;
    }

    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) context_info.name, cs_info->context_name, sizeof(context_info.name));
    if (field_stage_internal == bcmFieldStageIngressPMF1)
    {
        context_info.hashing_enabled = TRUE;
        context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
        context_info.context_compare_modes.compare_2_mode = bcmFieldContextCompareTypeSingle;
    }
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, field_stage_internal, &context_info, &cs_info->context_id));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cs_info->presel_id;
    p_id.stage = field_stage_internal;
    p_data.entry_valid = TRUE;
    p_data.context_id = cs_info->context_id;
    p_data.nof_qualifiers = 1;

    /**
     * Doesnt really mean which qualifier since the mask is 0
     * Easy to use this qualifier since present on all stages as context selection qualifier.
     */
    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = 0;
    p_data.qual_data[0].qual_mask = 0;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    LOG_INFO_EX(BSL_LOG_MODULE, "Presel (%d) was configured for stage(%s) context(%d)! \n%s",
                cs_info->presel_id, dnx_field_bcm_stage_text(field_stage_internal), cs_info->context_id, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This functions creates a field group and attaching it to a context.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which the group
 *  will be created (iPMF1/2/3, ePMF.....)
 * \param [in] context_id - Context id to which the FG will be attached.
 * \param [in] ffc_fg_info - Array which contains information per FG,
 *  like name, qualifiers.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_ffc_fg_config(
    int unit,
    bcm_field_stage_t field_stage,
    bcm_field_context_t context_id,
    ctest_dnx_field_ffc_fg_info_t * ffc_fg_info)
{
    int qual_index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_group_info_t_init(&fg_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = field_stage;

    sal_strncpy_s((char *) fg_info.name, ffc_fg_info->fg_name, sizeof(fg_info.name));
    fg_info.nof_quals = ffc_fg_info->nof_qualifiers;
    fg_info.nof_actions = CTEST_DNX_FIELD_FFC_NOF_ACTIONS;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    /** Create User Define qualifiers. */
    bcm_field_qualifier_info_create_t_init(&qual_info);

    for (qual_index = 0; qual_index < ffc_fg_info->nof_qualifiers; qual_index++)
    {
        qual_info.size = ffc_fg_info->ffc_qual_info[qual_index].qual_size;
        sal_strncpy_s((char *) qual_info.name, ffc_fg_info->ffc_qual_info[qual_index].name, sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                        (unit, 0, &qual_info, &(ffc_fg_info->ffc_qual_info[qual_index].qual_type)));

        fg_info.qual_types[qual_index] = ffc_fg_info->ffc_qual_info[qual_index].qual_type;
        attach_info.key_info.qual_types[qual_index] = fg_info.qual_types[qual_index];
        attach_info.key_info.qual_info[qual_index].input_type = ffc_fg_info->qual_input_type;
        attach_info.key_info.qual_info[qual_index].offset = 0;
        if (ffc_fg_info->qual_input_type == bcmFieldInputTypeLayerAbsolute ||
            ffc_fg_info->qual_input_type == bcmFieldInputTypeLayerRecordsAbsolute)
        {
            attach_info.key_info.qual_info[qual_index].input_arg = 0;
        }
    }

    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    fg_info.action_with_valid_bit[0] = TRUE;

    /** Create the field group. */
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &(ffc_fg_info->fg_id)));

    LOG_INFO_EX(BSL_LOG_MODULE, "Field Group ID %d was created. \n %s%s%s", ffc_fg_info->fg_id, EMPTY, EMPTY, EMPTY);

    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0, ffc_fg_info->fg_id);

    /** Attach the created field group to the context. */
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, ffc_fg_info->fg_id, context_id, &attach_info));

    LOG_INFO_EX(BSL_LOG_MODULE, "Field Group ID %d was attached to Context ID %d. \n %s%s", ffc_fg_info->fg_id,
                context_id, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function compares returned FFC_ID from the DNX layer, with
 *  the expected one, which is given in ffc_fg_info.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, of which the context FFC
 *  info will be retrieve (iPMF1/2/3, ePMF.....)
 * \param [in] context_id - Context id for which the FFC info will
 *  be retrieve.
 * \param [in] ffc_fg_info - Array which contains information per FG,
 *  like name, qualifiers and going to be compared with
 *  the retrieved context FFC info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_ffc_compare(
    int unit,
    bcm_field_stage_t field_stage,
    bcm_field_context_t context_id,
    ctest_dnx_field_ffc_fg_info_t * ffc_fg_info)
{
    dnx_field_stage_e dnx_field_stage;
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    int qual_iter, ffc_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    /** Convert the BCM field stage to DNX one. */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, field_stage, &dnx_field_stage));

    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, ffc_fg_info->fg_id, context_id, attach_full_info));

    /** Prepare and print FFCs and qualifiers which are using them. */
    for (qual_iter = 0; qual_iter < ffc_fg_info->nof_qualifiers; qual_iter++)
    {
        for (ffc_iter = 0; ffc_iter < DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL &&
             attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].ffc.ffc_id !=
             DNX_FIELD_FFC_ID_INVALID; ffc_iter++)
        {
            /** Check if the retrieved FFC ID is same as the expected FFC ID. */
            if (attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].ffc.ffc_id !=
                ffc_fg_info->ffc_qual_info[qual_iter].ffc_id_expected[ffc_iter])
            {
                LOG_INFO_EX(BSL_LOG_MODULE,
                            "FFC ID compare operation failed for CTX %d (FG %d). Expected FFC ID %d | Actual FFC ID %d! \n",
                            context_id, ffc_fg_info->fg_id,
                            ffc_fg_info->ffc_qual_info[qual_iter].ffc_id_expected[ffc_iter],
                            attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].
                            ffc.ffc_id);
                SHR_EXIT();
            }
        }
    }

    LOG_INFO_EX(BSL_LOG_MODULE, "FFC ID compare operation for CTX %d (FG %d) was successful! \n %s%s",
                context_id, ffc_fg_info->fg_id, EMPTY, EMPTY);

exit:
    SHR_FREE(attach_full_info);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function compares returned initial FFC_ID from the DNX layer, with
 *  the expected one, which is given in ffc_cmp_hash_info.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, of which the context FFC
 *  info will be retrieve (iPMF1/2/3, ePMF.....)
 * \param [in] context_id - Context id for which the FFC info will
 *  be retrieve.
 * \param [in] ffc_cmp_hash_info - Array which contains information per CMP or HASH modes,
 *  like key_id, qualifiers and going to be compared with
 *  the retrieved context FFC info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_ffc_cmp_hash_compare(
    int unit,
    bcm_field_stage_t field_stage,
    bcm_field_context_t context_id,
    ctest_dnx_field_ffc_cmp_hash_info_t * ffc_cmp_hash_info)
{
    dnx_field_stage_e dnx_field_stage;
    int qual_iter;
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];

    SHR_FUNC_INIT_VARS(unit);

    /** Convert the BCM field stage to DNX one. */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, field_stage, &dnx_field_stage));

    /** Get an array of ffc_id, which are used for construction of the current inital_key. */
    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get
                    (unit, dnx_field_stage, context_id, ffc_cmp_hash_info->initial_key_id, ffc_id));

    /** Prepare and print FFCs and qualifiers which are using them. */
    for (qual_iter = 0; qual_iter < ffc_cmp_hash_info->nof_qualifiers; qual_iter++)
    {
        /** Check if the retrieved FFC ID is same as the expected FFC ID. */
        if (ffc_id[qual_iter] != ffc_cmp_hash_info->ffc_qual_info[qual_iter].ffc_id_expected[0])
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "FFC ID (initial) compare operation failed for CTX %d. Expected FFC ID %d | Actual FFC ID %d! \n %s",
                        context_id, ffc_cmp_hash_info->ffc_qual_info[qual_iter].ffc_id_expected[0], ffc_id[qual_iter],
                        EMPTY);
            SHR_EXIT();
        }
    }

    LOG_INFO_EX(BSL_LOG_MODULE, "FFC ID (initial) compare operation for CTX %d was successful! \n %s%s%s",
                context_id, EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function destroys a context and sets its presel to invalid.
 *  It is used for the generic destroy function and for directly
 *  called for some of the negative cases, where we need just to
 *  destroy the created context.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the context was created (iPMF1/2/3, ePMF.....).
 * \param [in] ffc_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_ffc_cs_destroy(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_ffc_cs_info_t * ffc_cs_info)
{
    bcm_field_stage_t field_stage_internal;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;

    SHR_FUNC_INIT_VARS(unit);

    /** In case of IPMF2 stage the context was created in IPMF1 stage. */
    if (field_stage == bcmFieldStageIngressPMF2)
    {
        field_stage_internal = bcmFieldStageIngressPMF1;
    }
    else
    {
        field_stage_internal = field_stage;
    }

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    p_id.stage = field_stage_internal;
    p_id.presel_id = ffc_cs_info->presel_id;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data));

    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, field_stage_internal, ffc_cs_info->context_id));
    ffc_cs_info->context_id = BCM_FIELD_CONTEXT_ID_INVALID;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function destroys given FG and all of the
 *  configured qualifiers.
 *
 * \param [in] unit - Device ID
 * \param [in] ffc_fg_info - Array which contains information per FG,
 *  like fg_id, name and qualifier_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 * \remark
 */
static shr_error_e
ctest_dnx_field_ffc_fg_destroy(
    int unit,
    ctest_dnx_field_ffc_fg_info_t * ffc_fg_info)
{
    int qual_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, ffc_fg_info->fg_id));

    for (qual_index = 0; qual_index < ffc_fg_info->nof_qualifiers; qual_index++)
    {
        SHR_IF_ERR_EXIT(bcm_field_qualifier_destroy(unit, ffc_fg_info->ffc_qual_info[qual_index].qual_type));
        ffc_fg_info->ffc_qual_info[qual_index].qual_type = bcmFieldQualifyCount;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents negative case, where we are creating a
 *  context and FGs equal to nof_fgs, to which we add 16 qualifiers,
 *  then trying to add another qualifier.
 *
 * \param [in] unit - Device ID
 * \param [in] nof_fgs - Number of FGs to be created.
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] ffc_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] ffc_fg_info - Array which contains information per FG,
 *  like fg_id, name and qual_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_ffc_exhaustive(
    int unit,
    int nof_fgs,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_ffc_cs_info_t * ffc_cs_info,
    ctest_dnx_field_ffc_fg_info_t * ffc_fg_info)
{
    int fg_index;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = BCM_E_NONE;

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cs_config(unit, field_stage, &ffc_cs_info[0]));

    for (fg_index = 0; fg_index < nof_fgs; fg_index++)
    {
        /** Create a field group and attach it to the created context selection. */
        if (fg_index < nof_fgs - 1)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_fg_config
                            (unit, field_stage, ffc_cs_info[0].context_id, &ffc_fg_info[fg_index]));
        }
        else
        {
            rv = ctest_dnx_field_ffc_fg_config(unit, field_stage, ffc_cs_info[0].context_id, &ffc_fg_info[fg_index]);
            if (rv == BCM_E_RESOURCE)
            {
                LOG_INFO_EX(BSL_LOG_MODULE, "EXAUSTIVE TEST NEGATIVE CASE was successful! \r\n %s%s%s%s", EMPTY, EMPTY,
                            EMPTY, EMPTY);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "EXAUSTIVE NEGATIVE TEST CASE failed!\r\n");
            }
        }
    }

    for (fg_index = 0; fg_index < nof_fgs; fg_index++)
    {
        if (fg_index < nof_fgs - 1)
        {
            SHR_IF_ERR_EXIT(bcm_field_group_context_detach
                            (unit, ffc_fg_info[fg_index].fg_id, ffc_cs_info[0].context_id));
            SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_fg_destroy(unit, &ffc_fg_info[fg_index]));
        }
        else
        {
            SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cs_destroy(unit, field_stage, &ffc_cs_info[0]));
            SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_fg_destroy(unit, &ffc_fg_info[fg_index]));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents positive case, where we
 *  Create 3 FGs and add 16 header qualifiers to every group. Attach them to the context.
    Expecting all FFC group 1/3/4 to be used. IPMF1 only.
 *
 * \param [in] unit - Device ID
 * \param [in] nof_fgs - Number of FGs to be created.
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1).
 * \param [in] ffc_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] ffc_fg_info - Array which contains information per FG,
 *  like fg_id, name and qual_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_ffc_header(
    int unit,
    int nof_fgs,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_ffc_cs_info_t * ffc_cs_info,
    ctest_dnx_field_ffc_fg_info_t * ffc_fg_info)
{
    int fg_index;

    SHR_FUNC_INIT_VARS(unit);

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cs_config(unit, field_stage, &ffc_cs_info[0]));

    for (fg_index = 0; fg_index < nof_fgs; fg_index++)
    {
        /** Create a field group and attach it to the created context selection. */
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_fg_config
                        (unit, field_stage, ffc_cs_info[0].context_id, &ffc_fg_info[fg_index]));
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_compare
                        (unit, field_stage, ffc_cs_info[0].context_id, &ffc_fg_info[fg_index]));
    }

    for (fg_index = 0; fg_index < nof_fgs; fg_index++)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, ffc_fg_info[fg_index].fg_id, ffc_cs_info[0].context_id));
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_fg_destroy(unit, &ffc_fg_info[fg_index]));
    }

    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cs_destroy(unit, field_stage, &ffc_cs_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents positive case, where we
 *  Create 2 FGs and add 16 LR qualifiers to every group. Attach them to the context.
 *  Expecting all FFC group 1/2 to be used. IPMF1 only.
 *
 * \param [in] unit - Device ID
 * \param [in] nof_fgs - Number of FGs to be created.
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1).
 * \param [in] ffc_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] ffc_fg_info - Array which contains information per FG,
 *  like fg_id, name and qual_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_ffc_layer_record(
    int unit,
    int nof_fgs,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_ffc_cs_info_t * ffc_cs_info,
    ctest_dnx_field_ffc_fg_info_t * ffc_fg_info)
{
    int fg_index;

    SHR_FUNC_INIT_VARS(unit);

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cs_config(unit, field_stage, &ffc_cs_info[0]));

    for (fg_index = 0; fg_index < nof_fgs; fg_index++)
    {
        /** Create a field group and attach it to the created context selection. */
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_fg_config
                        (unit, field_stage, ffc_cs_info[0].context_id, &ffc_fg_info[fg_index]));
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_compare
                        (unit, field_stage, ffc_cs_info[0].context_id, &ffc_fg_info[fg_index]));
    }

    for (fg_index = 0; fg_index < nof_fgs; fg_index++)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, ffc_fg_info[fg_index].fg_id, ffc_cs_info[0].context_id));
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_fg_destroy(unit, &ffc_fg_info[fg_index]));
    }

    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cs_destroy(unit, field_stage, &ffc_cs_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents positive case, where we
 *  Create nof_fgs FGs and add 1 qualifier, with different size, to every group. Attach them to the context.
 *  Expecting all right amount of FFCs to be used for different sizes.
 *
 * \param [in] unit - Device ID
 * \param [in] nof_fgs - Number of FGs to be created.
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1).
 * \param [in] ffc_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] ffc_fg_info - Array which contains information per FG,
 *  like fg_id, name and qual_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_ffc_diff_sizes(
    int unit,
    int nof_fgs,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_ffc_cs_info_t * ffc_cs_info,
    ctest_dnx_field_ffc_fg_info_t * ffc_fg_info)
{
    int fg_index;

    SHR_FUNC_INIT_VARS(unit);

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cs_config(unit, field_stage, &ffc_cs_info[0]));

    for (fg_index = 0; fg_index < nof_fgs; fg_index++)
    {
        /** Create a field group and attach it to the created context selection. */
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_fg_config
                        (unit, field_stage, ffc_cs_info[0].context_id, &ffc_fg_info[fg_index]));
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_compare
                        (unit, field_stage, ffc_cs_info[0].context_id, &ffc_fg_info[fg_index]));
    }

    for (fg_index = 0; fg_index < nof_fgs; fg_index++)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, ffc_fg_info[fg_index].fg_id, ffc_cs_info[0].context_id));
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_fg_destroy(unit, &ffc_fg_info[fg_index]));
    }

    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cs_destroy(unit, field_stage, &ffc_cs_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents positive case, where we
 *  Create a Context with CMP or HASH mode (is_cmp) add qualifiers. Attach them to the context.
 *  Expecting only first 48 FFCs (0-47) to be used by different modes.
 *
 * \param [in] unit - Device ID
 * \param [in] is_cmp - Flag to indicate if function was called for CMP
 *  (in case of TRUE), or HASH (in case of FALSE).
 * \param [in] nof_keys - Number of KEYs to be used.
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1).
 * \param [in] ffc_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] ffc_cmp_hash_info - Array which contains information for CMP or HASH,
 *  like key_id, qual_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_ffc_cmp_hash(
    int unit,
    uint8 is_cmp,
    int nof_keys,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_ffc_cs_info_t * ffc_cs_info,
    ctest_dnx_field_ffc_cmp_hash_info_t * ffc_cmp_hash_info)
{
    int key_index;
    int qual_index;

    SHR_FUNC_INIT_VARS(unit);

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cs_config(unit, field_stage, &ffc_cs_info[0]));
    /** Create a field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cmp_hash_config
                    (unit, is_cmp, nof_keys, field_stage, ffc_cs_info[0].context_id, ffc_cmp_hash_info));

    for (key_index = 0; key_index < nof_keys; key_index++)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cmp_hash_compare
                        (unit, field_stage, ffc_cs_info[0].context_id, &ffc_cmp_hash_info[key_index]));
    }

    for (key_index = 0; key_index < nof_keys; key_index++)
    {
        for (qual_index = 0; qual_index < ffc_cmp_hash_info[key_index].nof_qualifiers; qual_index++)
        {
            SHR_IF_ERR_EXIT(bcm_field_qualifier_destroy
                            (unit, ffc_cmp_hash_info[key_index].ffc_qual_info[qual_index].qual_type));
            ffc_cmp_hash_info[key_index].ffc_qual_info[qual_index].qual_type = bcmFieldQualifyCount;
        }
    }

    if (is_cmp)
    {
        /** Compare destroy the first and second pair_ids. */
        SHR_IF_ERR_EXIT(bcm_field_context_compare_destroy
                        (unit, bcmFieldStageIngressPMF1, ffc_cs_info[0].context_id, 0));
        SHR_IF_ERR_EXIT(bcm_field_context_compare_destroy
                        (unit, bcmFieldStageIngressPMF1, ffc_cs_info[0].context_id, 1));
    }
    else
    {
        /** Hashing destroy. */
        SHR_IF_ERR_EXIT(bcm_field_context_hash_destroy(unit, bcmFieldStageIngressPMF1, ffc_cs_info[0].context_id));
    }

    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cs_destroy(unit, field_stage, &ffc_cs_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents positive case, where we
 *  Create a Context. Create one FG with 16 qualifiers and attach it to the context.
 *  Create another FG with 16 qualifiers (4 for EPMF stage, because we have 20 FFCs).
 *  Do several iterations of attaching and detaching the second FG to the context.
 *  Expecting after every attach and detach operation, same FFC IDs to be used
 *  by the second FG.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1).
 * \param [in] ffc_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] ffc_fg_info - Array which contains information per FG,
 *  like fg_id, name and qual_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_ffc_delete_readd(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_ffc_cs_info_t * ffc_cs_info,
    ctest_dnx_field_ffc_fg_info_t * ffc_fg_info)
{
    int delete_readd_index;

    SHR_FUNC_INIT_VARS(unit);

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cs_config(unit, field_stage, &ffc_cs_info[0]));

    /** Create a field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_fg_config(unit, field_stage, ffc_cs_info[0].context_id, &ffc_fg_info[0]));
    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_compare(unit, field_stage, ffc_cs_info[0].context_id, &ffc_fg_info[0]));

    for (delete_readd_index = 0; delete_readd_index < CTEST_DNX_FIELD_FFC_NOF_DELETE_READD_ITERATIONS;
         delete_readd_index++)
    {
        /** Create a field group and attach it to the created context selection. */
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_fg_config(unit, field_stage, ffc_cs_info[0].context_id, &ffc_fg_info[1]));
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_compare(unit, field_stage, ffc_cs_info[0].context_id, &ffc_fg_info[1]));

        SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, ffc_fg_info[1].fg_id, ffc_cs_info[0].context_id));
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_fg_destroy(unit, &ffc_fg_info[1]));
    }

    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, ffc_fg_info[0].fg_id, ffc_cs_info[0].context_id));
    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_fg_destroy(unit, &ffc_fg_info[0]));

    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cs_destroy(unit, field_stage, &ffc_cs_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This functions tests the FFC usage, positive and negative cases.
 *
 * \param [in] unit - Device ID
 * \param [in] args - Extra arguments
 * \param [in] sand_control - Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_ffc_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_field_stage_t field_stage, stage_index;
    bsl_severity_t original_severity_fld_proc = BSL_INFO;
    bsl_severity_t original_severity_dbal_dnx = BSL_INFO;
    bsl_severity_t original_severity_sw_state = BSL_INFO;
    bsl_severity_t original_severity_general_sw_state = BSL_INFO;
    int exhaustive_nof_fgs;
    ctest_dnx_field_ffc_fg_info_t *exhaustive_fg_info;
    int diff_sizes_nof_fgs;
    ctest_dnx_field_ffc_fg_info_t *delete_readd_fg_info;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Save the original severity level of the fld proc and dbal.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbal_dnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_sw_state);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_general_sw_state);

    /** Do the testing for all field stages IPMF1/2/3/EPMF. */
    for (stage_index = 0; stage_index < CTEST_DNX_FIELD_FFC_NOF_STAGES; stage_index++)
    {
        switch (stage_index)
        {
            case 0:
            {
                field_stage = bcmFieldStageIngressPMF1;
                exhaustive_nof_fgs = CTEST_DNX_FIELD_FFC_IPMF123_EXHAUSTIVE_NOF_FGS;
                Ctest_dnx_field_ffc_exhaustive_fg_info_array[CTEST_DNX_FIELD_FFC_IPMF123_EXHAUSTIVE_NOF_FGS -
                                                             1].ffc_qual_info[CTEST_DNX_FIELD_FFC_COMMON_NOF_QUALS
                                                                              - 1].qual_size = 42;
                exhaustive_fg_info = Ctest_dnx_field_ffc_exhaustive_fg_info_array;
                diff_sizes_nof_fgs = CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_FGS;
                delete_readd_fg_info = Ctest_dnx_field_ffc_delete_readd_fg_info_array;
                break;
            }
            case 1:
            {
                field_stage = bcmFieldStageIngressPMF2;
                exhaustive_nof_fgs = CTEST_DNX_FIELD_FFC_IPMF123_EXHAUSTIVE_NOF_FGS;
                Ctest_dnx_field_ffc_exhaustive_fg_info_array[CTEST_DNX_FIELD_FFC_IPMF123_EXHAUSTIVE_NOF_FGS -
                                                             1].ffc_qual_info
                    [CTEST_DNX_FIELD_FFC_COMMON_NOF_QUALS - 1].qual_size = 42;
                exhaustive_fg_info = Ctest_dnx_field_ffc_exhaustive_fg_info_array;
                diff_sizes_nof_fgs = CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_FGS;
                delete_readd_fg_info = Ctest_dnx_field_ffc_delete_readd_fg_info_array;
                break;
            }
            case 2:
            {
                field_stage = bcmFieldStageIngressPMF3;
                exhaustive_nof_fgs = CTEST_DNX_FIELD_FFC_IPMF123_EXHAUSTIVE_NOF_FGS;
                Ctest_dnx_field_ffc_exhaustive_fg_info_array[CTEST_DNX_FIELD_FFC_IPMF123_EXHAUSTIVE_NOF_FGS -
                                                             1].ffc_qual_info
                    [CTEST_DNX_FIELD_FFC_COMMON_NOF_QUALS - 1].qual_size = 42;
                exhaustive_fg_info = Ctest_dnx_field_ffc_exhaustive_fg_info_array;
                diff_sizes_nof_fgs = CTEST_DNX_FIELD_FFC_IPMF3_DIFF_SIZES_NOF_FGS;
                delete_readd_fg_info = Ctest_dnx_field_ffc_delete_readd_fg_info_array;
                break;
            }
            case 3:
            {
                field_stage = bcmFieldStageEgress;
                exhaustive_nof_fgs = CTEST_DNX_FIELD_FFC_EPMF_EXHAUSTIVE_NOF_FGS;
                exhaustive_fg_info = Ctest_dnx_field_ffc_exhaustive_fg_info_array;
                diff_sizes_nof_fgs = CTEST_DNX_FIELD_FFC_EPMF_DIFF_SIZES_NOF_FGS;
                Ctest_dnx_field_ffc_delete_readd_fg_info_array[CTEST_DNX_FIELD_FFC_IPMF1_LR_NOF_FGS -
                                                               1].nof_qualifiers =
                    CTEST_DNX_FIELD_FFC_EPMF_DELETE_READD_FG2_NOF_QUALS;
                delete_readd_fg_info = Ctest_dnx_field_ffc_delete_readd_fg_info_array;
                break;
            }
            default:
            {
                field_stage = bcmFieldStageIngressPMF1;
                exhaustive_nof_fgs = CTEST_DNX_FIELD_FFC_IPMF123_EXHAUSTIVE_NOF_FGS;
                Ctest_dnx_field_ffc_exhaustive_fg_info_array[CTEST_DNX_FIELD_FFC_IPMF123_EXHAUSTIVE_NOF_FGS -
                                                             1].ffc_qual_info[CTEST_DNX_FIELD_FFC_COMMON_NOF_QUALS
                                                                              - 1].qual_size = 42;
                exhaustive_fg_info = Ctest_dnx_field_ffc_exhaustive_fg_info_array;
                diff_sizes_nof_fgs = CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_FGS;
                delete_readd_fg_info = Ctest_dnx_field_ffc_delete_readd_fg_info_array;
                break;
            }
        }

        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

        LOG_INFO_EX(BSL_LOG_MODULE, "\nSTAGE %s \n %s%s%s", dnx_field_bcm_stage_text(field_stage), EMPTY, EMPTY, EMPTY);

        LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 1: FFC Exhaustive \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_exhaustive(unit, exhaustive_nof_fgs, field_stage,
                                                       Ctest_dnx_field_ffc_cs_info_array, exhaustive_fg_info));

        LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 2: FFC Different sizes \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_diff_sizes(unit, diff_sizes_nof_fgs, field_stage,
                                                       Ctest_dnx_field_ffc_cs_info_array,
                                                       Ctest_dnx_field_ffc_diff_sizes_fg_info_array));

        LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 3: FFC delete re-add \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_delete_readd(unit, field_stage,
                                                         Ctest_dnx_field_ffc_cs_info_array, delete_readd_fg_info));

        /** Cases relevant only for IPMF1 stage. */
        if (field_stage == bcmFieldStageIngressPMF1)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 4: FFC Header \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_header(unit, CTEST_DNX_FIELD_FFC_IPMF1_HEADER_NOF_FGS, field_stage,
                                                       Ctest_dnx_field_ffc_cs_info_array,
                                                       Ctest_dnx_field_ffc_ipmf1_header_fg_info_array));

            LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 5: FFC Layer Record \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_layer_record(unit, CTEST_DNX_FIELD_FFC_IPMF1_LR_NOF_FGS, field_stage,
                                                             Ctest_dnx_field_ffc_cs_info_array,
                                                             Ctest_dnx_field_ffc_ipmf1_layer_record_fg_info_array));

            LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 6: FFC Compare \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cmp_hash(unit, TRUE, CTEST_DNX_FIELD_FFC_CMP_NOF_KEYS, field_stage,
                                                         Ctest_dnx_field_ffc_cs_info_array,
                                                         Ctest_dnx_field_ffc_cmp_hash_info_array));

            Ctest_dnx_field_ffc_cmp_hash_info_array[0].initial_key_id = DBAL_ENUM_FVAL_FIELD_KEY_I;
            LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 7: FFC Hashing \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_cmp_hash(unit, FALSE, CTEST_DNX_FIELD_FFC_HASH_NOF_KEYS, field_stage,
                                                         Ctest_dnx_field_ffc_cs_info_array,
                                                         Ctest_dnx_field_ffc_cmp_hash_info_array));
        }

        /** Reset global arrays values to the default one, for every stage. */
        ctest_dnx_field_ffc_global_arrays_reset();

        /**
         * Set back the severity level of the field proc and dbal to the original values.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbal_dnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_sw_state);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_general_sw_state);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - runs the FFC ctest
 *
 * \param [in] unit - the unit number in system
 * \param [in] args - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "ctest field ffc"
 */
shr_error_e
sh_dnx_field_ffc_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnx_field_ffc_run(unit, args, sand_control));

exit:
    SHR_FUNC_EXIT;
}

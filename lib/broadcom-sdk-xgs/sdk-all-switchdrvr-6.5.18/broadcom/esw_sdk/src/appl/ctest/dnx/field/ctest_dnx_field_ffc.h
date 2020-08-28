/** \file ctest_dnx_field_ffc.h
 *
 *      FFC algorithm testing.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
* Include files.
* {
*/
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/field/field_map.h>
/*
 * }
 */

#ifndef DIAG_DNX_FIELD_FFC_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_FIELD_FFC_H_INCLUDED

/*
 * Number of qualifiers which will be used in the FFC tests.
 */
#define CTEST_DNX_FIELD_FFC_NOF_ACTIONS   1
/*
 * Number of stages which will be tested in the FFC tests.
 */
#define CTEST_DNX_FIELD_FFC_NOF_STAGES   4
/*
 * Number of delete re-add iteration which will be tested in the FFC tests.
 */
#define CTEST_DNX_FIELD_FFC_NOF_DELETE_READD_ITERATIONS    20
/*
 * Number of qualifiers, for different cases, which will be tested in the FFC tests.
 */
#define CTEST_DNX_FIELD_FFC_COMMON_NOF_QUALS   BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP
#define CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_QUALS  1
#define CTEST_DNX_FIELD_FFC_EPMF_DELETE_READD_FG2_NOF_QUALS  4
#define CTEST_DNX_FIELD_FFC_HEADER_QUAL_NOF_QUALS  16
#define CTEST_DNX_FIELD_FFC_LAYER_RECORD_QUAL_NOF_QUALS  16
#define CTEST_DNX_FIELD_FFC_CMP_HASH_NOF_QUALS  16
/*
 * Number of FGs, for different cases, which will be tested in the FFC tests.
 */
#define CTEST_DNX_FIELD_FFC_IPMF123_EXHAUSTIVE_NOF_FGS     2
#define CTEST_DNX_FIELD_FFC_EPMF_EXHAUSTIVE_NOF_FGS     1
#define CTEST_DNX_FIELD_FFC_IPMF1_HEADER_NOF_FGS     3
#define CTEST_DNX_FIELD_FFC_IPMF1_LR_NOF_FGS     2
#define CTEST_DNX_FIELD_FFC_DIFF_SIZES_NOF_FGS     4
#define CTEST_DNX_FIELD_FFC_IPMF3_DIFF_SIZES_NOF_FGS     1
#define CTEST_DNX_FIELD_FFC_EPMF_DIFF_SIZES_NOF_FGS     2
#define CTEST_DNX_FIELD_FFC_CMP_NOF_KEYS     3
#define CTEST_DNX_FIELD_FFC_HASH_NOF_KEYS     1

/**
 * This structure holds the information about context,
 * which will be created for FFC Algo tests.
 */
typedef struct
{
    /*
     * Context id.
     */
    bcm_field_context_t context_id;
    /*
     * Context name.
     */
    char *context_name;
    /*
     * Presel id.
     */
    bcm_field_presel_t presel_id;

} ctest_dnx_field_ffc_cs_info_t;

/**
 * This structure holds the information about
 * user and predefined qualifiers,
 * which will be created for FFC Algo tests.
 */
typedef struct
{
    /*
     * Qualifier name.
     */
    char *name;
    /*
     * Qualifier type.
     */
    bcm_field_qualify_t qual_type;
    /*
     * Qualifier size in bits.
     */
    int qual_size;
    /*
     * Expected FFC ID to be used for comparison to the
     * returned info from the context info.
     */
    int ffc_id_expected[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL];

} ctest_dnx_field_ffc_qual_info_t;

/**
 * This structure holds all needed information
 * about configuring of one field group.
 */
typedef struct
{
    /*
     * Field Group ID.
     */
    bcm_field_group_t fg_id;
    /*
     * Field Group Name.
     */
    char *fg_name;
    /*
     * Number of qualifiers per group.
     */
    int nof_qualifiers;
    /*
     * Qualifiers Input Type.
     */
    bcm_field_input_types_t qual_input_type;
    /*
     * Qualifiers Info.
     */
    ctest_dnx_field_ffc_qual_info_t *ffc_qual_info;

} ctest_dnx_field_ffc_fg_info_t;

/**
 * This structure holds all needed information
 * about configuring of compare and hash.
 */
typedef struct
{
    /*
     * Initial Key ID.
     */
    dbal_enum_value_field_field_key_e initial_key_id;
    /*
     * Number of qualifiers per group.
     */
    int nof_qualifiers;
    /*
     * Qualifiers Input Type.
     */
    bcm_field_input_types_t qual_input_type;
    /*
     * Qualifiers Info.
     */
    ctest_dnx_field_ffc_qual_info_t *ffc_qual_info;

} ctest_dnx_field_ffc_cmp_hash_info_t;

/**
 * \brief
 *   Options list for 'FFC' shell command
 */
extern sh_sand_option_t Sh_dnx_field_ffc_options[];

/**
 * \brief
 *   man for 'FFC' command
 */
extern sh_sand_man_t Sh_dnx_field_ffc_man;
/**
 * \brief
 *   Options list for 'FFC' tests to run on regression, precommit, ...
 */
extern sh_sand_invoke_t Sh_dnx_field_ffc_tests[];

/**
 * \brief - run FFC sequence in ctest shell
 *
 * \param [in] unit - Number of hardware unit used
 *      [in] args - Pointer to args_t struct, not used
 *      [in] sand_control - Pointer to comamnd control structure used for parameter delivery
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
shr_error_e sh_dnx_field_ffc_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* DIAG_DNX_FIELD_FFC_H_INCLUDED */

/** \file diag_dnx_field_key.c
 *
 * 'key' operations procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX
/*
 * Include files.
 * {
 */
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <sal/types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/field/field_key.h>
#include <bcm_int/dnx/field/field_context.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/dnx/diag_dnx_field.h>

#include "ctest_dnx_field_key.h"
#include "ctest_dnx_field_utils.h"
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_key_alloc_access.h>

#include <appl/diag/shell.h>

/*
 * }
 */
/*
 * DEFINEs
 * {
 */
/*
 * The various values available for 'test_type' input.
 */

/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/*
 * }
 */
/*
 * MACROs
 * {
 */
/**
 * \brief
 *   Keyword for test type on key command (key testing)
 */
#define DNX_DIAG_KEY_OPTION_TEST_TYPE        "type"

#define DNX_DIAG_KEY_OPTION_TEST_STAGE       "stage"
#define DNX_DIAG_KEY_OPTION_TEST_GROUP_TYPE  "group_type"
/**
 * \brief
 *   Keyword for the context or context range (minimal - maximal) for which key info is to be displayed
 */
#define DNX_DIAG_KEY_OPTION_TEST_CONTEXT     "context"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define DNX_DIAG_KEY_OPTION_TEST_COUNT        "count"
/**
 * \brief
 *   For positive 'allocate' (do allocate) only.
 *   Keyword for the number of bits to allocate.
 */
#define DNX_DIAG_KEY_OPTION_TEST_NUM_BITS     "num_bits"
/**
 * \brief
 *   For negative 'allocate' (free).
 *   Keyword for the offset of bit-range (within key) to free
 *   For positive 'allocate' (do allocate).
 *   Keyword for the offset of the bit to align (within 'num_bits'.
 *   See "align" below.
 */
#define DNX_DIAG_KEY_OPTION_TEST_OFFEST       "offset"
/**
 * \brief
 *   Keyword for the key (containing bit-range) to free
 */
#define DNX_DIAG_KEY_OPTION_TEST_KEY          "key"
/**
 * \brief
 *   Keyword for controlling whether to release resources at end of test.
 *   clean can be either 0 or 1, if the ctest will clean-up after its finished setting-up.
 */
#define DNX_DIAG_KEY_OPTION_CLEAN             "clean"
/**
 * \brief
 *   Keyword for controlling whether to free specified bit-range or to allocate it.
 *   Can be either 0 (free) or 1 (allocate).
 */
#define DNX_DIAG_KEY_OPTION_TEST_ALLOCATE     "allocate"
/**
 * \brief
 *   For positive 'allocate' only. Keyword for controlling whether to align the bit indicated
 *   on 'aligned_bit' to 16.
 *   Can be either 0 (do not align) or 1 (do align).
 */
#define DNX_DIAG_KEY_OPTION_TEST_ALIGN        "align"

/**
 * \brief
 * Input is the description of the key allocation request.
 * This macro displays all input parameters for a key allocation request.
 * The elements are:
 * * stage
 * * field group type
 * * context ID
 * * key length
*/
#define DNX_DIAG_KEY_DISPLAY_ALLOC_PARAMS(_key_alloc_description) \
{  \
    LOG_INFO_EX(BSL_LOG_MODULE, _key_alloc_description \
               "  field_stage       %s (%d)\r\n"       \
               "  fg_type           %s (%d)\r\n",      \
               dnx_field_stage_e_get_name(field_stage), field_stage, \
               dnx_field_group_type_e_get_name(fg_type[alloc_test_step]), fg_type[alloc_test_step]); \
    LOG_INFO_EX(BSL_LOG_MODULE, \
               "  context_id        %d\r\n"          \
               "  key_length        %s (%d) %s\r\n", \
               context_id[alloc_test_step],          \
               dnx_field_key_length_type_e_get_name(key_length[alloc_test_step]), \
               key_length[alloc_test_step],EMPTY);                    \
}
/**
 * \brief
 * This macro displays all result parameters of a key allocation request.
 * The elements are:
 * * key id
 * * key part type
 * * Encoded key allocation id
*/
#define DNX_DIAG_KEY_DISPLAY_ALLOC_RESULTS() \
{  \
    LOG_INFO_EX(BSL_LOG_MODULE, "Result of allocation request:\r\n" \
        "  key_id_0          %c (%d)\r\n" \
        "  key_id_1          %c (%d)\r\n", \
        'A' + key_id[alloc_test_step].id[0], key_id[alloc_test_step].id[0], \
        (key_id[alloc_test_step].id[1] == DNX_FIELD_KEY_ID_INVALID)? '-' : 'A' + key_id[alloc_test_step].id[1],\
         key_id[alloc_test_step].id[1]); \
    LOG_INFO_EX(BSL_LOG_MODULE, \
        "  full_key_allocated %s%s%s%s\r\n", \
        ((key_id[alloc_test_step].full_key_allocated)?("yes"):("no")), EMPTY,EMPTY,EMPTY); \
}
/**
 * \brief
 * Input is the description of the 'key free' request.
 * This macro displays all parameters of a key free request.
 * The elements are:
 * * field stage
 * * field group type type
 * * context
 * * key part type
 * * key id
*/
#define DNX_DIAG_KEY_DISPLAY_FREE_PARAMS(_key_free_description) \
{  \
    LOG_INFO_EX(BSL_LOG_MODULE, _key_free_description \
        "  field_stage       %s (%d)\r\n"  \
        "  fg_type           %s (%d)\r\n", \
        dnx_field_stage_e_get_name(field_stage), field_stage, \
        dnx_field_group_type_e_get_name(fg_type[free_test_step]), fg_type[free_test_step]); \
    LOG_INFO_EX(BSL_LOG_MODULE,               \
        "  context_id        %d\r\n"          \
        "  key_id            %c (%d) %s\r\n", \
        context_id[free_test_step], 'A' + key_id[free_test_step].id[0], key_id[free_test_step].id[0], EMPTY); \
    LOG_INFO_EX(BSL_LOG_MODULE, "  key_id            %c (%d) \r\n"    \
                                "  full_key_allocated %s %s\r\n",   \
                                (key_id[free_test_step].id[1] == DNX_FIELD_KEY_ID_INVALID)? '-' : 'A' + key_id[free_test_step].id[1],\
                                 key_id[free_test_step].id[1],   \
                                (key_id[alloc_test_step].full_key_allocated)?("yes"):("no"), EMPTY); \
}
/*
 * }
 */
/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */
/**
 * Enum for the various 'key' tests
 * See 'ctest_dnx_field_key.c'
 */
static sh_sand_enum_t Field_key_test_type_enum_table[] = {
    {"KEY", CTEST_DNX_FIELD_KEY_TEST_TYPE_KEY},
    {"FFC", CTEST_DNX_FIELD_KEY_TEST_TYPE_FFC},
    {"FFC-2", CTEST_DNX_FIELD_KEY_TEST_TYPE_FFC_PER_MULTIPLE_PROGRAMS},
    {"BIT-RANGE", CTEST_DNX_FIELD_KEY_TEST_TYPE_BIT_RANGE},
    {"FFC-SHARED", CTEST_DNX_FIELD_KEY_TEST_TYPE_SHARED_FFC},

    {NULL}
};

/**
 * \brief
 * Enum for option minimal and maximal value of context id
 * that are used for active testing (only part of the contexts
 * are used because some are captured for default applications
 * such as ITMH_PPH).
 */
static sh_sand_enum_t Field_context_enum_table_for_test[] = {
    {"CONTEXT_LOWEST", 20},
    {"CONTEXT_HIGHEST", 40},
    {NULL}
};

/**
 * \brief
 *   Options list for 'key' shell command
 * \remark
 *   Set the default value such that no test is carried out but procedure does return with 'success'
 *   This ensures success on regression and pre-commit:
 *   When sh_dnx_key_cmd() is automatically run in regression, it gets no options and, so, the
 *   'default' will go into effect and the procedure will not fail.
 */
sh_sand_option_t Dnx_field_key_test_options[] = {
    /* Name */                                 /* Type */                 /* Description */                             /* Default */                       /* Enum table */
    {DNX_DIAG_KEY_OPTION_TEST_TYPE,        SAL_FIELD_TYPE_ENUM,    "Type of test (key/ffc/ffc-2/bit-range/ffc-shared)",NULL,                              (void *)Field_key_test_type_enum_table },
    {DNX_DIAG_KEY_OPTION_TEST_STAGE,       SAL_FIELD_TYPE_ENUM,    "Stage of test (ipmf1,ipmf2,ipmf3,epmf,...)",       "ipmf1",                           (void *)Field_stage_enum_table},
    {DNX_DIAG_KEY_OPTION_TEST_GROUP_TYPE,  SAL_FIELD_TYPE_ENUM,    "group_type to allocate/free bit-range for",        "DIRECT_EXTRACTION",               (void *)Group_type_enum_table},
    {DNX_DIAG_KEY_OPTION_TEST_CONTEXT,     SAL_FIELD_TYPE_UINT32,  "context to get key info for or to alloc bit-range", "CONTEXT_LOWEST-CONTEXT_HIGHEST", (void *)Field_context_enum_table_for_test},
    {DNX_DIAG_KEY_OPTION_TEST_NUM_BITS,    SAL_FIELD_TYPE_UINT32,  "Number of bits to allocate for bit-range",          _STRINGIFY(DNX_FIELD_KEY_BIT_RANGE_MIN_LENGTH)},
    {DNX_DIAG_KEY_OPTION_TEST_COUNT,       SAL_FIELD_TYPE_UINT32,  "Number of times test will run",                     "1"},
    {DNX_DIAG_KEY_OPTION_TEST_ALLOCATE,    SAL_FIELD_TYPE_BOOL,    "Bit-range 'allocation' (yes) or 'free' (no)",       "Yes"},
    {DNX_DIAG_KEY_OPTION_CLEAN,            SAL_FIELD_TYPE_BOOL,    "Perform resource clean-up (yes) or not (no)",       "Yes"},
    {DNX_DIAG_KEY_OPTION_TEST_OFFEST,      SAL_FIELD_TYPE_INT32,   "Offset of bit-range (within key) to free"
                                                                   "or offset of bit (within bit-range) to align",      "0"},
    {DNX_DIAG_KEY_OPTION_TEST_KEY,         SAL_FIELD_TYPE_ENUM,    "Key (containing bit-range to free)",                "A",(void *)Field_key_enum_table},
    {DNX_DIAG_KEY_OPTION_TEST_ALIGN,       SAL_FIELD_TYPE_BOOL,    "Align allocated bits (yes) or 'free' (no)",         "No"},

    { NULL }
    /** End of options list - must be last. */
};

/**
 *  'Help' description for KEY/FFC tests.
 */
sh_sand_man_t Sh_dnx_field_key_test_man = {
    "'Key' related test utilities"
    ,
    "Activate 'key' related test utilities. This covers key-level tests, ffc-level tests and bit-range tests.\r\n"
    "For both key-level tests and ffc-level tests,\r\n"
    "- If no stage is specified then 'ipmf1' is assumed.\r\n"
    "- If no context_range is specified then 'minimal val'-'maximal val' is assumed (32-63).\r\n"
    "  If only one value is specified for context_range then this 'single value' range is assumed.\r\n"
    "For bit-range tests, the following are default or expected parameters:\r\n"
    "- A single context (If not specified, or a range is specified, then CTEST_DNX_FIELD_KEY_DEFAULT_CONTEXT_FOR_BIT_RANGE is selected).\r\n"
    "- A 'field group type' (If not specified then DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION is selected).\r\n"
    "- A 'field stage' (If not specified then 'ipmf2' is selected).\r\n"
    "- The number of bits to allocate (If not specified then 'DNX_FIELD_KEY_BIT_RANGE_MIN_LENGTH' is selected). (Allocate only)\r\n"
    "- An indication on whether to 'allocate' or to 'free'\r\n"
    "- The offset at which to free ('free' operation only)\r\n"
    "- The offset at which to align ('allocate' operation only)\r\n"
    "- Indication on whether to align ('allocate' operation only)\r\n"
    "ctest field key type=<key | ffc | bit-range | ffc-shared> stage=<ipmf1 |ipmf2 |ipmf3 | epmf | ...> group_type=<TCAM | DIRECT_EXTRACTION | ...> context=<<low>-<high>> num_bits=<integer> clean=<YES | NO>",
    "type=ffc\r\n" "TY=KEY stage=ipmf3\r\n"
    "type=bit-range stage=ipmf2 context=2 group_type=DIRECT_EXTRACTION num_bits=5\r\n",
};

/**
 * \brief
 *   List of tests for 'key' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_key_tests[] = {
    {"DNX_key_create_ipmf1", "type=FFC  stage=ipmf1 count=6", CTEST_POSTCOMMIT},
    {"DNX_key_create_ipmf2", "type=FFC  stage=ipmf2 count=6", CTEST_POSTCOMMIT},
    {"DNX_key_create_ipmf3", "type=FFC  stage=ipmf3 count=6", CTEST_POSTCOMMIT},
    {"DNX_key_create_epmf",  "type=FFC  stage=epmf count=6",  CTEST_POSTCOMMIT},
    {"DNX_key_alloc_ipmf1",  "type=KEY  stage=ipmf1 count=6", CTEST_POSTCOMMIT},
    {"DNX_key_alloc_ipmf2",  "type=KEY  stage=ipmf2 count=6", CTEST_POSTCOMMIT},
    {"DNX_key_alloc_ipmf3",  "type=KEY  stage=ipmf3 count=6", CTEST_POSTCOMMIT},
    {"DNX_key_alloc_epmf",   "type=KEY  stage=epmf count=6", CTEST_POSTCOMMIT},
    {"DNX_key_shared_initial_ffc","type=FFC-SHARED  stage=ipmf2 group_type=tcam count=6", CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */

#if (0)
/*{ */


/**
 * \brief
 *  This diagnostics function will set the array of qualifiers for internal testing.
 *  the array is adjustable for 80b/160b/320b key size.
 * \param [in] unit               - Device ID
 *  param [in] key_size           - Key Size
 *  param [out] qual_type         - Array of qualifiers, according to key_size
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
diag_dnx_field_key_fill_quals(
    int unit,
    int key_size,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t ** qual_type)
{
    int i_qual;
    SHR_FUNC_INIT_VARS(unit);

    for (i_qual = 0; i_qual < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; i_qual++)
    {
        *qual_type[i_qual] = DNX_FIELD_QUAL_TYPE_INVALID;
    }

    switch (key_size)
    {
            /*
             * Single TCAM key size or Half TCAM
             */
        case DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF:
            
        case DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE:
        {
            
            if (field_stage != DNX_FIELD_STAGE_IPMF3)
            {
                *qual_type[0] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_IPV4_SRC);

                *qual_type[1] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_MAC_SRC);

                *qual_type[2] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_L4_SRC_PORT);
            }
            else
            {
                *qual_type[0] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_EEI);

                *qual_type[1] =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF3,
                             DNX_FIELD_IPMF3_QUAL_EGRESS_LEARN_ENABLE);

                *qual_type[2] =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FWD_PAYLOAD);
            }

            break;
        }
            /*
             * Double TCAM key size
             */
        case DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_DOUBLE:
        {
            LOG_CLI_EX(" 'double key size' testing is not implemented. Return with 'success' flag%s%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY, EMPTY);

            break;
        }
        default:
        {
            LOG_CLI_EX(" Invalid Key Size %d Not supported%s%s%s \r\n", key_size, EMPTY, EMPTY, EMPTY);

            break;
        }
    }

    SHR_FUNC_EXIT;
}
/* } */
#endif

/**
 * \brief
 *  This diagnostics function will test the key construction, ffc allocation
 *  and ffc hw_set. The test is per 80b/160b/320b key size, per multiple number of programs.
 * \param [in] unit               - Device ID
 * \param [in] field_stage        -
 *   dnx_field_stage_e. Stage to use for key and FFC testing
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * At exit, all attached context IDs are detached.
 * \see
 *   * None
 */

static shr_error_e
field_key_tester_key_plus_ffc(
    int unit,
    dnx_field_stage_e field_stage,
    sh_sand_control_t * sand_control)
{
    int context_idx;
    /*
     * 'context_idx_attached' is to be used as counter of the number of
     * Context IDs which have actually been attached.
     */
    int context_idx_attached;
    dnx_field_key_attach_info_in_t key_in_info;
    dnx_field_key_template_t key_template;
    dnx_field_key_id_t key_id[DNX_DIAG_KEY_CONTEXT_NOF];
    dnx_field_group_info_t field_group;
    dnx_field_context_t context_id[DNX_DIAG_KEY_CONTEXT_NOF] = { 7, 9 };
    dnx_field_qualifier_in_info_t user_pmf1_ipv4, user_pmf2_ipv4, user_pmf2_dest_port;
    dnx_field_context_mode_t ipmf1_context_mode;
    bcm_field_qualify_t bcm_qual_ipv4 = 2200;
    bcm_field_qualify_t bcm_qual_dport = 2201;
    int key_test_clean;
    dnx_field_stage_e stage_for_context_create = DNX_FIELD_STAGE_NOF;
    int rv = 0;

    SHR_FUNC_INIT_VARS(unit);
    key_test_clean = 0;
    context_idx_attached = 0;
    SH_SAND_GET_BOOL(DNX_DIAG_KEY_OPTION_CLEAN, key_test_clean);
    /*
     * Clear the dnx_field_fg_info_in_t, set it to preferred init values
     */
    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &field_group));
    /** For both iPMF1 and iPMF2, create an iPMF1/2 pair by creating an iPMF1 context.*/
    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        stage_for_context_create = DNX_FIELD_STAGE_IPMF1;
    }
    else
    {
        stage_for_context_create = field_stage;
    }
    for (context_idx = 0; context_idx < DNX_DIAG_KEY_CONTEXT_NOF; context_idx++)
    {
        dnx_field_key_id_t_init(unit, &(key_id[context_idx]));
        dnx_field_context_mode_t_init(unit, &ipmf1_context_mode);
        SHR_IF_ERR_EXIT(dnx_field_context_create
                        (unit, DNX_FIELD_CONTEXT_FLAG_WITH_ID, stage_for_context_create, &ipmf1_context_mode,
                         &context_id[context_idx]));
    }
    /** Field stage, for which stage the Database was created*/
    field_group.field_stage = field_stage;

    /*
     * Database type: The database may be:TCAM, Direct Table,
     * or Direct Extraction.
     */
    field_group.fg_type = DNX_FIELD_GROUP_TYPE_TCAM;

    /*
     * Types of the Qualifier fields for this database.
     */
    
/*    diag_dnx_field_key_fill_quals(unit, DNX_DATA_MAX_FIELD_TCAM_SINGLE_KEY_SIZE, field_stage, &(field_database.qual_types);*/
    SHR_IF_ERR_EXIT(dnx_field_key_attach_info_in_t_init(unit, &key_in_info));
    
    if ((field_stage == DNX_FIELD_STAGE_IPMF1) || (field_stage == DNX_FIELD_STAGE_EPMF))
    {
        field_group.dnx_quals[0] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_USER, field_stage, DNX_FIELD_HEADER_QUAL_IPV4_SRC);
        dnx_field_qual_in_info_init(unit, &user_pmf1_ipv4);

        sal_strncpy(user_pmf1_ipv4.name, "user_pmf1_ipv4", sizeof(user_pmf1_ipv4.name));
        user_pmf1_ipv4.size = 32;

        SHR_IF_ERR_EXIT(dnx_field_qual_create(unit, DNX_FIELD_QUALIFIER_FLAG_WITH_ID, &user_pmf1_ipv4, &bcm_qual_ipv4));
        SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, field_stage, bcm_qual_ipv4, &field_group.dnx_quals[0]));

        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(key_in_info.qual_info[0])));
        key_in_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
        key_in_info.qual_info[0].input_arg = 3;
        key_in_info.qual_info[0].offset = 0x111;

        field_group.dnx_quals[1] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_MAC_SRC);
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(key_in_info.qual_info[1])));
        key_in_info.qual_info[1].input_type = DNX_FIELD_INPUT_TYPE_LAYER_FWD;
        key_in_info.qual_info[1].input_arg = -3;
        key_in_info.qual_info[1].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;

        field_group.dnx_quals[2] =
            DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_L4_SRC_PORT);
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(key_in_info.qual_info[2])));
        key_in_info.qual_info[2].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
        key_in_info.qual_info[2].input_arg = 3;
        key_in_info.qual_info[2].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;

        key_in_info.key_length = DNX_FIELD_KEY_LENGTH_TYPE_SINGLE;

    }
    else if (field_stage == DNX_FIELD_STAGE_IPMF3)
    {
        field_group.dnx_quals[0] =
            DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_SNOOP_STRENGTH);
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(key_in_info.qual_info[0])));
        key_in_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
        key_in_info.qual_info[0].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;

        field_group.dnx_quals[1] =
            DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_RPF_DST_VALID);
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(key_in_info.qual_info[1])));
        key_in_info.qual_info[1].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
        key_in_info.qual_info[1].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;

        field_group.dnx_quals[2] =
            DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_RPF_DST);
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(key_in_info.qual_info[2])));
        key_in_info.qual_info[2].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
        key_in_info.qual_info[2].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;

        key_in_info.key_length = DNX_FIELD_KEY_LENGTH_TYPE_SINGLE;
    }
    else if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        field_group.dnx_quals[0] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_USER, field_stage, DNX_FIELD_HEADER_QUAL_IPV4_SRC);

        dnx_field_qual_in_info_init(unit, &user_pmf2_ipv4);

        sal_strncpy(user_pmf2_ipv4.name, "user_pmf2_ipv4", sizeof(user_pmf2_ipv4.name));
        user_pmf2_ipv4.size = 32;
        SHR_IF_ERR_EXIT(dnx_field_qual_create(unit, DNX_FIELD_QUALIFIER_FLAG_WITH_ID, &user_pmf2_ipv4, &bcm_qual_ipv4));
        SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx
                        (unit, DNX_FIELD_STAGE_IPMF2, bcm_qual_ipv4, &field_group.dnx_quals[0]));

        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(key_in_info.qual_info[0])));
        key_in_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
        key_in_info.qual_info[0].input_arg = 4;
        key_in_info.qual_info[0].offset = 5;

        field_group.dnx_quals[1] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_MAC_SRC);
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(key_in_info.qual_info[1])));
        key_in_info.qual_info[1].input_type = DNX_FIELD_INPUT_TYPE_LAYER_FWD;
        key_in_info.qual_info[1].input_arg = 2;
        key_in_info.qual_info[1].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;

        field_group.dnx_quals[2] =
            DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_L4_SRC_PORT);
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(key_in_info.qual_info[2])));
        key_in_info.qual_info[2].input_type = DNX_FIELD_INPUT_TYPE_LAYER_FWD;
        key_in_info.qual_info[2].input_arg = 3;
        key_in_info.qual_info[2].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;

        field_group.dnx_quals[3] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_IPV4_DST);
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(key_in_info.qual_info[3])));
        key_in_info.qual_info[3].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
        key_in_info.qual_info[3].input_arg = 4;
        key_in_info.qual_info[3].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;

        field_group.dnx_quals[4] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_MAC_DST);
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(key_in_info.qual_info[4])));
        key_in_info.qual_info[4].input_type = DNX_FIELD_INPUT_TYPE_LAYER_FWD;
        key_in_info.qual_info[4].input_arg = -2;
        key_in_info.qual_info[4].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;

        field_group.dnx_quals[5] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_USER, field_stage, DNX_FIELD_HEADER_QUAL_L4_DST_PORT);
        SHR_IF_ERR_EXIT(dnx_field_qual_in_info_init(unit, &user_pmf2_dest_port));

        sal_strncpy(user_pmf2_dest_port.name, "user_pmf2_dest_port", sizeof(user_pmf2_dest_port.name));
        user_pmf2_dest_port.size = 16;
        SHR_IF_ERR_EXIT(dnx_field_qual_create
                        (unit, DNX_FIELD_QUALIFIER_FLAG_WITH_ID, &user_pmf2_dest_port, &bcm_qual_dport));
        SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx
                        (unit, DNX_FIELD_STAGE_IPMF2, bcm_qual_dport, &field_group.dnx_quals[5]));
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(key_in_info.qual_info[5])));
        key_in_info.qual_info[5].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
        key_in_info.qual_info[5].input_arg = 3;
        key_in_info.qual_info[5].offset = -10;

        field_group.dnx_quals[6] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_IPV4_DST);
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(key_in_info.qual_info[6])));
        key_in_info.qual_info[6].input_type = DNX_FIELD_INPUT_TYPE_LAYER_FWD;
        key_in_info.qual_info[6].input_arg = -1;
        key_in_info.qual_info[6].offset = 0;

        field_group.dnx_quals[7] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_MAC_DST);
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(key_in_info.qual_info[7])));
        key_in_info.qual_info[7].input_type = DNX_FIELD_INPUT_TYPE_LAYER_FWD;
        key_in_info.qual_info[7].input_arg = 2;
        key_in_info.qual_info[7].offset = 0;

        key_in_info.key_length = DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE;
    }
    /*
     * Action types applied by the database,For PMF1. Select the list of actions to be applied to matching entries. can
     * Hold any of the following types depends on database stage: dbal_enum_value_field_field_pmf_a_action_e
     * dbal_enum_value_field_field_pmf_b_action_e dbal_enum_value_field_field_e_pmf_action_e Number of meaningful
     * elements on this array: All elements until DNX_FIELD_ACTION__INVALID is encountered.
     */
    field_group.dnx_actions[0] = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_IPMF1_ACTION_DP);

    /*
     * Init Key template
     * */
    SHR_IF_ERR_EXIT(dnx_field_key_template_t_init(unit, &key_template));

    /*
     * Create Key template
     * */
    SHR_IF_ERR_EXIT(dnx_field_key_template_create
                    (unit, field_group.field_stage, field_group.dnx_quals, DNX_FIELD_KEY_LOOKUP_STARTING_OFFSET,
                     &key_template));

    key_in_info.field_stage = field_group.field_stage;

    key_in_info.fg_type = field_group.fg_type;

    /*
     * TBD: Do testing for every size
     */

    key_in_info.app_db_id = 5;
    key_in_info.compare_id = DNX_FIELD_GROUP_COMPARE_ID_NONE;

    sal_memcpy(&key_in_info.key_template, &key_template, sizeof(key_in_info.key_template));

    /** Attach performed per context ID*/
    for (context_idx = 0; context_idx < DNX_DIAG_KEY_CONTEXT_NOF; context_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_attach
                        (unit, context_id[context_idx], &key_in_info, NULL, &(key_id[context_idx])));
        /*
         * Count successful 'attach'es
         */
        context_idx_attached++;
    }

    /*
     * Free all allocated resources
     */
exit:
/*
 * Do not use SHR_IF_ERR_EXIT inside the exit label. in case of failure this will cause an infinite loop.
 * */
    if (key_test_clean == TRUE)
    {
        /*
         * Detach performed per context ID which have been attached
         */
        for (context_idx = 0; context_idx < DNX_DIAG_KEY_CONTEXT_NOF; context_idx++)
        {
            CTEST_DNX_FIELD_UTIL_ERR(dnx_field_key_detach
                                     (unit, context_id[context_idx], &key_in_info, &key_id[context_idx]));
            CTEST_DNX_FIELD_UTIL_ERR(dnx_field_context_destroy
                                     (unit, stage_for_context_create, context_id[context_idx]));
        }
        if ((field_stage == DNX_FIELD_STAGE_IPMF1) || (field_stage == DNX_FIELD_STAGE_EPMF))
        {
            CTEST_DNX_FIELD_UTIL_ERR(dnx_field_qual_destroy(unit, bcm_qual_ipv4));
        }
        else if (field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            CTEST_DNX_FIELD_UTIL_ERR(dnx_field_qual_destroy(unit, bcm_qual_ipv4));
            CTEST_DNX_FIELD_UTIL_ERR(dnx_field_qual_destroy(unit, bcm_qual_dport));
        }
        SHR_IF_ERR_RETURN(rv);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This diagnostics function will display key allocation state, as a PRT table,
 *  for specified context and stage.
 *  Display table only if severity level attached to current module (BSL_LOG_MODULE) is
 *  BSL_INFO or higher (in the direction of BSL_FATAL)
 * \param [in] unit               - Device ID
 * \param [in] field_stage        -
 *   dnx_field_stage_e. Stage to use for key occupation display
 * \param [in] context_id            -
 *   dnx_field_context_t. Context id (program) to use for key occupation display
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * Output of this procedure is keys occupation as stored on SW state.
 * \see
 *   * None
 */
static shr_error_e
field_key_display_key_occupation_info(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If current severity is higher than 'info' then display nothing.
     */
    if (bsl_fast_check(BSL_LOG_MODULE | BSL_INFO))
    {
        int cmd_rv;
        char cmd[500] = { 0 };
        sal_snprintf(cmd, sizeof(cmd), "FieLD KEY INFo type=by_half_key context=%d stage=%s",
                     context_id, dnx_field_stage_text(unit, field_stage));
        cmd_rv = sh_process_command(unit, cmd);
        if (cmd_rv != CMD_OK)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error %d in sh_process_command.\r\n", cmd_rv);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This diagnostics function will test the bit-range allocation (within key). The test is
 *  per key size, per program (context), per stage and per field group type.
 * \param [in] unit               - Device ID
 * \param [in] field_stage        -
 *   Stage to use for the test (allocate,free bit range)
 * \param [in] context_lower      -
 *   Lowest value of context range. If not equal to 'context_upper' then ignore it and use
 *   the default context (CTEST_DNX_FIELD_KEY_DEFAULT_CONTEXT_FOR_BIT_RANGE)
 * \param [in] context_upper      -
 *   See 'context_lower' above.
 * \param [in] fg_type        -
 *   Field group type to use for the test (allocate,free bit range)
 * \param [in] num_bits_in_bit_range        -
 *   Number of bits on range which is to be used for the test (allocate,free bit range)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
field_key_tester_bit_range(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_lower,
    dnx_field_context_t context_upper,
    dnx_field_group_type_e fg_type,
    uint8 num_bits_in_bit_range,
    sh_sand_control_t * sand_control)
{
    int key_test_allocate;
    uint32 bit_range_offset_within_key;
    uint32 do_align;
    int32 bit_to_align;
    dnx_field_key_id_t key_id;
    dnx_algo_field_key_bit_range_align_t bit_range_alignment_info;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_BOOL(DNX_DIAG_KEY_OPTION_TEST_ALLOCATE, key_test_allocate);
    if (context_upper != context_lower)
    {
        context_lower = CTEST_DNX_FIELD_KEY_DEFAULT_CONTEXT_FOR_BIT_RANGE;
    }
    key_id.id[0] = key_id.id[1] = DBAL_NOF_ENUM_FIELD_KEY_VALUES;
    if (key_test_allocate)
    {
        /*
         * Carry out 'allocate' operation'
         */
        SH_SAND_GET_INT32(DNX_DIAG_KEY_OPTION_TEST_OFFEST, bit_to_align);
        SH_SAND_GET_BOOL(DNX_DIAG_KEY_OPTION_TEST_ALIGN, do_align);
        sal_memset(&bit_range_alignment_info, 0x0, sizeof(bit_range_alignment_info));
        bit_range_alignment_info.align_lsb = do_align;
        bit_range_alignment_info.lsb_aligned_bit = bit_to_align;
        LOG_CLI_EX("Bit range allocation: Try to allocate %d bits for stage %s, fg_type %s context %d\r\n",
                   (int) num_bits_in_bit_range, dnx_field_stage_e_get_name(field_stage),
                   dnx_field_group_type_e_get_name(fg_type), (int) context_lower);
        SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_allocate
                        (unit, field_stage, fg_type, context_lower, num_bits_in_bit_range, &bit_range_alignment_info,
                         FALSE, &key_id, &bit_range_offset_within_key));
        LOG_CLI_EX("Bit range allocation: Allocated %d bits at offset %d of key '%c' %s\r\n",
                   (int) num_bits_in_bit_range, (int) bit_range_offset_within_key, 'A' + key_id.id[0], EMPTY);
    }
    else
    {
        /*
         * Carry out 'free' operation'
         */
        uint32 bit_range_offset_within_key;

        SH_SAND_GET_INT32(DNX_DIAG_KEY_OPTION_TEST_OFFEST, bit_range_offset_within_key);
        SH_SAND_GET_ENUM(DNX_DIAG_KEY_OPTION_TEST_KEY, key_id.id[0]);
        LOG_CLI_EX("Bit range free: Try to free key %c at offset %d. for stage %s, fg_type %s",
                   ('A' + key_id.id[0]), bit_range_offset_within_key, dnx_field_stage_e_get_name(field_stage),
                   dnx_field_group_type_e_get_name(fg_type));
        LOG_CLI_EX(" context %d %s%s%s\r\n", (int) context_lower, EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_free
                        (unit, field_stage, fg_type, context_lower, key_id, bit_range_offset_within_key));
        LOG_CLI_EX("Bit range free: Completed 'free' of key '%c' at offset %d %s%s\r\n", 'A' + key_id.id[0],
                   bit_range_offset_within_key, EMPTY, EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_fg_add(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    dnx_field_action_t dnx_action,
    dnx_field_group_t * fg_id_p)
{
    dnx_field_group_info_t fg_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &fg_info));
    fg_info.field_stage = field_stage;
    fg_info.fg_type = fg_type;

    snprintf(fg_info.name, sizeof(fg_info.name), "SHARE_FFC_FG_%d", *fg_id_p);

    fg_info.dnx_quals[0] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, field_stage, DNX_FIELD_IPMF1_QUAL_DP);
    fg_info.dnx_quals[1] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_MAC_SRC);
    fg_info.dnx_quals[2] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_MAC_DST);
    fg_info.dnx_quals[3] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_IPV6_SIP);

    /*
     *  Create a void user defined function
     */

    fg_info.dnx_actions[0] = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, dnx_action);

    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, 0 /* DNX_FIELD_GROUP_ADD_FLAG_WITH_ID */ , &fg_info, fg_id_p));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_fg_attach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id)
{
    uint32 qual_ndx, action_ndx;
    dnx_field_group_attach_info_t group_attach_info;
    dnx_field_key_template_t key_template;
    dnx_field_actions_fg_payload_sw_info_t actions_payload_info;

    SHR_FUNC_INIT_VARS(unit);

    dnx_field_group_attach_info_t_init(unit, &group_attach_info);
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.key_template.get(unit, fg_id, &(key_template)));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.actions_payload_info.get(unit, fg_id, &(actions_payload_info)));

    /*
     * Build the attach information
     */

    for (qual_ndx = 0; qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_ndx++)
    {
        group_attach_info.dnx_quals[qual_ndx] = key_template.key_qual_map[qual_ndx].qual_type;
    }

    for (action_ndx = 0; action_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG; action_ndx++)
    {
        group_attach_info.dnx_actions[action_ndx] = actions_payload_info.actions_on_payload_info[action_ndx].dnx_action;
        group_attach_info.action_info[action_ndx].priority = BCM_FIELD_ACTION_PRIORITY(action_ndx, fg_id);
    }

    /*
     * Build the qualifier attach information
     */
    group_attach_info.qual_info[0].input_arg = 0;
    group_attach_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
    group_attach_info.qual_info[0].offset = 0;

    group_attach_info.qual_info[1].input_arg = 2;
    group_attach_info.qual_info[1].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    group_attach_info.qual_info[1].offset = 0xf;

    group_attach_info.qual_info[2].input_arg = 2;
    group_attach_info.qual_info[2].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    group_attach_info.qual_info[2].offset = 0xf;

    group_attach_info.qual_info[3].input_arg = 2;
    group_attach_info.qual_info[3].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    group_attach_info.qual_info[3].offset = 0xf;

    /*
     * Call the attach API
     */
    SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, fg_id, context_id, &group_attach_info));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_fg_context_create(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t * context_id_p)
{
    dnx_field_context_mode_t context_mode;
    dnx_field_stage_e ctx_create_field_stage = field_stage;

    SHR_FUNC_INIT_VARS(unit);

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
        ctx_create_field_stage = DNX_FIELD_STAGE_IPMF1;

    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
    context_mode.context_ipmf1_mode.compare_mode_1 = DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE;

    SHR_IF_ERR_EXIT(dnx_field_context_create
                    (unit, 0 /* DNX_FIELD_CONTEXT_FLAG_WITH_ID */ , ctx_create_field_stage, &context_mode,
                     context_id_p));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
field_key_tester_shared_ffc(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t * context_p,
    dnx_field_group_t * fg_id_1_p,
    dnx_field_group_t * fg_id_2_p,
    dnx_field_group_type_e fg_type)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * serves as index in the name of the fg. on this stage does not represent real fg_id
     */
    SHR_IF_ERR_EXIT(appl_dnx_fg_context_create(unit, field_stage, context_p));
    LOG_CLI_EX("Created context %d %s%s%s\r\n", *context_p, EMPTY, EMPTY, EMPTY);

    /*
     * Create a field group 
     */
    SHR_IF_ERR_EXIT(appl_dnx_fg_add(unit, field_stage, fg_type, DBAL_ENUM_FVAL_IPMF1_ACTION_DP, fg_id_1_p));
    LOG_CLI_EX("Created fg %d %s%s%s\r\n", *fg_id_1_p, EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(appl_dnx_fg_add(unit, field_stage, fg_type, DBAL_ENUM_FVAL_IPMF1_ACTION_TC, fg_id_2_p));
    LOG_CLI_EX("Created fg %d %s%s%s\r\n", *fg_id_2_p, EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(appl_dnx_fg_attach(unit, *fg_id_1_p, *context_p));
    LOG_CLI_EX("Attached fg %d to context %d %s%s\r\n", *fg_id_1_p, *context_p, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(appl_dnx_fg_attach(unit, *fg_id_2_p, *context_p));
    LOG_CLI_EX("Attached fg %d to context %d %s%s\r\n", *fg_id_2_p, *context_p, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
field_key_tester_shared_ffc_group_cleanup(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context,
    dnx_field_group_t fg_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_context_detach(unit, fg_id, context));
    LOG_CLI_EX("Detached fg %d from context %d %s%s\r\n", fg_id, context, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_group_delete(unit, fg_id));
    LOG_CLI_EX("Deleted fg %d %s%s%s\r\n", fg_id, EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
field_key_tester_shared_ffc_cleanup(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context,
    dnx_field_group_t fg_id_1,
    dnx_field_group_t fg_id_2)
{
    dnx_field_stage_e ctx_destroy_field_stage = field_stage;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(field_key_tester_shared_ffc_group_cleanup(unit, field_stage, context, fg_id_1));

    SHR_IF_ERR_EXIT(field_key_tester_shared_ffc_group_cleanup(unit, field_stage, context, fg_id_2));

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
        ctx_destroy_field_stage = DNX_FIELD_STAGE_IPMF1;

    SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, ctx_destroy_field_stage, context));
    LOG_CLI_EX("Destroyed context %d %s%s%s\r\n", context, EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This diagnostics function will test the key allocation. The test is
 *  per 80b/160b/320b key size, per multiple number of programs per
 *  field group type.
 * \param [in] unit               - Device ID
 * \param [in] field_stage        - Stage to use within the test
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
field_key_tester_key(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_lower,
    dnx_field_context_t context_upper,
    sh_sand_control_t * sand_control)
{

    dnx_field_group_type_e fg_type[CTEST_DNX_FIELD_KEY_NUM_ALLOCATIONS_IN_KEY_TEST];
    dnx_field_context_t context_id[CTEST_DNX_FIELD_KEY_NUM_ALLOCATIONS_IN_KEY_TEST];
    dnx_field_key_length_type_e key_length[CTEST_DNX_FIELD_KEY_NUM_ALLOCATIONS_IN_KEY_TEST];
    dnx_field_key_id_t key_id[CTEST_DNX_FIELD_KEY_NUM_ALLOCATIONS_IN_KEY_TEST];
    int key_length_bits[CTEST_DNX_FIELD_KEY_NUM_ALLOCATIONS_IN_KEY_TEST];
    uint32 key_offset_bits[CTEST_DNX_FIELD_KEY_NUM_ALLOCATIONS_IN_KEY_TEST];
    unsigned int alloc_test_step;
    unsigned int free_test_step;
    int key_test_clean;
    dnx_field_context_t context_index;
    int key_idx;
    dnx_field_context_mode_t ipmf1_context_mode;
    dnx_field_stage_e stage_for_context_create = DNX_FIELD_STAGE_NOF;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_BOOL(DNX_DIAG_KEY_OPTION_CLEAN, key_test_clean);

    for (key_idx = 0; key_idx < CTEST_DNX_FIELD_KEY_NUM_ALLOCATIONS_IN_KEY_TEST; key_idx++)
    {
        dnx_field_key_id_t_init(unit, &key_id[key_idx]);
    }
    LOG_INFO_EX(BSL_LOG_MODULE, "Testing KEY-ALLOC and KEY-FREE %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    for (context_index = context_lower; context_index <= context_upper; context_index++)
    {
        dnx_field_context_mode_t_init(unit, &ipmf1_context_mode);
        if (context_index != DNX_FIELD_CONTEXT_ID_DEFAULT(unit))
        {
            /** For both iPMF1 and iPMF2, create an iPMF1/2 pair by creating an iPMF1 context.*/
            if (field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                stage_for_context_create = DNX_FIELD_STAGE_IPMF1;
            }
            else
            {
                stage_for_context_create = field_stage;
            }
            SHR_IF_ERR_EXIT(dnx_field_context_create
                            (unit, DNX_FIELD_CONTEXT_FLAG_WITH_ID, stage_for_context_create, &ipmf1_context_mode,
                             &context_index));
        }

        
        {
            /*
             * Allocate first key. Half.
             */
            LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 1 **************************************%s%s%s%s\r\n", EMPTY,
                        EMPTY, EMPTY, EMPTY);
            alloc_test_step = 0;
            fg_type[alloc_test_step] = DNX_FIELD_GROUP_TYPE_TCAM;
            context_id[alloc_test_step] = context_index;
            key_length[alloc_test_step] = DNX_FIELD_KEY_LENGTH_TYPE_HALF;

            DNX_DIAG_KEY_DISPLAY_ALLOC_PARAMS("Key allocation request - half key for TCAM:\r\n");

            SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate
                            (unit, 0, field_stage, fg_type[alloc_test_step], context_id[alloc_test_step],
                             key_length[alloc_test_step], &key_id[alloc_test_step]));

            DNX_DIAG_KEY_DISPLAY_ALLOC_RESULTS();
        }
        {
            /*
             * Allocate second key. Half.
             */
            LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 2 ************************************** %s%s%s%s\r\n", EMPTY,
                        EMPTY, EMPTY, EMPTY);
            alloc_test_step = 1;
            fg_type[alloc_test_step] = DNX_FIELD_GROUP_TYPE_TCAM;
            context_id[alloc_test_step] = context_index;
            key_length[alloc_test_step] = DNX_FIELD_KEY_LENGTH_TYPE_HALF;

            DNX_DIAG_KEY_DISPLAY_ALLOC_PARAMS("Key allocation request - half key for TCAM:\r\n");

            SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate
                            (unit, 0, field_stage, fg_type[alloc_test_step], context_id[alloc_test_step],
                             key_length[alloc_test_step], &key_id[alloc_test_step]));

            DNX_DIAG_KEY_DISPLAY_ALLOC_RESULTS();
        }
        {
            /*
             * Allocate third key. Single. Exem.
             */
            LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 3 ************************************** %s%s%s%s\r\n", EMPTY,
                        EMPTY, EMPTY, EMPTY);
            alloc_test_step = 2;
            fg_type[alloc_test_step] = DNX_FIELD_GROUP_TYPE_EXEM;
            context_id[alloc_test_step] = context_index;
            key_length[alloc_test_step] = DNX_FIELD_KEY_LENGTH_TYPE_SINGLE;

            DNX_DIAG_KEY_DISPLAY_ALLOC_PARAMS("Key allocation request - single key for EXEM:\r\n");

            SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate
                            (unit, 0, field_stage, fg_type[alloc_test_step], context_id[alloc_test_step],
                             key_length[alloc_test_step], &key_id[alloc_test_step]));

            DNX_DIAG_KEY_DISPLAY_ALLOC_RESULTS();
        }
        SHR_IF_ERR_EXIT(field_key_display_key_occupation_info(unit, field_stage, context_id[alloc_test_step]));
        /*
         * When cleaning resources is not applied then quit after this batch because there are not enough keys for
         * other batches.
         */
        if (key_test_clean == TRUE)
        {
            {
                /*
                 * Free first key. Half.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 4 **************************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
                free_test_step = 0;

                DNX_DIAG_KEY_DISPLAY_FREE_PARAMS("Key free request - First-allocated half key:\r\n");

                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free
                                (unit, fg_type[free_test_step], field_stage, context_id[free_test_step],
                                 &(key_id[free_test_step])));
            }
            {
                /*
                 * Free second key. Half.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 5 **************************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
                free_test_step = 1;

                DNX_DIAG_KEY_DISPLAY_FREE_PARAMS("Key free request - Second-allocated half key:\r\n");

                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free
                                (unit, fg_type[free_test_step], field_stage, context_id[free_test_step],
                                 &(key_id[free_test_step])));
            }
            {
                /*
                 * Free third key. Single.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 6 **************************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
                free_test_step = 2;

                DNX_DIAG_KEY_DISPLAY_FREE_PARAMS("Key free request - Third-allocated single key:\r\n");

                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free
                                (unit, fg_type[free_test_step], field_stage, context_id[free_test_step],
                                 &(key_id[free_test_step])));
            }
            SHR_IF_ERR_EXIT(field_key_display_key_occupation_info(unit, field_stage, context_id[alloc_test_step]));
            /*
             * All keys are now free.
             */
            LOG_INFO_EX(BSL_LOG_MODULE, "Key test. ALL keys are FREE now. **************************%s%s%s%s\r\n",
                        EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else
        {
            continue;
        }
        /*
         * The following sequence involves DIRECT EXTRACTION so do not execute for IPMF1 or EPMF.
         */
        if ((field_stage != DNX_FIELD_STAGE_IPMF1) && (field_stage != DNX_FIELD_STAGE_EPMF))
        {
            {
                /*
                 * Allocate first key. Half. DE. Force MSB
                 */
                /*
                 * Test originally designed for allocation that was allocating MSB half first.
                 * To accomodate this, we force allocate the MSB half.
                 */
                dnx_algo_field_key_bit_range_align_t align_info;
                uint8 first_available_key;
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 7 **************************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
                alloc_test_step = 0;
                fg_type[alloc_test_step] = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
                context_id[alloc_test_step] = context_index;
                key_length_bits[alloc_test_step] = 80;

                sal_memset(&align_info, 0x0, sizeof(align_info));
                align_info.align_lsb = FALSE;
                align_info.align_msb = FALSE;

                key_offset_bits[alloc_test_step] = 80;

                SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                                first_available_key.get(unit, field_stage, fg_type[alloc_test_step],
                                                        &first_available_key));

                SHR_IF_ERR_EXIT(dnx_field_key_id_t_init(unit, &(key_id[alloc_test_step])));
                key_id[alloc_test_step].id[0] = first_available_key;
                key_id[alloc_test_step].bit_range.bit_range_valid = TRUE;
                key_id[alloc_test_step].bit_range.bit_range_offset = key_offset_bits[alloc_test_step];
                key_id[alloc_test_step].bit_range.bit_range_size = key_length_bits[alloc_test_step];

                LOG_INFO_EX(BSL_LOG_MODULE, "Key allocation request - Half key for direct extraction:\r\n"
                            "  field_stage       %s (%d)\r\n"
                            "  fg_type           %s (%d)\r\n",
                            dnx_field_stage_e_get_name(field_stage), field_stage,
                            dnx_field_group_type_e_get_name(fg_type[alloc_test_step]), fg_type[alloc_test_step]);
                LOG_INFO_EX(BSL_LOG_MODULE,
                            "  context_id        %d\r\n"
                            "  key_offset_bits   %d %s%s\r\n",
                            context_id[alloc_test_step], key_offset_bits[alloc_test_step], EMPTY, EMPTY);

                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_allocate
                                (unit, field_stage, fg_type[alloc_test_step], context_id[alloc_test_step],
                                 key_length_bits[alloc_test_step], &align_info, TRUE, &key_id[alloc_test_step],
                                 &key_offset_bits[alloc_test_step]));

                LOG_INFO_EX(BSL_LOG_MODULE, "Result of allocation request:\r\n"
                            "  key_id_0          %c (%d)\r\n"
                            "  key_id_1          %c (%d)\r\n",
                            'A' + key_id[alloc_test_step].id[0], key_id[alloc_test_step].id[0],
                            (key_id[alloc_test_step].id[1] ==
                             DNX_FIELD_KEY_ID_INVALID) ? '-' : 'A' + key_id[alloc_test_step].id[1],
                            key_id[alloc_test_step].id[1]);
                LOG_INFO_EX(BSL_LOG_MODULE, "  key_offset_bits   %d%s%s%s\r\n", key_offset_bits[alloc_test_step], EMPTY,
                            EMPTY, EMPTY);
            }
            {
                /*
                 * Allocate second key. Single.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 8 **************************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
                alloc_test_step = 1;
                fg_type[alloc_test_step] = DNX_FIELD_GROUP_TYPE_TCAM;
                context_id[alloc_test_step] = context_index;
                key_length[alloc_test_step] = DNX_FIELD_KEY_LENGTH_TYPE_SINGLE;

                DNX_DIAG_KEY_DISPLAY_ALLOC_PARAMS("Key allocation request - Single key for TCAM:\r\n");

                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate
                                (unit, 0, field_stage, fg_type[alloc_test_step], context_id[alloc_test_step],
                                 key_length[alloc_test_step], &key_id[alloc_test_step]));

                DNX_DIAG_KEY_DISPLAY_ALLOC_RESULTS();
            }
            {
                /*
                 * Allocate third key. Half.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 9 **************************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
                alloc_test_step = 2;
                fg_type[alloc_test_step] = DNX_FIELD_GROUP_TYPE_TCAM;
                context_id[alloc_test_step] = context_index;
                key_length[alloc_test_step] = DNX_FIELD_KEY_LENGTH_TYPE_HALF;

                DNX_DIAG_KEY_DISPLAY_ALLOC_PARAMS("Key allocation request - Half key for TCAM:\r\n");

                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate
                                (unit, 0, field_stage, fg_type[alloc_test_step], context_id[alloc_test_step],
                                 key_length[alloc_test_step], &key_id[alloc_test_step]));

                DNX_DIAG_KEY_DISPLAY_ALLOC_RESULTS();
            }
            {
                /*
                 * Allocate fourth key. Single. Exem.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 10 **************************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
                alloc_test_step = 3;
                fg_type[alloc_test_step] = DNX_FIELD_GROUP_TYPE_EXEM;
                context_id[alloc_test_step] = context_index;
                key_length[alloc_test_step] = DNX_FIELD_KEY_LENGTH_TYPE_SINGLE;

                DNX_DIAG_KEY_DISPLAY_ALLOC_PARAMS("Key allocation request - Single key for EXEM:\r\n");

                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate
                                (unit, 0, field_stage, fg_type[alloc_test_step], context_id[alloc_test_step],
                                 key_length[alloc_test_step], &key_id[alloc_test_step]));

                DNX_DIAG_KEY_DISPLAY_ALLOC_RESULTS();
            }
            SHR_IF_ERR_EXIT(field_key_display_key_occupation_info(unit, field_stage, context_id[alloc_test_step]));
            {
                {
                    /*
                     * Free second key. Single.
                     */
                    LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 11 **************************************%s%s%s%s\r\n",
                                EMPTY, EMPTY, EMPTY, EMPTY);
                    free_test_step = 1;

                    DNX_DIAG_KEY_DISPLAY_FREE_PARAMS("Key free request - Second allocated single key:\r\n");

                    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free
                                    (unit, fg_type[free_test_step], field_stage, context_id[free_test_step],
                                     &(key_id[free_test_step])));
                }
                {
                    /*
                     * Free fourth key. Single. Exem.
                     */
                    LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 12 **************************************%s%s%s%s\r\n",
                                EMPTY, EMPTY, EMPTY, EMPTY);
                    free_test_step = 3;

                    DNX_DIAG_KEY_DISPLAY_FREE_PARAMS("Key free request - Fourth allocated single key:\r\n");

                    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free
                                    (unit, fg_type[free_test_step], field_stage, context_id[free_test_step],
                                     &(key_id[free_test_step])));
                }
                {
                    /*
                     * Free first key. Half. DE.
                     */
                    LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 13 **************************************%s%s%s%s\r\n",
                                EMPTY, EMPTY, EMPTY, EMPTY);
                    free_test_step = 0;

                    LOG_INFO_EX(BSL_LOG_MODULE, "Key free request - First allocated half key (DIR EXT).\r\n"
                                "  field_stage       %s (%d)\r\n"
                                "  fg_type           %s (%d)\r\n",
                                dnx_field_stage_e_get_name(field_stage), field_stage,
                                dnx_field_group_type_e_get_name(fg_type[free_test_step]), fg_type[free_test_step]);
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "  context_id        %d\r\n"
                                "  key_id_0          %c (%d) %s\r\n",
                                context_id[free_test_step], 'A' + key_id[free_test_step].id[0],
                                key_id[free_test_step].id[0], EMPTY);
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "  key_id_1          %c (%d) \r\n"
                                "  key_offset_bits   %d %s\r\n",
                                (key_id[free_test_step].id[1] ==
                                 DNX_FIELD_KEY_ID_INVALID) ? '-' : 'A' + key_id[free_test_step].id[1],
                                key_id[free_test_step].id[1], key_offset_bits[free_test_step], EMPTY);

                    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_free
                                    (unit, field_stage, fg_type[free_test_step], context_id[free_test_step],
                                     key_id[free_test_step], key_offset_bits[free_test_step]));
                }
                {
                    /*
                     * Free third key. Half.
                     */
                    LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 14 **************************************%s%s%s%s\r\n",
                                EMPTY, EMPTY, EMPTY, EMPTY);
                    free_test_step = 2;

                    DNX_DIAG_KEY_DISPLAY_FREE_PARAMS("Key free request - Third allocated half key (TCAM):\r\n");

                    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free
                                    (unit, fg_type[free_test_step], field_stage, context_id[free_test_step],
                                     &(key_id[free_test_step])));
                }
                SHR_IF_ERR_EXIT(field_key_display_key_occupation_info(unit, field_stage, context_id[alloc_test_step]));
                /*
                 * All keys are now free.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. ALL keys are FREE now. **************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
            }
        }
        if ((field_stage == DNX_FIELD_STAGE_IPMF1) || (field_stage == DNX_FIELD_STAGE_IPMF2))
        {
            /*
             * The following sequence is only valid for IPMF1 or IPMF2 because they have enough
             * keys-space to host one single key plus one double key.
             */
            {
                /*
                 * Allocate first key. Single. TCAM.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 15 ************************************** %s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
                alloc_test_step = 0;
                fg_type[alloc_test_step] = DNX_FIELD_GROUP_TYPE_TCAM;
                context_id[alloc_test_step] = context_index;
                key_length[alloc_test_step] = DNX_FIELD_KEY_LENGTH_TYPE_SINGLE;

                DNX_DIAG_KEY_DISPLAY_ALLOC_PARAMS("Key allocation request - single key for TCAM:\r\n");

                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate
                                (unit, 0, field_stage, fg_type[alloc_test_step], context_id[alloc_test_step],
                                 key_length[alloc_test_step], &key_id[alloc_test_step]));

                DNX_DIAG_KEY_DISPLAY_ALLOC_RESULTS();
            }
            {
                /*
                 * Allocate second key. Double.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 16 **************************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
                alloc_test_step = 1;
                fg_type[alloc_test_step] = DNX_FIELD_GROUP_TYPE_TCAM;
                context_id[alloc_test_step] = context_index;
                key_length[alloc_test_step] = DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE;

                DNX_DIAG_KEY_DISPLAY_ALLOC_PARAMS("Key allocation request - Double key for TCAM:\r\n");

                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate
                                (unit, 0, field_stage, fg_type[alloc_test_step], context_id[alloc_test_step],
                                 key_length[alloc_test_step], &key_id[alloc_test_step]));

                DNX_DIAG_KEY_DISPLAY_ALLOC_RESULTS();

            }
        }
        else
        {
            /*
             * The following sequence is only for IPMF3 or EPMF because they have only enough
             * keys-space to host one double key.
             */
            {
                /*
                 * Allocate first key. Double.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 15 **************************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
                alloc_test_step = 0;
                fg_type[alloc_test_step] = DNX_FIELD_GROUP_TYPE_TCAM;
                context_id[alloc_test_step] = context_index;
                key_length[alloc_test_step] = DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE;

                DNX_DIAG_KEY_DISPLAY_ALLOC_PARAMS("Key allocation request - Double key for TCAM:\r\n");

                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate
                                (unit, 0, field_stage, fg_type[alloc_test_step], context_id[alloc_test_step],
                                 key_length[alloc_test_step], &key_id[alloc_test_step]));

                DNX_DIAG_KEY_DISPLAY_ALLOC_RESULTS();
            }
            {
                /*
                 * Void step. Skip.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. SKIP Step 16 **************************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
            }
        }
        SHR_IF_ERR_EXIT(field_key_display_key_occupation_info(unit, field_stage, context_id[alloc_test_step]));
        {
            if ((field_stage == DNX_FIELD_STAGE_IPMF1) || (field_stage == DNX_FIELD_STAGE_IPMF2))
            {
                {
                    /*
                     * Free first key. single.
                     */
                    LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 17 **************************************%s%s%s%s\r\n",
                                EMPTY, EMPTY, EMPTY, EMPTY);
                    free_test_step = 0;

                    DNX_DIAG_KEY_DISPLAY_FREE_PARAMS("Key free request - First allocated double key:\r\n");

                    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free
                                    (unit, fg_type[free_test_step], field_stage, context_id[free_test_step],
                                     &(key_id[free_test_step])));
                }
                {
                    /*
                     * Free second key. Double.
                     */
                    LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 18 **************************************%s%s%s%s\r\n",
                                EMPTY, EMPTY, EMPTY, EMPTY);
                    free_test_step = 1;

                    DNX_DIAG_KEY_DISPLAY_FREE_PARAMS("Key free request - First allocated double key:\r\n");

                    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free
                                    (unit, fg_type[free_test_step], field_stage, context_id[free_test_step],
                                     &(key_id[free_test_step])));
                }
            }
            else
            {
                {
                    /*
                     * Free first key. Double.
                     */
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "Key test. SKIP Step 17 **************************************%s%s%s%s\r\n", EMPTY,
                                EMPTY, EMPTY, EMPTY);
                    LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 18 **************************************%s%s%s%s\r\n",
                                EMPTY, EMPTY, EMPTY, EMPTY);
                    free_test_step = 0;

                    DNX_DIAG_KEY_DISPLAY_FREE_PARAMS("Key free request - First allocated double key:\r\n");

                    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free
                                    (unit, fg_type[free_test_step], field_stage, context_id[free_test_step],
                                     &(key_id[free_test_step])));
                }
            }
            SHR_IF_ERR_EXIT(field_key_display_key_occupation_info(unit, field_stage, context_id[alloc_test_step]));
            /*
             * All keys are now free.
             */
            LOG_INFO_EX(BSL_LOG_MODULE, "Key test. ALL keys are FREE now. **************************%s%s%s%s\r\n",
                        EMPTY, EMPTY, EMPTY, EMPTY);
        }
        /*
         * The following sequence involves DIRECT EXTRACTION so do not execute for IPMF1 or EPMF.
         */
        if ((field_stage != DNX_FIELD_STAGE_IPMF1) && (field_stage != DNX_FIELD_STAGE_EPMF))
        {
            {
                /*
                 * Allocate first key. Single.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 19 **************************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
                alloc_test_step = 0;
                fg_type[alloc_test_step] = DNX_FIELD_GROUP_TYPE_TCAM;
                context_id[alloc_test_step] = context_index;
                key_length[alloc_test_step] = DNX_FIELD_KEY_LENGTH_TYPE_SINGLE;

                DNX_DIAG_KEY_DISPLAY_ALLOC_PARAMS("Key allocation request - Single key for TCAM:\r\n");

                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate
                                (unit, 0, field_stage, fg_type[alloc_test_step], context_id[alloc_test_step],
                                 key_length[alloc_test_step], &key_id[alloc_test_step]));

                DNX_DIAG_KEY_DISPLAY_ALLOC_RESULTS();
            }
            {
                /*
                 * Allocate second key. Half.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 20 **************************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
                alloc_test_step = 1;
                fg_type[alloc_test_step] = DNX_FIELD_GROUP_TYPE_TCAM;
                context_id[alloc_test_step] = context_index;
                key_length[alloc_test_step] = DNX_FIELD_KEY_LENGTH_TYPE_HALF;

                DNX_DIAG_KEY_DISPLAY_ALLOC_PARAMS("Key allocation request - Half key for TCAM:\r\n");

                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate
                                (unit, 0, field_stage, fg_type[alloc_test_step], context_id[alloc_test_step],
                                 key_length[alloc_test_step], &key_id[alloc_test_step]));

                DNX_DIAG_KEY_DISPLAY_ALLOC_RESULTS();
            }
            if (field_stage != DNX_FIELD_STAGE_IPMF3)
            {
                /*
                 * For IPMF3, there are only 3 keys so, at this point, we can not add
                 * a double key.
                 */
                /*
                 * Allocate third key. Double.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 21 **************************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
                alloc_test_step = 2;
                fg_type[alloc_test_step] = DNX_FIELD_GROUP_TYPE_TCAM;
                context_id[alloc_test_step] = context_index;
                key_length[alloc_test_step] = DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE;

                DNX_DIAG_KEY_DISPLAY_ALLOC_PARAMS("Key allocation request - Double key for TCAM:\r\n");

                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate
                                (unit, 0, field_stage, fg_type[alloc_test_step], context_id[alloc_test_step],
                                 key_length[alloc_test_step], &key_id[alloc_test_step]));

                DNX_DIAG_KEY_DISPLAY_ALLOC_RESULTS();
            }
            {
                /*
                 * Allocate fourth key. Half. DE.
                 */
                dnx_algo_field_key_bit_range_align_t align_info = { 0 };
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 22 **************************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
                alloc_test_step = 3;
                fg_type[alloc_test_step] = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
                context_id[alloc_test_step] = context_index;
                key_length_bits[alloc_test_step] = 80;

                align_info.align_lsb = FALSE;
                align_info.align_msb = FALSE;

                LOG_INFO_EX(BSL_LOG_MODULE, "Key allocation request - Half key for direct extraction:\r\n"
                            "  field_stage       %s (%d)\r\n"
                            "  fg_type           %s (%d)\r\n",
                            dnx_field_stage_e_get_name(field_stage), field_stage,
                            dnx_field_group_type_e_get_name(fg_type[alloc_test_step]), fg_type[alloc_test_step]);
                LOG_INFO_EX(BSL_LOG_MODULE,
                            "  context_id        %d\r\n"
                            "  key_offset_bits   %d %s%s\r\n",
                            context_id[alloc_test_step], key_offset_bits[alloc_test_step], EMPTY, EMPTY);

                SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_allocate
                                (unit, field_stage, fg_type[alloc_test_step], context_id[alloc_test_step],
                                 key_length_bits[alloc_test_step], &align_info, FALSE, &key_id[alloc_test_step],
                                 &key_offset_bits[alloc_test_step]));

                LOG_INFO_EX(BSL_LOG_MODULE, "Result of allocation request:\r\n"
                            "  key_id_0          %c (%d)\r\n"
                            "  key_id_1          %c (%d)\r\n",
                            'A' + key_id[alloc_test_step].id[0], key_id[alloc_test_step].id[0],
                            (key_id[alloc_test_step].id[1] ==
                             DNX_FIELD_KEY_ID_INVALID) ? '-' : 'A' + key_id[alloc_test_step].id[1],
                            key_id[alloc_test_step].id[1]);
                LOG_INFO_EX(BSL_LOG_MODULE, "  key_offset_bits   %d%s%s%s\r\n", key_offset_bits[alloc_test_step], EMPTY,
                            EMPTY, EMPTY);
            }
            SHR_IF_ERR_EXIT(field_key_display_key_occupation_info(unit, field_stage, context_id[alloc_test_step]));
            {
                {
                    /*
                     * Free second key. Half.
                     */
                    LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 23 **************************************%s%s%s%s\r\n",
                                EMPTY, EMPTY, EMPTY, EMPTY);
                    free_test_step = 1;

                    DNX_DIAG_KEY_DISPLAY_FREE_PARAMS("Key free request - Second allocated half key:\r\n");

                    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free
                                    (unit, fg_type[free_test_step], field_stage, context_id[free_test_step],
                                     &(key_id[free_test_step])));
                }
                {
                    /*
                     * Free fourth key. Half. DE.
                     */
                    LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 24 **************************************%s%s%s%s\r\n",
                                EMPTY, EMPTY, EMPTY, EMPTY);
                    free_test_step = 3;

                    LOG_INFO_EX(BSL_LOG_MODULE, "Key free request - First allocated half key (DIR EXT).\r\n"
                                "  field_stage       %s (%d)\r\n"
                                "  fg_type           %s (%d)\r\n",
                                dnx_field_stage_e_get_name(field_stage), field_stage,
                                dnx_field_group_type_e_get_name(fg_type[free_test_step]), fg_type[free_test_step]);
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "  context_id        %d\r\n"
                                "  key_id_0          %c (%d) %s\r\n",
                                context_id[free_test_step], 'A' + key_id[free_test_step].id[0],
                                key_id[free_test_step].id[0], EMPTY);
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "  key_id_1          %c (%d) \r\n"
                                "  key_offset_bits   %d %s\r\n",
                                (key_id[free_test_step].id[1] ==
                                 DNX_FIELD_KEY_ID_INVALID) ? '-' : 'A' + key_id[free_test_step].id[1],
                                key_id[free_test_step].id[1], key_offset_bits[free_test_step], EMPTY);

                    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_free
                                    (unit, field_stage, fg_type[free_test_step], context_id[free_test_step],
                                     key_id[free_test_step], key_offset_bits[free_test_step]));
                }
                {
                    /*
                     * Free first key. Single.
                     */
                    LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 25 **************************************%s%s%s%s\r\n",
                                EMPTY, EMPTY, EMPTY, EMPTY);
                    free_test_step = 0;

                    DNX_DIAG_KEY_DISPLAY_FREE_PARAMS("Key free request - First allocated single key:\r\n");

                    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free
                                    (unit, fg_type[free_test_step], field_stage, context_id[free_test_step],
                                     &(key_id[free_test_step])));
                }
                if (field_stage != DNX_FIELD_STAGE_IPMF3)
                {
                    /*
                     * For IPMF3, there are only 3 keys so, at this point, double key was not added
                     * and does not need to be freed.
                     */
                    /*
                     * Free third key. double.
                     */
                    LOG_INFO_EX(BSL_LOG_MODULE, "Key test. Step 26 **************************************%s%s%s%s\r\n",
                                EMPTY, EMPTY, EMPTY, EMPTY);
                    free_test_step = 2;

                    DNX_DIAG_KEY_DISPLAY_FREE_PARAMS("Key free request - Third allocated double key (TCAM):\r\n");

                    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free
                                    (unit, fg_type[free_test_step], field_stage, context_id[free_test_step],
                                     &(key_id[free_test_step])));
                }
                SHR_IF_ERR_EXIT(field_key_display_key_occupation_info(unit, field_stage, context_id[alloc_test_step]));
                /*
                 * All keys are now free.
                 */
                LOG_INFO_EX(BSL_LOG_MODULE, "Key test. ALL keys are FREE now. **************************%s%s%s%s\r\n",
                            EMPTY, EMPTY, EMPTY, EMPTY);
            }
        }
        if (context_index != DNX_FIELD_CONTEXT_ID_DEFAULT(unit))
        {
            SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, stage_for_context_create, context_index));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is the basic 'key' testing
 *   application.
 *   For 'key', this function allocates different types of keys.
 *   For 'ffc', this function sets all required HW
 *   configuration for the 'key' template which is supposed to have been
 *   set on the 'database' step.
 *
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
appl_dnx_key_starter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int key_test_type_name;
    dnx_field_stage_e field_stage;
    dnx_field_context_t context_lower, context_upper;
    dnx_field_group_type_e fg_type;
    uint32 num_bits_in_bit_range;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_ENUM(DNX_DIAG_KEY_OPTION_TEST_STAGE, field_stage);
    SH_SAND_GET_ENUM(DNX_DIAG_KEY_OPTION_TEST_TYPE, key_test_type_name);
    SH_SAND_GET_UINT32_RANGE(DNX_DIAG_KEY_OPTION_TEST_CONTEXT, context_lower, context_upper);
    SH_SAND_GET_ENUM(DNX_DIAG_KEY_OPTION_TEST_GROUP_TYPE, fg_type);
    SH_SAND_GET_UINT32(DNX_DIAG_KEY_OPTION_TEST_NUM_BITS, num_bits_in_bit_range);

    /*
     * For now, a fail to match the input parameter result just by a message.
     * Procedure returns with 'success'.
     */
    switch (key_test_type_name)
    {
        case CTEST_DNX_FIELD_KEY_TEST_TYPE_KEY:
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Testing 'Key only' %s%s\r\n", __func__, __LINE__, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(field_key_tester_key(unit, field_stage, context_lower, context_upper, sand_control));
            break;
        }
        case CTEST_DNX_FIELD_KEY_TEST_TYPE_FFC:
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Testing 'ffc plus key %s' %s\r\n", __func__, __LINE__,
                        dnx_field_stage_text(unit, field_stage), EMPTY);
            SHR_IF_ERR_EXIT(field_key_tester_key_plus_ffc(unit, field_stage, sand_control));
            break;
        }
        case CTEST_DNX_FIELD_KEY_TEST_TYPE_FFC_PER_MULTIPLE_PROGRAMS:
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Testing 'ffc plus key' %s%s\r\n", __func__, __LINE__, EMPTY,
                        EMPTY);
            SHR_IF_ERR_EXIT(field_key_tester_key_plus_ffc(unit, field_stage, sand_control));
            break;
        }
        case CTEST_DNX_FIELD_KEY_TEST_TYPE_BIT_RANGE:
        {
            unsigned int stage_is_present;
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Testing 'bit-range' %s%s\r\n", __func__, __LINE__, EMPTY,
                        EMPTY);
            /*
             * Check whether 'stage' was specified by the user. If not, change the default from 'IPMF1' to 'IPMF2'
             */
            SH_SAND_IS_PRESENT(DNX_DIAG_KEY_OPTION_TEST_STAGE, stage_is_present);
            if (stage_is_present != TRUE)
            {
                field_stage = DNX_FIELD_STAGE_IPMF2;
            }
            SHR_IF_ERR_EXIT(field_key_tester_bit_range
                            (unit, field_stage, context_lower, context_upper, fg_type, (uint8) num_bits_in_bit_range,
                             sand_control));
            break;
        }
        case CTEST_DNX_FIELD_KEY_TEST_TYPE_SHARED_FFC:
        {
            unsigned int stage_is_present;
            unsigned int test_clean;
            dnx_field_context_t context;
            dnx_field_group_t fg_id_1 = 1;
            dnx_field_group_t fg_id_2 = 2;
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Testing 'ffc-shared' %s%s\r\n", __func__, __LINE__, EMPTY,
                        EMPTY);
            /*
             * Check whether 'stage' was specified by the user. If not, change the default from 'IPMF1' to 'IPMF2'
             */
            SH_SAND_IS_PRESENT(DNX_DIAG_KEY_OPTION_TEST_STAGE, stage_is_present);
            SH_SAND_GET_BOOL(DNX_DIAG_KEY_OPTION_CLEAN, test_clean);

            if ((stage_is_present != TRUE) || (field_stage != DNX_FIELD_STAGE_IPMF2))
            {
                LOG_ERROR_EX(BSL_LOG_MODULE,
                             "%s(), line %d, Testing 'ffc-shared' currently supported in IPMF2 only %s%s\r\n",
                             __func__, __LINE__, EMPTY, EMPTY);
            }
            SHR_IF_ERR_EXIT(field_key_tester_shared_ffc(unit, field_stage, &context, &fg_id_1, &fg_id_2, fg_type));
            if (test_clean == TRUE)
            {
                SHR_IF_ERR_EXIT(field_key_tester_shared_ffc_cleanup(unit, field_stage, context, fg_id_1, fg_id_2));
            }
            break;
        }
        default:
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Option value %d (for %s) is not implemented. Illegal parameter. Return with 'success' flag\r\n",
                        __func__, __LINE__, key_test_type_name, DNX_DIAG_KEY_OPTION_TEST_TYPE);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - run 'key' sequence in diag shell
 */
shr_error_e
sh_dnx_field_key_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");

    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SH_SAND_GET_UINT32(DNX_DIAG_KEY_OPTION_TEST_COUNT, count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_key_starter(unit, args, sand_control));
    }

exit:
    SHR_FUNC_EXIT;

}
/*
 * }
 */

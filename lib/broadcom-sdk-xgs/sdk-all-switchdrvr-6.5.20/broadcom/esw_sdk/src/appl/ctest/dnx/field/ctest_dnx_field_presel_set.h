/** \file diag_dnx_field_presel_set.h
 * Semantic test for field presel in the TCAM
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
* Include files.
* {
*/
/* soc */
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm/field.h>
#include <bcm/types.h>
#include <bcm/rx.h>
#include <bcm/error.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
/*
 * }
 */
/**
 * DEFINEs
 */
/**
 * {
 */
/*
 * Number of PMF stages to incorporate in the test
 */
#define CTEST_DNX_PRESEL_SET_NOF_STAGES                     4
/**
 * Number of quals used for key creation per stage
 */
#define CTEST_DNX_PRESEL_SET_NUM_OF_QUALS                   2
 /**
  * Creating 4 preselectors per stage
  */
#define CTEST_DNX_PRESEL_SET_NUM_OF_PRESELS                 4

/**
 * }
 */
/**
 * This structure holds the data per preselector
 * Used by presel_set function
 */
typedef struct
{
    /**
     * Number of valid qualifiers.
     * Max value: DNX_FIELD_CS_QUAL_NOF_MAX
     */
    uint32 nof_quals;
    /**
     * Indicates whether this preselector entry is valid
     */
    uint8 entry_valid;
    /**
     * The ID of the context
     * Max value: DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS
     */
    dnx_field_context_t context_id;
} dnx_ctest_presel_info_t;
/**
 * Qualifier attributes for each qualifier to be used for a preselector
 */
typedef struct
{
    /**
     * Qualifier type
     */
    dbal_fields_e qual_type;
    /**
     * Qualifier value
     */
    uint32 qual_value;
    /**
     * Qualifier mask
     */
    uint32 qual_mask;
} dnx_ctest_presel_qual_t;

/**
 * Holds all of the test stages flags
 * These flags indicate specific positive or one of the negative cases
 * Used in appl_dnx_field_presel_set_semantic()
 */
typedef enum
{

    CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_FIRST = 0x0,
    /** Positive test */
    CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_POSITIVE_TEST = CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_FIRST,
    /** Negative case for illegal presel ID */
    CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_ILLEGAL_PRESEL_ID = 0x1,
    /** Negative case for illegal context ID */
    CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_ILLEGAL_CONTEXT_ID = 0x2,
    /** Negative case for illegal stage */
    CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_ILLEGAL_STAGE = 0x3,
    /** Negative case for illegal qual type */
    CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_ILLEGAL_QUAL_TYPE = 0x4,
    /** Negative case for number of qualifiers*/
    CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_ILLEGAL_NOF_QUALS = 0x5,
    CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_NOF
} ctest_dnx_field_presel_set_negative_test_flag_e;

extern sh_sand_man_t Sh_dnx_field_presel_set_man;
/**
 * \brief
 *   Options list for 'database' shell command
 * \see
 *   appl_dnx_database_starter() in diag_dnx_field_database.c
 */
extern sh_sand_option_t Sh_dnx_field_presel_set_options[];
/**
 * \brief
 *   Options list for 'database' tests to run on regression, precommit, ...
 * \see
 *   appl_dnx_database_starter() in diag_dnx_field_database.c
 */
extern sh_sand_invoke_t Sh_dnx_field_presel_set_tests[];

/**
 * \brief - run 'database' (TCAM Look-ups) sequence in diag shell
 */
shr_error_e sh_dnx_field_presel_set_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

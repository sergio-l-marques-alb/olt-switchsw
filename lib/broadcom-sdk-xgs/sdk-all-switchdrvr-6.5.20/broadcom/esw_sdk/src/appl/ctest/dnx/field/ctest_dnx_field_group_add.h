/** \file diag_dnx_field_group_add.h
 * Semantic test for field group add in the TCAM
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
/**
 * DEFINEs
 */
/**
 * Number of PMF stages
 * for array size allocation
 */
#define CTEST_DNX_FIELD_GROUP_ADD_NOF_STAGES                    4
 /**
  * Number of Qualifiers and Action used for the test
  */
#define CTEST_DNX_FIELD_GROUP_ADD_NOF_QUALS_AND_ACTIONS         3
 /**
  * Number of Action used for
  * negative case3 (too big action buffer)
  */
#define CTEST_DNX_FIELD_GROUP_ADD_NOF_TOO_MANY_ACTIONS          6
 /**
  * Number of qualifiers and actions used for
  * negative case4 (using same qual twice)
  * negative case5 (using same action twice)
  */
#define CTEST_DNX_FIELD_GROUP_ADD_SAME_QUAL_AND_ACTIONS         2
/**
  * Number of qualifiers and actions used for
  * negative case1 (illegal FG ID)
  * negative case7 (using user qual which is not created yet)
  */
#define CTEST_DNX_FIELD_GROUP_ADD_ONLY_ONE_QUAL_AND_ACTION      0
 /**
  * Number of Qualifiers used for
  * negative test case2 (too many quals)
  */
#define CTEST_DNX_FIELD_GROUP_ADD_NOF_TOO_MANY_QUALS            4
 /**
  * Number of FG used by positive test
  * (Creating 2 FG with and without ID)
  */
#define CTEST_DNX_FIELD_GROUP_ADD_NOF_FG                        2
 /**
  * The ID of the FG WITH_ID
  * Used in the positive case
  */
#define CTEST_DNX_FIELD_GROUP_ADD_FIELD_GROUP_ID               30
/**
  * The context ID
  * Used in FG with preallocation
  */
#define CTEST_DNX_FIELD_GROUP_ADD_FIELD_CONTEXT_ID             0
 /**
  * Number of Invalid FG used by negative test case2
  * (Creating FG with invalid ID)
  */
#define CTEST_DNX_FIELD_GROUP_ADD_INVALID_FG_ID      (dnx_data_field.group.nof_fgs_get(unit) + 2)
/**
 * Number of TCAM BANKS
 * Used to set the value of nof_tcam_banks in fg with bank preallocation
 */
#define CTEST_DNX_FIELD_GROUP_ADD_NOF_TCAM_BANKS               2
/**
 * The priority of the entry
 */
#define CTEST_DNX_FIELD_GROUP_ADD_ENTRY_PRIORITY               5
/**
 * Number of retries to apply if, after sending a packet, signals were not yet received.
 * Time delay, in seconds, between retries, is CTEST_DNX_FIELD_ACE_PACKET_WAIT.
 */
#define CTEST_DNX_FIELD_GROUP_ADD_PACKET_RETRY     1
/**
 * For each retry (as seen above in CTEST_DNX_FIELD_ACE_PACKET_RETRY), how many seconds to wait.
 */
#define CTEST_DNX_FIELD_GROUP_ADD_PACKET_WAIT      5
/**
 * The value to write to the actions in the entry.
 * Used in FG preallocation for IPMF1 (with traffic)
 * The 'expected_value' that the traffic function had to return.
 */
#define CTEST_DNX_FIELD_GROUP_ADD_ACTION_VALUE     0x3
/**
 * Holds all of the negative stage flags
 * This flags indicate specific negative case
 * Used in appl_dnx_field_group_add_semantic()
 */
typedef enum
{
    INVALID_CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_E = -1,
    CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_FIRST = 0x0,
    /** Negative case for positive test */
    CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_POSITIVE = CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_FIRST,
    /** Negative case for illegal FG ID */
    CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_ILLEGAL_FG_ID = 0x1,
    /** NEgative case for too big key */
    CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_TOO_BIG_KEY = 0x2,
    /** Negative case for too big action buffer*/
    CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_TOO_BIG_ACTION_BUFFER = 0x3,
    /** Negative case for using same qual */
    CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_USING_SAME_QUAL_TWICE = 0x4,
    /** Negative case for using same action */
    CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_USING_SAME_ACTION_TWICE = 0x5,
    /** Negative case for using wrong stage qual*/
    CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_WRONG_STAGE_QUAL = 0x6,
    /** Negative case for using not created qual*/
    CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_NOT_CREATED_USER_QUAL = 0x7,
    NUM_CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_E
} ctest_dnx_field_group_add_test_flag_e;
/*
 * }
 */
extern sh_sand_man_t Sh_dnx_field_group_add_man;
/**
 * \brief
 *   Options list for 'group_add' shell command
 * \see
 *   appl_dnx_group_add_starter() in ctest_dnx_field_group_add.c
 */
extern sh_sand_option_t Sh_dnx_field_group_add_options[];
/**
 * \brief
 *   Options list for 'group_add' tests to run on regression, precommit, ...
 * \see
 *   appl_dnx_group_add_starter() in ctest_dnx_field_group_add.c
 */
extern sh_sand_invoke_t Sh_dnx_field_group_add_tests[];

/**
 * \brief - run 'group_add' (TCAM Look-ups) sequence in diag shell
 */
shr_error_e sh_dnx_field_group_add_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

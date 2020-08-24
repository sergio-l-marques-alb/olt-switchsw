
/** \file diag_dnx_field_dir_ext_semantic.h
 * Direct Extraction appl
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_DIR_EXT_SEMANTIC_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_DIR_EXT_SEMANTIC_H_INCLUDED
/*
* Include files.
* {
*/
/** shared */
#include <shared/bsl.h>
#include <shared/bslenum.h>
/** appl */
#include <appl/diag/diag.h>
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

#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>

/*
 * }
 */
/*
 * DEFINEs
 * {
 */
/**
 * Number of PMF stages used in the test.
 * In DE we support only the IPMF2 and IPMF3
 */
#define CTEST_DNX_DIR_EXT_SEMANTIC_NOF_STAGES               2
/**
 * Number of Pre defined Qualifiers per stage used in the test.
 */
#define CTEST_DNX_DIR_EXT_SEMANTIC_NOF_PRE_DEFINED_QUALS    4
/**
 * Number of Qualifiers and Actions used in the test
 * We are using 7 user defined actions and
 * 4 pre defined quals + 3 user defined quals.
 */
#define CTEST_DNX_DIR_EXT_SEMANTIC_NOF_QUALS_AND_ACTIONS    7
/**
 * \brief
 *   Keyword for test type on 'dir_ext_semantic' command (direct extraction testing)
 *   Type can be either BCM or DNX
 */
#define CTEST_DNX_DIR_EXT_SEMANTIC_OPTION_TYPE        "type"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define CTEST_DNX_DIR_EXT_SEMANTIC_OPTION_COUNT       "count"
/**
 * \brief
 *   Keyword for controlling whether to release resources at end of test.
 *   clean can be either 0 or 1, if the ctest will clean-up after its finished setting-up.
 */
#define CTEST_DNX_DIR_EXT_SEMANTIC_OPTION_CLEAN       "clean"
/**
 * \brief
 *   Keyword for stage of test on 'dir_ext_semantic' command (data base testing)
 *   stage can be only IPMF2 or IPMF3, because only for those 2 stages we support DE.
 */
#define CTEST_DNX_DIR_EXT_SEMANTIC_OPTION_STAGE       "stage"
/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/**
 * \brief
 *   Structure of descriptors of qualifiers to use for testing the interface
 *   for direct extraction.
 */
typedef struct
{
    /**
     * Qualifier's Name of the user defined qualifier
     */
    char *qual_name;
    /**
     * Number of bits to assign to this user defined qualifier
     */
    unsigned int qual_nof_bits;
} dnx_ctest_dir_ext_semantic_quals_t;
/**
 * \brief
 *   Structure of descriptors of actions to use for testing the interface
 *   for direct extraction.
 *   Note that we are only handling user defined actions (class = DNX_FIELD_ACTION_CLASS_USER)
 */
typedef struct
{
    /**
     * Action's Name
     */
    char *action_name;
    /**
     * Number of bits to assign to this user defined action
     */
    unsigned int action_nof_bits;
    /**
     * Prefix size for this user defined action.
     */
    unsigned int prefix_nof_bits;
    /**
     * The BCM action
     */
    bcm_field_action_t bcm_action_id;
    /*
     * Action info required for attaching context to filed group.
     * Currently, only contains 'priority'.
     */
    dnx_field_action_attach_info_t action_attach_info;
    /**
     * Memory space to load identifier of this user defined action (result
     * of dnx_field_action_create()).
     */
    dnx_field_action_t dnx_action;
} dnx_ctest_dir_ext_semantic_actions_t;
/**
 * Holds all of the created FGs.
 * The enum will be used for two purposes:
 *   1. In appl_dnx_dir_ext_positive_test().
 *     Used like an iterator between all of the FG.
 *     We start the iteration from the CTEST_DNX_DIR_EXT_FG_FIRST till CTEST_DNX_DIR_EXT_NOF_FG.
 *     In each iteration we assign different quals and actions.
 *   2. In appl_dnx_dir_ext_fg_create_and_attach()
 *     Shows how many qualifiers and actions will be attached.
 *     If we are in the first FG, we are attaching only 1 qual and action.
 *     If we are in the second FG, we are attaching 2 quals
 *     and 2 actions and so on.
 */
typedef enum
{
    INVALID_CTEST_DNX_DIR_EXT_FG_E = -1,
    CTEST_DNX_DIR_EXT_FG_FIRST = 0x0,
    /**
     * FG with 32 bits key
     * We are using 1 pre defined qual and 1 user defined action
     */
    CTEST_DNX_DIR_EXT_FG_32B = CTEST_DNX_DIR_EXT_FG_FIRST,
    /**
     * Fg with 54 bits key
     * We are using 2 pre defined qual and 2 user defined action
     */
    CTEST_DNX_DIR_EXT_FG_54B = 0x1,
    /**
     * Fg with 72 bits key
     * We are using 3 pre defined qual and 3 user defined action
     */
    CTEST_DNX_DIR_EXT_FG_72B = 0x2,
    /**
     * Fg with 80 bits key
     * We are using 4 pre defined qual and 4 user defined action
     */
    CTEST_DNX_DIR_EXT_FG_80B = 0x3,
    /**
     * Fg with 106 bits key
     * We are using 4 pre defined qual + 1 user_defined
     * and 5 user defined action. 
     * The 5th action is ActionVoid.
     */
    CTEST_DNX_DIR_EXT_FG_106B = 0x4,
    /**
     * Fg with 131 bits key
     * We are using 4 pre defined qual + 2 user_defined
     * and 6 user defined action.
     * The 5th action is ActionVoid.
     */
    CTEST_DNX_DIR_EXT_FG_131B = 0x5,
    /**
     * Fg with 160 bits key
     * We are using 4 pre defined qual + 3 user_defined
     * and 7 user defined action.
     * The 5th action is ActionVoid.
     */
    CTEST_DNX_DIR_EXT_FG_160B = 0x6,
    CTEST_DNX_DIR_EXT_NOF_FG
} ctest_dnx_dir_ext_fg_e;

/**
 * The enum will be used for two purposes:
 *    1.In appl_dnx_dir_ext_fg_create_and_attach()
 *        To make a difference between positive and negative test.
 *        In order to eliminate the ERRORS from the log if the case is negative.
 *        And to EXIT from the function with the proper ERROR if the case is positive.
 *    2.In appl_dnx_dir_ext_negative_test()
 *       To switch between all negative cases.
 */
typedef enum
{
    CTEST_DNX_DIR_EXT_TEST_FIRST = 0x0,
    /**
     * Positive case:
     * Used to make a difference between the positive and the negative cases.
     */
    CTEST_DNX_DIR_EXT_POSITIVE_TEST = CTEST_DNX_DIR_EXT_TEST_FIRST,
    /**
     * Negative case1:
     * For creating a DE FG with qualifiers and actions with different size.
     */
    CTEST_DNX_DIR_EXT_NEGATIVE_DIFF_SIZE_QUALS_AND_ACTIONS = 0x1,
    /**
     * Negative case2:
     *For creating a DE FG with too large key.
     */
    CTEST_DNX_DIR_EXT_NEGATIVE_TOO_LARGE_KEY = 0x2,
    /**
     * Negative case3:
     * For creating a DE FG with qualifier with different stage.
     */
    CTEST_DNX_DIR_EXT_NEGATIVE_DIFFERENT_STAGE_QUAL = 0x3,
    /**
     * Negative case3:
     * For creating a DE FG with illegal stage.
     */
    CTEST_DNX_DIR_EXT_NEGATIVE_ILLEGAL_STAGE = 0x4,
    CTEST_DNX_DIR_EXT_NOF_TESTS
} ctest_dnx_dir_ext_test_flag_e;
/*
 * }
 */

extern sh_sand_man_t Sh_dnx_dir_ext_sem_man;

extern sh_sand_option_t Sh_dnx_field_dir_ext_sem_options[];

extern sh_sand_invoke_t Sh_dnx_field_dir_ext_sem_tests[];

/**
 * \brief - run Direct Extraction init sequence in diag shell
 */
shr_error_e sh_dnx_dir_ext_sem_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
/*
 * }
 */

#endif /* DIAG_DNX_DIR_EXT_SEMANTIC_H_INCLUDED */

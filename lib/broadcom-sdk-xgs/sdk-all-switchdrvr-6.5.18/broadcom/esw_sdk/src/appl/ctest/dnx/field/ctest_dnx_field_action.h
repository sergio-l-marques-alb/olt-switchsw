/**
 * \file ctest_dnx_field_action.h
 *
 * 'Action' operations (for database create and entry add) procedures for DNX.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef CTEST_DNX_FIELD_ACTION_H_INCLUDED
/*
 * {
 */
#define CTEST_DNX_FIELD_ACTION_H_INCLUDED
/*
* Include files.
* {
*/
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm/field.h>
#include <bcm/types.h>
#include <bcm/error.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
/** soc */
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
/*
 * }
 */
/*
 * DEFINEs
 * {
 */
/*
 * Number of setups for 'condition's array on
 *   'CTEST_DNX_FIELD_FEM_TEST_FEM_ADD_TYPE' test.
 *   'CTEST_DNX_FIELD_FEM_TEST_FEM_ATTACH_TYPE' test.
 * Each 'condition's setup may point to different 'action's
 */
#define CTEST_DNX_FIELD_FEM_NUM_CONDITION_SETUPS               2
/*
 * Number of field groups which are cretaed ('added') on 'CTEST_DNX_FIELD_FEM_TEST_FEM_ADD_TYPE' test.
 * Field groups are created in pairs: One as the 'main' and one as 'second ' (for 'replace')
 */
#define CTEST_DNX_FIELD_FEM_NUM_FGS_TO_CREATE_ON_ADD           4
/*
 * Number of field groups which are cretaed ('added') on 'CTEST_DNX_FIELD_FEM_TEST_FEM_ATTACH_TYPE' test.
 */
#define CTEST_DNX_FIELD_FEM_NUM_FGS_TO_CREATE_ON_ATTACH        4
/*
 * Number of contexts to attach to each field groups on 'CTEST_DNX_FIELD_FEM_TEST_FEM_ATTACH_TYPE' test.
 */
#define CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_FG_ON_ATTACH      4

/*
 * Number of field groups which are cretaed ('added') on 'CTEST_DNX_FIELD_FEM_TEST_FEM_CASCADE_TYPE' test.
 */
#define CTEST_DNX_FIELD_FEM_NUM_FGS_TO_CREATE_ON_CASCADE             2
/*
 * Index identifier of 'owner' field group on 'CTEST_DNX_FIELD_FEM_TEST_FEM_CASCADE_TYPE' test.
 */
#define CTEST_DNX_FIELD_FEM_INDEX_OF_OWNER_FG_ON_CASCADE             0
/*
 * Index identifier of 'secondary' field group on 'CTEST_DNX_FIELD_FEM_TEST_FEM_CASCADE_TYPE' test.
 */
#define CTEST_DNX_FIELD_FEM_INDEX_OF_SECONDARY_FG_ON_CASCADE         1
/*
 * Number of contexts to attach to 'owner' field groups on 'CTEST_DNX_FIELD_FEM_TEST_FEM_CASCADE_TYPE' test.
 */
#define CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_OWNER_FG_ON_CASCADE     1
/*
 * Index of first contexts to attach to 'owner' field groups on 'CTEST_DNX_FIELD_FEM_TEST_FEM_CASCADE_TYPE' test.
 */
#define CTEST_DNX_FIELD_FEM_FIRST_CONTEXT_PER_OWNER_FG_ON_CASCADE    0
/*
 * Number of contexts to attach to 'secondary' field groups on 'CTEST_DNX_FIELD_FEM_TEST_FEM_CASCADE_TYPE' test.
 */
#define CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_SECONDARY_FG_ON_CASCADE 3
/*
 */
/*
 * Number of setups for FEMs and 'condition's array on
 *   'CTEST_DNX_FIELD_FEM_TEST_FEM_CASCADE_TYPE' test.
 * Each FEM gets a 'condition's setup which may point to different 'action's
 */
#define CTEST_DNX_FIELD_FEM_NUM_FEM_SETUPS_CASCADED                  2
/*
 * Display SWSTATE info relating to '_fem_id' provided current BSL severity is 'info'
 * or higher:
 * Invoke appl_dnx_action_swstate_fem_display() if severity is 'info' or above.
 * To be used for dispaly on 'ctest' (as opposed to using on 'diag')
 */
#define APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(_unit,_fem_id,_test_text,_sand_control) \
    if (bsl_fast_check(BSL_LOG_MODULE | BSL_INFO)) \
    { \
        SHR_IF_ERR_EXIT(appl_dnx_action_swstate_fem_display(_unit, _fem_id, _test_text, _sand_control)); \
    }
/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/**
 * \brief
 *   Structure of descriptors of qualifiers to use for testing interface
 *   of direct extraction.
 * \see
 *   DNX_CTEST_DIR_EXT_NUM_QUALS_FOR_N_DE
 */
typedef struct
{
    /**
     * Qualifier's Name
     */
    char *qual_name;
    /**
     * Number of bits to assign to this user defined qualifier
     */
    unsigned int qual_nof_bits;
    /**
     * input_type/input_arg/offset to assign to this user defined qualifier.
     */
    dnx_field_qual_attach_info_t qual_attach_info;
    /**
     * Memory space to load identifier of this user defined qualifier (result
     * of dnx_field_qual_create()).
     */
    dnx_field_qual_t dnx_qual;
} dnx_ctest_field_quals_for_fg_t;
/**
 * \brief
 *   Structure of descriptors of actions to use for testing interface
 *   of direct extraction.
 *   Note that we are only handling user defined actions (class = DNX_FIELD_ACTION_CLASS_USER)
 * \see
 *   DNX_CTEST_DIR_EXT_NUM_QUALS_FOR_N_DE
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
     * DBAL identifier for this action. Also used as index to get base action's
     * properties. See dnx_ipmf1_action_info[].
     */
    dbal_enum_value_field_ipmf1_action_e dbal_action_id;
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
} dnx_ctest_field_actions_for_fg_t;
/**
 * Describe the various FEM tests.
 * Definitions used to fill enum for type of FEM test within 'action' tests
 * See 'ctest_dnx_field_action.c' and 'diag_dnx_field.c'
 */
typedef enum
{
    /**
     * Invalid test type
     */
    CTEST_DNX_FIELD_FEM_TEST_TYPE_INVALID = -1,
    CTEST_DNX_FIELD_FEM_TEST_TYPE_FIRST = 0,
    /**
     * Carry out test of FEMs: Check utilities dnx_field_actions_fem_action_info_hw_get()
     * and dnx_field_actions_fem_action_info_hw_set()
     */
    CTEST_DNX_FIELD_FEM_TEST_ACTION_INFO_TYPE = CTEST_DNX_FIELD_FEM_TEST_TYPE_FIRST,
    /**
     * Carry out test of FEMs: Check utilities dnx_field_actions_fem_map_index_hw_get()
     * and dnx_field_actions_fem_map_index_hw_set()
     */
    CTEST_DNX_FIELD_FEM_TEST_MAP_INDEX_TYPE,
    /**
     * Carry out test of FEMs: Check utilities dnx_field_actions_fem_condition_ms_bit_hw_get()
     * and dnx_field_actions_fem_condition_ms_bit_hw_set()
     */
    CTEST_DNX_FIELD_FEM_TEST_BIT_SELECT_TYPE,
    /**
     * Carry out test of FEMs: Check utilities dnx_field_actions_fem_get()
     * and dnx_field_actions_fem_set()
     */
    CTEST_DNX_FIELD_FEM_TEST_ACTIONS_FEM_TYPE,
    /**
     * Carry out test of FEMs: Check utilities dnx_field_actions_pmf_fem_context_hw_get()
     * and dnx_field_actions_pmf_fem_context_hw_set()
     */
    CTEST_DNX_FIELD_FEM_TEST_FEM_CONTEXT_TYPE,
    /**
     * Carry out test of FEMs: Check utilities dnx_field_fem_action_add()
     * and dnx_field_fem_action_remove()
     */
    CTEST_DNX_FIELD_FEM_TEST_FEM_ADD_TYPE,
    /**
     * Carry out test of FEMs: Check utilities dnx_field_group_fems_context_attach()
     * and dnx_field_group_fems_context_detach(), dnx_field_actions_dir_ext_fem_key_select_get()
     * dnx_field_actions_fem_key_select_get()
     */
    CTEST_DNX_FIELD_FEM_TEST_FEM_ATTACH_TYPE,
    /**
     * Carry out test of FEMs: Similar to CTEST_DNX_FIELD_FEM_TEST_FEM_ATTACH_TYPE but
     * create contexts with cascading.
     * Check utilities dnx_field_group_fems_context_attach()
     * and dnx_field_group_fems_context_detach(), dnx_field_actions_dir_ext_fem_key_select_get()
     * dnx_field_actions_fem_key_select_get()
     */
    CTEST_DNX_FIELD_FEM_TEST_FEM_CASCADE_TYPE,
    /**
     * Carry out test of encoding 'fem_id' and decoding encoded position.
     * Essentially, this is a test for DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET(),
     * which is to be used at entry to FEM procedures (e.g., bcm_dnx_field_fem_action_add).
     * Encoding is assumed to be done using BCM_FIELD_ACTION_POSITION.
     */
    CTEST_DNX_FIELD_FEM_TEST_ENCODED_POSITION_TYPE,
    /**
     * Number of types in ctest_dnx_field_fem_test_type_e
     */
    CTEST_DNX_FIELD_FEM_TEST_TYPE_NOF
} ctest_dnx_field_fem_test_type_e;
/*
 * }
 */

extern sh_sand_man_t Sh_dnx_field_action_man;
/**
 * \brief
 *   Options list for 'action' tests to run on regression, precommit, ...
 * \see
 *   appl_dnx_action_starter() in diag_dnx_field_action.c
 */
extern sh_sand_invoke_t Sh_dnx_field_action_tests[];
/**
 * \brief
 *   Options list for 'action' shell command
 * \see
 *   appl_dnx_action_starter() in diag_dnx_field_action.c
 */
extern sh_sand_option_t Sh_dnx_field_action_options[];

/**
 * \brief - run 'action' sequence in diag shell
 */
shr_error_e sh_dnx_field_action_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * }
 */

#endif /* CTEST_DNX_FIELD_ACTION_H_INCLUDED */

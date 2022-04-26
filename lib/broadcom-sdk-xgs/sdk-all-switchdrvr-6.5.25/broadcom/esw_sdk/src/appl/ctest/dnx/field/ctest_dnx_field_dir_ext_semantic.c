/** \file diag_dnx_field_dir_ext_semantic.c
 *
 * Direct Extraction application procedures for DNX.
 *
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /*
  * Include files.
  * {
  */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/field.h>
#include <bcm/error.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_context.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include "ctest_dnx_field_utils.h"
#include "ctest_dnx_field_dir_ext_semantic.h"
#include <soc/dnx/dnx_err_recovery_manager.h>
/*
 * }
 */

 /*
  * Global and Static
  * {
  */
/* *INDENT-OFF* */
/**
 * \brief
 *   Options list for 'dir_ext_sem' shell command
 * \remark
 *   Set the default value such that no test is carried out but procedure does return with 'success'
 *   This ensures success on regression and pre-commit:
 */
sh_sand_option_t Sh_dnx_field_dir_ext_sem_options[] = {
     /* Name */                                    /* Type */               /* Description */                                       /* Default */
    {CTEST_DNX_DIR_EXT_SEMANTIC_OPTION_TYPE,       SAL_FIELD_TYPE_ENUM,    "Type (level) of test (dnx or bcm)",                     "DNX",  (void *)Field_level_enum_table},
    {CTEST_DNX_DIR_EXT_SEMANTIC_OPTION_COUNT,      SAL_FIELD_TYPE_UINT32,  "Number of times test will run",                         "1"},
    {CTEST_DNX_DIR_EXT_SEMANTIC_OPTION_STAGE,      SAL_FIELD_TYPE_ENUM,    "Stage of test (ipmf1, ipmf2, ipmf3, epmf)",             "ipmf2", (void *)Field_stage_enum_table},
     /* End of options list - must be last. */
    {NULL}
};

/**
 * \brief
 *   List of tests for 'dir_ext_semantic' ctest command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_dir_ext_sem_tests[] = {
    {"DNX_dir_ext_sem_ipmf2", "type=DNX count=3 stage=ipmf2", CTEST_POSTCOMMIT},
    {"DNX_dir_ext_sem_ipmf3", "type=DNX count=3 stage=ipmf3", CTEST_POSTCOMMIT},
    {NULL}
};

/**
 *  Direct Extraction leaf details
 */
sh_sand_man_t Sh_dnx_dir_ext_sem_man = {
    "Direct Extraction semantic CTEST",
    "Start the CTEST for Direct Extraction via BCM or DNX APIs.\r\n"
    "Testing may be for various stages: 'ipmf1' or 'ipmf2' or 'ipmf3' or 'epmf' \r\n"
    "Setting 'count' to, say, '2' will make the test run twice.\r\n",
    "ctest field dir_ext_sem type=<BCM | DNX> stage=<IPMF2 | IPMF3> count=<integer>  count=<integer>",
    "type=BCM de_type=old"
};
/**
 * Pre defined quals per stage.
 * Used for creating the key of the FG.
 * In first FG we use only the first qual from the array.
 * In second FG we use first 2 quals from the array and so on.
 */
static dnx_field_qual_t
    Pre_defined_qual_array[CTEST_DNX_DIR_EXT_SEMANTIC_NOF_STAGES][CTEST_DNX_DIR_EXT_SEMANTIC_NOF_PRE_DEFINED_QUALS] = {
{ DNX_FIELD_QUAL_CONTEXT_KEY_GEN_VAR,    DNX_FIELD_QUAL_GLOB_IN_LIF_0,   DNX_FIELD_QUAL_FWD_DOMAIN,     DNX_FIELD_QUAL_IN_TTL}, /**IPMF2*/
{ DNX_FIELD_QUAL_CONTEXT_KEY_GEN_VAR,    DNX_FIELD_QUAL_GLOB_IN_LIF_0,   DNX_FIELD_QUAL_FWD_DOMAIN,     DNX_FIELD_QUAL_IN_TTL}, /**IPMF3*/
};
/**
 * Input parameters for the the direct extraction style. To be used on the 'positive' test.
 * User defined quals used for creating of 5,6 and 7th FG.
 * In the 5th FG we use 4 pre-defined qualifiers + 1 user defined.
 * In the 6th FG we use 4 pre-defined qualifiers + 2 user defined and so on.
 */
static dnx_ctest_dir_ext_semantic_quals_t Ctest_dnx_dir_ext_semantic_quals[] =
{
     /* name */      /* size */
    { "",               26},
    { "",               25},
    { "",               29},
  /** Must be last! This NULL indicates the end of the array. */
    { NULL }
};
/**
 * Input parameters for the direct extraction.
 * In the first FG we use only the first action from the array.
 * In the second FG we use first 2 actions from the array and so on.
 */
static dnx_ctest_dir_ext_semantic_actions_t Ctest_dnx_dir_ext_semantic_actions[] =
{
     /* name */      /* size */  /*prefix size*/   /* BCM action id */       /* Priority */
    {"",                  32,           0,           bcmFieldActionForward,    {  BCM_FIELD_ACTION_POSITION(0, 8)  }},
    {"",                  22,          10,           bcmFieldActionForward,    {  BCM_FIELD_ACTION_POSITION(0, 7)  }},
    {"",                  18,          14,           bcmFieldActionForward,    {  BCM_FIELD_ACTION_POSITION(0, 6)  }},
    {"",                   8,          24,           bcmFieldActionVoid,       {  BCM_FIELD_ACTION_DONT_CARE  }},
    {"",                  26,           6,           bcmFieldActionForward,    {  BCM_FIELD_ACTION_POSITION(0, 4)  }},
    {"",                  25,           7,           bcmFieldActionRedirectMcast,{  BCM_FIELD_ACTION_POSITION(0, 3)  }},
    {"",                  29,           3,           bcmFieldActionRedirect,   {  BCM_FIELD_ACTION_POSITION(0, 2)  }},
  /** Must be last! This NULL indicates the end of the array. */
    { NULL }
};
/* *INDENT-ON* */
/*
 * }
 */
/**
 * \brief
 *   This function is creating 2 arrays:
 *   1 with quals made by 4 pre difeined quals and 3 user_defined.
 *   1 with actions made by 7 user_defined actions.
 *   This two arrays are used for creating different FGs.
 *
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Indicate the PMF stage.
 * \param [in] pre_defined_quals_p - Pointer to array per stage with pre_defined quals.
 * \param [in] user_qual_info_p - Pointer to array with user_defined quals.
 *                          For details, see 'dnx_ctest_dir_ext_semantic_quals_t'
 * \param [in] user_actions_p - Pointer to array with user_defined actions.
 *                          For details, see 'dnx_ctest_dir_ext_semantic_actions_t'
 * \param [out] new_quals_array_p - Pointer to the array with quals
 *                              which is going to be created in this function.
 * \param [out] new_actions_array_p - Pointer to the array with actions
 *                              which is going to be created in this function.
 *
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
ctest_dnx_dir_ext_sem_quals_and_actions(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t * pre_defined_quals_p,
    dnx_ctest_dir_ext_semantic_quals_t * user_qual_info_p,
    dnx_ctest_dir_ext_semantic_actions_t * user_actions_p,
    dnx_field_qual_t * new_quals_array_p,
    dnx_field_action_t * new_actions_array_p)
{
    int qual_index, user_qual_index, action_index;
    dnx_field_qualifier_in_info_t user_qual_info;
    dnx_field_action_in_info_t user_action_info;
    dnx_field_qual_flags_e qual_flags;
    bcm_field_qualify_t bcm_qual;
    bcm_field_action_t bcm_action;
    dnx_field_action_flags_e action_flags;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * NULL check for all pointers
     */
    SHR_NULL_CHECK(user_qual_info_p, _SHR_E_PARAM, "user_qual_info_p");
    SHR_NULL_CHECK(pre_defined_quals_p, _SHR_E_PARAM, "pre_defined_quals_p");
    SHR_NULL_CHECK(user_actions_p, _SHR_E_PARAM, "user_actions_p");
    SHR_NULL_CHECK(new_quals_array_p, _SHR_E_PARAM, "new_quals_array_p");
    SHR_NULL_CHECK(new_actions_array_p, _SHR_E_PARAM, "new_actions_array_p");
    /**
     * Iterate between all pre-defined quals
     */
    for (qual_index = 0; qual_index < CTEST_DNX_DIR_EXT_SEMANTIC_NOF_PRE_DEFINED_QUALS; qual_index++)
    {
        /**
         * Fill the first CTEST_DNX_DIR_EXT_SEMANTIC_NOF_PRE_DEFINED_QUALS elements in the new_quals_array_p.
         */
        new_quals_array_p[qual_index] =
            DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, field_stage, pre_defined_quals_p[qual_index]);
    }
    user_qual_index = 0;
    /**
     * Fill the user_qual_info structure and create all user_defined qualifiers.
     * Then add them into the new_quals_array_p right after the predefined quals.
     */
    while (user_qual_info_p[user_qual_index].qual_name != NULL)
    {
        /**
         * Creating a qual without ID
         */
        qual_flags = 0;
        /**
         * Init the user_qual_info struct
         */
        SHR_IF_ERR_EXIT(dnx_field_qual_in_info_init(unit, &user_qual_info));
        /**
         * Fill the user_qual_info struct
         */
        sal_strncpy(user_qual_info.name, user_qual_info_p[user_qual_index].qual_name, sizeof(user_qual_info.name));
        user_qual_info.size = user_qual_info_p[user_qual_index].qual_nof_bits;
        /*
         * 4th parameter should be pointer to bcm qualifier, if requested
         */
        SHR_IF_ERR_EXIT(dnx_field_qual_create(unit, qual_flags, &user_qual_info, &bcm_qual));
        SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, field_stage, bcm_qual, &(new_quals_array_p[qual_index])));

        qual_index++;
        user_qual_index++;
    }
    /**
     * Iterate between all of the actions and fill the user_action_info structure
     * till we reach the last element in the array.
     * Create all user defined actions and fill them into the new_actions_array_p.
     * Note that 'fg_info_p->dnx_actions' has been initiated in 'dnx_field_group_info_t_init' above.
     * Note that, in this test, we refer to the first '2msb' combination only. Currently,
     * the code only supports that.
     */
    action_index = 0;
    while (user_actions_p[action_index].action_name != NULL)
    {
        /**
         * Creating an action without ID
         */
        action_flags = 0;
        /**
         * Init the user_action_info struct
         */
        SHR_IF_ERR_EXIT(dnx_field_action_in_info_t_init(unit, &user_action_info));
        /**
         * Fill the user_action_info struct
         */
        user_action_info.bcm_action = user_actions_p[action_index].bcm_action_id;
        sal_strncpy(user_action_info.name, user_actions_p[action_index].action_name, sizeof(user_action_info.name));
        user_action_info.size = user_actions_p[action_index].action_nof_bits;
        user_action_info.stage = field_stage;
        user_action_info.prefix_size = user_actions_p[action_index].prefix_nof_bits;
        /*
         * 4th parameter should be pointer to bcm action, if requested
         */
        SHR_IF_ERR_EXIT(dnx_field_action_create(unit,
                                                action_flags,
                                                &user_action_info,
                                                &bcm_action, &(user_actions_p[action_index].dnx_action)));
        new_actions_array_p[action_index] = user_actions_p[action_index].dnx_action;
        action_index++;
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function contains the Direct Extraction
 *   application.This function sets all required HW
 *   configuration for Direct Extraction to be performed.
 *   It is creating 1 FG with type DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION.
 *   And it is Attaching the FG to the Default context.
 *
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Indicate the PMF stage.
 * \param [in] nof_iterations - Shows how many qualifiers and actions will be attached.
 *                           If we are in the first FG, we are attaching only 1 qual and action.
 *                           If we are in the second FG, we are attaching 2 quals
 *                           and 2 actions and so on.
 *                           For details, see 'ctest_dnx_dir_ext_fg_e'
 * \param [in] test_flag - To make a difference between positive and negative test.
 *                       In order to eliminate the ERRORS from the log if the case is negative.
 *                       And to EXIT from the function with the proper ERROR if the case is positive.
 *                       For details, see 'ctest_dnx_dir_ext_test_flag_e'
 * \param [in] actions_p - Pointer to the new actions array created in the function
 *                        ctest_dnx_dir_ext_sem_quals_and_actions()
 * \param [in] quals_p - Pointer to the new quals array created in the function
 *                        ctest_dnx_dir_ext_sem_quals_and_actions()
 * \param [in] user_actions_p - Pointer to dnx_ctest_dir_ext_semantic_actions_t.
 *                              Used to assign proper action priority in the attach_info structure.
 * \param [out] fg_info_p - Pointer to FG database info that is to be filled in by this procedure.
 *                       For details, see 'dnx_field_group_info_t'
 * \param [out] attach_info_p - Pointer to Context Attach database info that is to be filled in by this procedure.
 *                            For details, see 'dnx_field_group_attach_info_t'
 * \param [out] fg_id_p -  The ID of the FG.
 *                     In case group_add_flags = 0, will return the created field group ID.
 *
 * \return
 * For positive test:
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 * For negative test:
 *   Setting the error with SHR_SET_CURRENT_ERR macro
 */
static shr_error_e
ctest_dnx_dir_ext_sem_create_fg_and_attach(
    int unit,
    dnx_field_stage_e field_stage,
    ctest_dnx_dir_ext_fg_e nof_iterations,
    ctest_dnx_dir_ext_test_flag_e test_flag,
    dnx_field_action_t * actions_p,
    dnx_field_qual_t * quals_p,
    dnx_ctest_dir_ext_semantic_actions_t * user_actions_p,
    dnx_field_group_info_t * fg_info_p,
    dnx_field_group_attach_info_t * attach_info_p,
    dnx_field_group_t * fg_id_p,
    dnx_field_context_t * context_id_p)
{
    unsigned int assignment_index;
    dnx_field_group_add_flags_e group_add_flags;
    dnx_field_context_mode_t context_mode;
    dnx_field_stage_e context_field_stage;
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * NULL check for all pointers
     */
    SHR_NULL_CHECK(fg_info_p, _SHR_E_PARAM, "fg_info_p");
    SHR_NULL_CHECK(attach_info_p, _SHR_E_PARAM, "attach_info_p");
    SHR_NULL_CHECK(fg_id_p, _SHR_E_PARAM, "fg_id_p");
    SHR_NULL_CHECK(quals_p, _SHR_E_PARAM, "quals_p");
    SHR_NULL_CHECK(actions_p, _SHR_E_PARAM, "actions_p");
    SHR_NULL_CHECK(user_actions_p, _SHR_E_PARAM, "user_actions_p");
    SHR_NULL_CHECK(context_id_p, _SHR_E_PARAM, "context_id_p");
    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        context_field_stage = DNX_FIELD_STAGE_IPMF1;
    }
    else
    {
        context_field_stage = field_stage;
    }
    /**
     * Init all of the structures which are going to be filled.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, attach_info_p));
    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, fg_info_p));
    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
    group_add_flags = 0;
    rv = BCM_E_NONE;
    /**
     * Create a FG without ID
     */
    fg_info_p->field_stage = field_stage;
    fg_info_p->fg_type = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
    /**
     * Fill the attach_info and fg_info structures
     * Note that the input_type of the quals is DNX_FIELD_INPUT_TYPE_META_DATA.
     * That is why the offset and the input_arg are getting the default value.
     * They are receiving this values from the dnx_field_group_attach_info_t_init func.
     * The nof_iterations is enum and starts from 0, that's why we are adding 1 to it.
     */
    for (assignment_index = 0; assignment_index < (nof_iterations + 1); assignment_index++)
    {
        fg_info_p->dnx_actions[assignment_index] = actions_p[assignment_index];
        fg_info_p->use_valid_bit[assignment_index] = FALSE;
        fg_info_p->dnx_quals[assignment_index] = quals_p[assignment_index];

        attach_info_p->qual_info[assignment_index].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
        attach_info_p->qual_info[assignment_index].offset = 0;
        attach_info_p->dnx_quals[assignment_index] = fg_info_p->dnx_quals[assignment_index];
        attach_info_p->dnx_actions[assignment_index] = fg_info_p->dnx_actions[assignment_index];
        attach_info_p->action_info[assignment_index].priority =
            user_actions_p[assignment_index].action_attach_info.priority;
    }
    /**
     * Calling the API dnx_field_group_context_attach and dnx_field_group_add for positive case
     */
    if (test_flag == CTEST_DNX_DIR_EXT_POSITIVE_TEST)
    {
        /**
         * Create the context
         */
        SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, context_field_stage, &context_mode, context_id_p));
        /**
         * Create the FG
         */
        SHR_IF_ERR_EXIT(dnx_field_group_add(unit, group_add_flags, fg_info_p, fg_id_p));
        /**
         * Attach the FG to the default context
         */
        SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, *fg_id_p, (*context_id_p), attach_info_p));
    }
    /**
     * In all negative cases the error value is needed.
     * To be compare with the expected error.
     * So the error is set using SHR_SET_CURRENT_ERR.
     */
    else
    {
        rv = dnx_field_group_add(unit, group_add_flags, fg_info_p, fg_id_p);
        if (rv != BCM_E_NONE)
        {
            SHR_SET_CURRENT_ERR(rv);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *
 *  This function contains the field_group_get
 *  application. The function gets all of the FG info from the SW state
 *  and compares it with the set_info (from the field_group_add).
 *
 * \param [in] unit - The unit number.
 * \param [in] fg_id - The Field group ID
 * \param [in] set_fg_info_p - Pointer to SET database info from field_group_add
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 *
 */
static shr_error_e
appl_dnx_dir_ext_sem_fg_compare(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_info_t * set_fg_info_p)
{
    dnx_field_group_full_info_t get_fg_info;
    int fg_iterator;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Init the basic_info structure
     */
    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &get_fg_info.group_basic_info));
    /**
     * Calling dnx_field_group_get
     */
    SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id, &get_fg_info));
    /**
     * Compare the set and get field_stage
     */
    if (set_fg_info_p->field_stage != get_fg_info.group_basic_info.field_stage)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Set_field_stage (%d) is not the same as get_field_stage (%d)\r\n",
                     set_fg_info_p->field_stage, get_fg_info.group_basic_info.field_stage);
    }
    /**
     * Compare the set and get fg_type
     */
    if (set_fg_info_p->fg_type != get_fg_info.group_basic_info.fg_type)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Set_fg_type (%d) is not the same as get_fg_type (%d)\r\n",
                     set_fg_info_p->fg_type, get_fg_info.group_basic_info.fg_type);
    }
    /**
     * Iterate between all of the qualifiers and compare the set and get qual_type
     */
    for (fg_iterator = 0; set_fg_info_p->dnx_quals[fg_iterator] != DNX_FIELD_QUAL_TYPE_INVALID; fg_iterator++)
    {
        if (set_fg_info_p->dnx_quals[fg_iterator] != get_fg_info.group_basic_info.dnx_quals[fg_iterator])
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Set_fg qual_type (%d) is not the same as Get_fg qual_type (%d)\r\n",
                         set_fg_info_p->dnx_quals[fg_iterator], get_fg_info.group_basic_info.dnx_quals[fg_iterator]);
        }
    }
    /**
     * Iterate between all of the actions and compare the set and get dnx_action
     */
    for (fg_iterator = 0; set_fg_info_p->dnx_actions[fg_iterator] != DNX_FIELD_ACTION_INVALID; fg_iterator++)
    {
        if (set_fg_info_p->dnx_actions[fg_iterator] != get_fg_info.group_basic_info.dnx_actions[fg_iterator])
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Set_fg action (%d) is not the same as Get_fg action (%d)\r\n",
                         set_fg_info_p->dnx_actions[fg_iterator],
                         get_fg_info.group_basic_info.dnx_actions[fg_iterator]);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  This procedure is used for positive testing
 *  The test flow:
 *  Iterate between fg_key_size
 *    1. Create a different FG with 'fg_key_size' number of quals and actions in each iteration.
 *    2. Compare the set and get elements.
 *    3. Detach the context and delete the FG.
 *
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Indicate the PMF stage.
 * \param [in] user_qual_info_p - Pointer to array with user_defined quals.
 *                          For details, see 'dnx_ctest_dir_ext_semantic_quals_t'
 *                          Used for input of the ctest_dnx_dir_ext_sem_quals_and_actions().
 * \param [in] pre_defined_quals_p - Pointer to array per stage with pre_defined quals.
 *                          Used for input of the ctest_dnx_dir_ext_sem_quals_and_actions().
 * \param [in] user_actions_p - Pointer to array with user_defined actions.
 *                          For details, see 'dnx_ctest_dir_ext_semantic_actions_t'
 *                          Used for input of the ctest_dnx_dir_ext_sem_quals_and_actions().
 * \param [in] fg_info_p - Pointer to SET database info from dnx_field_group_add
 * \param [in] attach_info_p - Pointer to SET database info from dnx_field_group_context_attach
 * \param [in] fg_id_p - Pointer to FG ID SET in dnx_field_group_add.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_dir_ext_sem_positive_test(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_ctest_dir_ext_semantic_quals_t * user_qual_info_p,
    dnx_field_qual_t * pre_defined_quals_p,
    dnx_ctest_dir_ext_semantic_actions_t * user_actions_p,
    dnx_field_group_info_t * fg_info_p,
    dnx_field_group_attach_info_t * attach_info_p,
    dnx_field_group_t * fg_id_p)
{
    dnx_field_action_t actions_array[CTEST_DNX_DIR_EXT_SEMANTIC_NOF_QUALS_AND_ACTIONS];
    dnx_field_qual_t quals_array[CTEST_DNX_DIR_EXT_SEMANTIC_NOF_QUALS_AND_ACTIONS];
    dnx_field_action_t positive_test_actions[CTEST_DNX_DIR_EXT_SEMANTIC_NOF_QUALS_AND_ACTIONS];
    dnx_field_qual_t positive_test_quals[CTEST_DNX_DIR_EXT_SEMANTIC_NOF_QUALS_AND_ACTIONS];
    /**
     * Used like an iterator between all of the FG.
     * For details, see 'ctest_dnx_dir_ext_fg_e'.
     */
    ctest_dnx_dir_ext_fg_e fg_key_size;
    dnx_field_context_t context_id;
    dnx_field_stage_e context_field_stage;
    SHR_FUNC_INIT_VARS(unit);
    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        context_field_stage = DNX_FIELD_STAGE_IPMF1;
    }
    else
    {
        context_field_stage = field_stage;
    }
    /**
     * NULL check for all pointers
     */
    SHR_NULL_CHECK(user_qual_info_p, _SHR_E_PARAM, "user_qual_info_p");
    SHR_NULL_CHECK(pre_defined_quals_p, _SHR_E_PARAM, "pre_defined_quals_p");
    SHR_NULL_CHECK(user_actions_p, _SHR_E_PARAM, "user_actions_p");
    SHR_NULL_CHECK(fg_info_p, _SHR_E_PARAM, "fg_info_p");
    SHR_NULL_CHECK(attach_info_p, _SHR_E_PARAM, "attach_info_p");
    SHR_NULL_CHECK(fg_id_p, _SHR_E_PARAM, "fg_id_p");
    /**
     * Init all used arrays
     */
    sal_memset(quals_array, 0, sizeof(quals_array));
    sal_memset(actions_array, 0, sizeof(actions_array));
    sal_memset(positive_test_actions, 0, sizeof(positive_test_actions));
    sal_memset(positive_test_quals, 0, sizeof(positive_test_quals));
    /**
     * Create the quals_array from 4 pre defined quals and 3 user defined
     * and actions_array from 7 user defined actions.
     */
    SHR_IF_ERR_EXIT(ctest_dnx_dir_ext_sem_quals_and_actions(unit,
                                                            field_stage,
                                                            pre_defined_quals_p,
                                                            user_qual_info_p,
                                                            user_actions_p, quals_array, actions_array));
    /**
     * We start the iteration from the CTEST_DNX_DIR_EXT_FG_FIRST till CTEST_DNX_DIR_EXT_NOF_FG.
     * In each iteration we assign different quals and actions.
     * We create a FG and attach it to the context with the ctest_dnx_dir_ext_sem_create_fg_and_attach().
     */
    for (fg_key_size = CTEST_DNX_DIR_EXT_FG_FIRST; fg_key_size < CTEST_DNX_DIR_EXT_NOF_FG; fg_key_size++)
    {

        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s Create FG %d with DE key!\r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), fg_key_size);

        positive_test_actions[fg_key_size] = actions_array[fg_key_size];
        positive_test_quals[fg_key_size] = quals_array[fg_key_size];
        SHR_IF_ERR_EXIT(ctest_dnx_dir_ext_sem_create_fg_and_attach(unit,
                                                                   field_stage,
                                                                   fg_key_size,
                                                                   CTEST_DNX_DIR_EXT_POSITIVE_TEST,
                                                                   positive_test_actions,
                                                                   positive_test_quals,
                                                                   user_actions_p, fg_info_p, attach_info_p, fg_id_p,
                                                                   &context_id));
        /**
         * Compare the get elements and the set elements
         */
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %s .Performing compare between the add and get FG: %d \r\n", __func__,
                    __LINE__, dnx_field_stage_e_get_name(field_stage), fg_key_size);
        SHR_IF_ERR_EXIT(appl_dnx_dir_ext_sem_fg_compare(unit, *fg_id_p, fg_info_p));

        /**
         * Delete the FG
         */
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_context_detach(unit, *fg_id_p, context_id));
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_context_destroy(unit, context_field_stage, context_id));
        CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_delete(unit, *fg_id_p));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *
 *  This procedure is used for creating 4 negative cases.
 *
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Indicate the PMF stage.
 * \param [in] user_qual_info_p - Pointer to array with user_defined quals.
 *                          For details, see 'dnx_ctest_dir_ext_semantic_quals_t'
 *                          Used for input of the ctest_dnx_dir_ext_sem_quals_and_actions().
 * \param [in] pre_defined_quals_p - Pointer to array per stage with pre_defined quals.
 *                          Used for input of the ctest_dnx_dir_ext_sem_quals_and_actions().
 * \param [in] user_actions_p - Pointer to array with user_defined actions.
 *                          For details, see 'dnx_ctest_dir_ext_semantic_actions_t'
 *                          Used for input of the ctest_dnx_dir_ext_sem_quals_and_actions().
 * \param [in] fg_info_p - Pointer to SET database info from dnx_field_group_add
 * \param [in] attach_info_p - Pointer to SET database info from dnx_field_group_context_attach
 * \param [in] fg_id_p - Pointer to FG ID SET in dnx_field_group_add.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_dir_ext_sem_negative_test(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_ctest_dir_ext_semantic_quals_t * user_qual_info_p,
    dnx_field_qual_t * pre_defined_quals_p,
    dnx_ctest_dir_ext_semantic_actions_t * user_actions_p,
    dnx_field_group_info_t * fg_info_p,
    dnx_field_group_attach_info_t * attach_info_p,
    dnx_field_group_t * fg_id_p)
{
    dnx_field_action_t actions_array[CTEST_DNX_DIR_EXT_SEMANTIC_NOF_QUALS_AND_ACTIONS];
    dnx_field_qual_t quals_array[CTEST_DNX_DIR_EXT_SEMANTIC_NOF_QUALS_AND_ACTIONS];
    dnx_field_qual_t negative_test_quals[CTEST_DNX_DIR_EXT_SEMANTIC_NOF_QUALS_AND_ACTIONS + 1];
    dnx_field_action_t negative_test_actions[CTEST_DNX_DIR_EXT_SEMANTIC_NOF_QUALS_AND_ACTIONS + 1];
    ctest_dnx_dir_ext_test_flag_e negative_case_iterator;
    int nof_iterations;
    dnx_field_context_t context_id;
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * NULL check for all pointers
     */
    SHR_NULL_CHECK(user_qual_info_p, _SHR_E_PARAM, "user_qual_info_p");
    SHR_NULL_CHECK(pre_defined_quals_p, _SHR_E_PARAM, "pre_defined_quals_p");
    SHR_NULL_CHECK(user_actions_p, _SHR_E_PARAM, "user_actions_p");
    SHR_NULL_CHECK(fg_info_p, _SHR_E_PARAM, "fg_info_p");
    SHR_NULL_CHECK(attach_info_p, _SHR_E_PARAM, "attach_info_p");
    SHR_NULL_CHECK(fg_id_p, _SHR_E_PARAM, "fg_id_p");
    /**
     * Init all used arrays
     */
    sal_memset(quals_array, 0, sizeof(quals_array));
    sal_memset(actions_array, 0, sizeof(actions_array));
    sal_memset(negative_test_quals, 0, sizeof(negative_test_quals));
    sal_memset(negative_test_actions, 0, sizeof(negative_test_actions));

    /**
     * Create the quals_array from 4 pre defined quals and 3 user defined
     * and actions_array from 7 user defined negative_test_actions.
     */
    SHR_IF_ERR_EXIT(ctest_dnx_dir_ext_sem_quals_and_actions(unit,
                                                            field_stage,
                                                            pre_defined_quals_p,
                                                            user_qual_info_p,
                                                            user_actions_p, quals_array, actions_array));
    /**
     * Iterate between all negative cases starts from the first one 'CTEST_DNX_DIR_EXT_NEGATIVE_DIFF_SIZE_QUALS_AND_ACTIONS'
     * till we reach the end of the enum in which they are defined 'CTEST_DNX_DIR_EXT_NOF_TESTS'
     */
    for (negative_case_iterator = CTEST_DNX_DIR_EXT_NEGATIVE_DIFF_SIZE_QUALS_AND_ACTIONS;
         negative_case_iterator < CTEST_DNX_DIR_EXT_NOF_TESTS; negative_case_iterator++)
    {
        rv = BCM_E_NONE;
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s NEGATIVE TESTING! CASE: %d \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), negative_case_iterator);
        nof_iterations = 0;
        negative_test_actions[0] = actions_array[0];
        /**
         * Switching between all negative cases
         */
        switch (negative_case_iterator)
        {
            /**
             * Creating Fg with different size quals and actions.
             * Using the first action from the array and the second qualifier.
             */
            case CTEST_DNX_DIR_EXT_NEGATIVE_DIFF_SIZE_QUALS_AND_ACTIONS:
            {
                negative_test_quals[negative_case_iterator] = quals_array[negative_case_iterator + 1];
                break;
            }
            /**
             * Adding more then 160 quals and actions.
             * The DE is supporting only 80 and 160 bit key.
             */
            case CTEST_DNX_DIR_EXT_NEGATIVE_TOO_LARGE_KEY:
            {
                sal_memcpy(negative_test_quals, quals_array, MIN(sizeof(negative_test_quals), sizeof(quals_array)));
                negative_test_quals[CTEST_DNX_DIR_EXT_SEMANTIC_NOF_QUALS_AND_ACTIONS] = quals_array[0];
                sal_memcpy(negative_test_actions, actions_array,
                           MIN(sizeof(negative_test_actions), sizeof(actions_array)));
                negative_test_actions[CTEST_DNX_DIR_EXT_SEMANTIC_NOF_QUALS_AND_ACTIONS] = actions_array[0];
                /**
                 * Because we are creating +1 quals and actions.
                 * And we will need +1 iterations in the fg_create and context_attach.
                 * However we already perform +1 inside ctest_dnx_dir_ext_sem_create_fg_and_attach and so we do not 
                 * add more iterations.
                 */
                nof_iterations = CTEST_DNX_DIR_EXT_SEMANTIC_NOF_QUALS_AND_ACTIONS;
                break;
            }
            /**
             * Indicating different stage qual from the pre_defined qual array.
             * We are adding the qualifier for EPMF.
             */
            case CTEST_DNX_DIR_EXT_NEGATIVE_DIFFERENT_STAGE_QUAL:
            {
                negative_test_quals[0] =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LB_KEY);
                break;
            }
            /**
             * Indicating wrong stage.
             * The DE is supported only in IPMF2 and IPMF3!
             */
            case CTEST_DNX_DIR_EXT_NEGATIVE_ILLEGAL_STAGE:
            {
                field_stage = DNX_FIELD_STAGE_IPMF1;
                break;
            }
            default:
            {
             /**
              * None of the supported cases
              */
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Negative case:%d \n", negative_case_iterator);
                break;
            }
        }

        /**
         *  The begging of the region of error recovery transaction
         *  Used for clean-up in negative case
         */
        DNX_ROLLBACK_JOURNAL_START(unit);
        rv = ctest_dnx_dir_ext_sem_create_fg_and_attach(unit,
                                                        field_stage,
                                                        nof_iterations,
                                                        negative_case_iterator,
                                                        negative_test_actions, negative_test_quals, user_actions_p,
                                                        fg_info_p, attach_info_p, fg_id_p, &context_id);
       /**
        * The end of the region of error recovery transaction
        */
        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);

       /**
        * Giving the proper error message for different negative cases
        */
        if (rv == _SHR_E_PARAM)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s NEGATIVE TEST CASE: %d was successful! \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), negative_case_iterator);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Illegal FG from negative test case: %d was created! Test has failed!\r\n",
                         negative_case_iterator);
        }
        /**
         * Clear the arrays for next negative case
         */
        sal_memset(negative_test_quals, 0, sizeof(negative_test_quals));
        sal_memset(negative_test_actions, 0, sizeof(negative_test_actions));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function contains the Direct Extraction
 *   application.This function sets all required HW
 *   configuration for Direct Extraction to be performed.
 *
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_dir_ext_semantic_init(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    unsigned int dir_ext_semantic_test_type, dir_ext_semantic_test_stage;
    dnx_field_group_info_t fg_info;
    dnx_field_group_attach_info_t attach_info;
    dnx_field_group_t fg_id;
    /**
    * This variables stores the sevirity of the Field processor dnx
    */
    bsl_severity_t original_severity_fldprocdnx;
   /**
    * This variables stores the sevirity of the Resource manager
    */
    bsl_severity_t original_severity_resmngr;
    /**
     * This variables stores the sevirity of the DBAL
     */
    bsl_severity_t original_severity_dbaldnx;
    /**
     * This variables stores the sevirity of the HASH
     */
    bsl_severity_t original_severity_hashdnx;
    /**
     * This variables stores the sevirity of the SW State
     */
    bsl_severity_t original_severity_swstate;
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Since this procedure includes negative tests, error logs are expected.
     * To avoid seeing these messages, we store the original severity here and, in case of negative tests,
     * increase the severity to 'fatal' so that standard errors will not show on the screen.
     * Upon exit, we restore the original severity.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_resmngr);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, original_severity_hashdnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_ENUM(CTEST_DNX_DIR_EXT_SEMANTIC_OPTION_TYPE, dir_ext_semantic_test_type);
    SH_SAND_GET_ENUM(CTEST_DNX_DIR_EXT_SEMANTIC_OPTION_STAGE, dir_ext_semantic_test_stage);

    if (dir_ext_semantic_test_type == 0)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "%s(), line %d, Not implemented yet!: %s %s()\r\n",
                     __func__, __LINE__, "BCM", "dnx_field_dir_ext_semantic");
    }
    else if (dir_ext_semantic_test_type == 1)
    {
        /**
         * Calling the positive test function
         */
        SHR_IF_ERR_EXIT(appl_dnx_dir_ext_sem_positive_test(unit,
                                                           dir_ext_semantic_test_stage,
                                                           Ctest_dnx_dir_ext_semantic_quals,
                                                           Pre_defined_qual_array[dir_ext_semantic_test_stage - 1],
                                                           Ctest_dnx_dir_ext_semantic_actions,
                                                           &fg_info, &attach_info, &fg_id));
        /**
         * Increase the severity to 'fatal' to avoid seeing errors messages on the screen.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

        /**
         * Calling the negative test function
         */
        SHR_IF_ERR_EXIT(appl_dnx_dir_ext_sem_negative_test(unit,
                                                           dir_ext_semantic_test_stage,
                                                           Ctest_dnx_dir_ext_semantic_quals,
                                                           Pre_defined_qual_array[dir_ext_semantic_test_stage - 1],
                                                           Ctest_dnx_dir_ext_semantic_actions,
                                                           &fg_info, &attach_info, &fg_id));
        /**
         *  Restore the original severity after the end of Negative test.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_resmngr);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, original_severity_hashdnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    }
exit:
    /**
     *  Restore the original severity after the end of Negative test.
     */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_resmngr);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, original_severity_hashdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    SHR_FUNC_EXIT;
}
/**
 * \brief - run Direct Extraction sequence in diag shell
 */
shr_error_e
sh_dnx_dir_ext_sem_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SH_SAND_GET_UINT32(CTEST_DNX_DIR_EXT_SEMANTIC_OPTION_COUNT, count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_dir_ext_semantic_init(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}

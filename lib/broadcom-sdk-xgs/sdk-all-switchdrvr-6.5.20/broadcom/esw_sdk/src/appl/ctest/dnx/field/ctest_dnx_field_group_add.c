/** \file ctest_dnx_field_group_add.c
 * $Id$
 *
 * Semantic test for field_group_add DNX APIs.
 *
 */
/**
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

/**
 * Include files.
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include "ctest_dnx_field_group_add.h"
#include "ctest_dnx_field_utils.h"
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include "ctest_dnx_field_utils.h"
#include <bcm_int/dnx/field/field_init.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/sand/sand_signals.h>
/**
 * DEFINEs
 */
/**
 * This feature is not implemented yet
 */
#define CTEST_DNX_FIELD_GROUP_ADD_NOT_IMPLEMENTED_YET
/**
 * \brief
 *   Keyword for test type on 'group_add' command (data base testing)
 *   Type can be either BCM or DNX
 */
#define CTEST_DNX_FIELD_GROUP_ADD_OPTION_TEST_TYPE         "type"
/**
 * \brief
 *   Keyword for stage of test on 'group_add' command (data base testing)
 *   stage can be either IPMF1, IPMF2 ,IPMF3, EPMF .
 */

#define CTEST_DNX_FIELD_GROUP_ADD_OPTION_TEST_STAGE        "stage"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */

#define CTEST_DNX_FIELD_GROUP_ADD_OPTION_TEST_COUNT        "count"
/**
 * \brief
 *   Keyword for controlling whether to perform tracfic test. can be either 0 or 1.
 *   Only relevant for test type GROUP.
 */
#define CTEST_DNX_FIELD_GROUP_ADD_OPTION_TEST_TRAFFIC      "traffic"
/* *INDENT-OFF* */
/**
 * \brief
 *   Options list for 'group_add' shell command
 * \remark
 */
 sh_sand_option_t Sh_dnx_field_group_add_options[] = {
     /** Name */                                    /** Type */              /** Description */                                   /** Default */
    {CTEST_DNX_FIELD_GROUP_ADD_OPTION_TEST_TYPE,   SAL_FIELD_TYPE_ENUM,     "Type (level) of test (dnx or bcm)",                 "DNX",   (void *)Field_level_enum_table},
    {CTEST_DNX_FIELD_GROUP_ADD_OPTION_TEST_STAGE,  SAL_FIELD_TYPE_ENUM,     "Stage of test (ipmf1, ipmf2, ipmf3, epmf)",         "ipmf1", (void *)Field_stage_enum_table},
    {CTEST_DNX_FIELD_GROUP_ADD_OPTION_TEST_COUNT,  SAL_FIELD_TYPE_UINT32,   "Number of times test will run",                     "1"},
    {CTEST_DNX_FIELD_GROUP_ADD_OPTION_TEST_TRAFFIC,SAL_FIELD_TYPE_BOOL,     "Will the test indlude traffic test or not",         "No"},
    {NULL}
    /**
     *End of options list - must be last.
     */
};
/**
 * \brief
 *   List of tests for 'group_add' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_group_add_tests[] = {
    {"DNX_field_group_add_1", "type=DNX stage=ipmf1 count=1", CTEST_POSTCOMMIT},
    {"DNX_field_group_add_2", "type=DNX stage=ipmf2 count=1", CTEST_POSTCOMMIT},
    {"DNX_field_group_add_3", "type=DNX stage=ipmf3 count=1", CTEST_POSTCOMMIT},
    {"DNX_field_group_add_e", "type=DNX stage=epmf  count=1", CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/**
 * Details for semantic field group add test.
 */
sh_sand_man_t Sh_dnx_field_group_add_man = {
    "Field group add related test utilities",
    "Start a semantic test for field group, add, get, compare and delete with verification. "
        "Currently there is only DNX-level testing."
        "Testing may be for various stages: 'ipmf1' or 'ipmf2' or 'ipmf3' or 'epmf' \r\n"
        "'TRAffic' also performs traffic test.\r\n"
        "The 'count' variable defines how many times the test will run. \r\n",
    "ctest field group_add type=<BCM | DNX> stage=<IPMF1 | IPMF2 | IPMF3 | EPMF> count=1 traffic=<YES | NO>",
    "ctest field group_add type=DNX stage=IPMF1 traffic=Yes\r\n",
};

/**
 * Qualifiers per stage add up to 160b , so the FG we create with them will be 160b
 */
static dnx_field_qual_t
    Qual_array[CTEST_DNX_FIELD_GROUP_ADD_NOF_STAGES][CTEST_DNX_FIELD_GROUP_ADD_NOF_QUALS_AND_ACTIONS] = {
/**
 * IPMF1/2
 */
    {DNX_FIELD_IPMF1_QUAL_RPF_OUT_LIF, DNX_FIELD_IPMF1_QUAL_FWD_CONTEXT, DNX_FIELD_IPMF1_QUAL_IN_PORT_KEY_GEN_VAR},
/**
 * IPMF1/2
 */
    {DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_0, DNX_FIELD_IPMF2_QUAL_TRJ_HASH, DNX_FIELD_IPMF2_QUAL_NASID},
/**
 * IPMF3
 */
    {DNX_FIELD_IPMF3_QUAL_USER_HEADER_4, DNX_FIELD_IPMF3_QUAL_INGRESS_LEARN_ENABLE,
     DNX_FIELD_IPMF3_QUAL_IN_PORT_KEY_GEN_VAR},
/**
 * EPMF
 */
    {DNX_FIELD_EPMF_QUAL_TSH_EXT, DNX_FIELD_EPMF_QUAL_FTMH_SRC_SYS_PORT, DNX_FIELD_EPMF_QUAL_DST_SYS_PORT}
};

/**
 * Actions per stage add up to 32b , so the FG we create with them will be 32b
 */

static dnx_field_action_t
    Action_array[CTEST_DNX_FIELD_GROUP_ADD_NOF_STAGES][CTEST_DNX_FIELD_GROUP_ADD_NOF_QUALS_AND_ACTIONS] = {
/**
 * IPMF1/2
 */
    {DBAL_ENUM_FVAL_IPMF1_ACTION_DP, DBAL_ENUM_FVAL_IPMF1_ACTION_FWD, DBAL_ENUM_FVAL_IPMF1_ACTION_TC},
/**
 * IPMF1/2
 */
    {DBAL_ENUM_FVAL_IPMF1_ACTION_IN_TTL, DBAL_ENUM_FVAL_IPMF1_ACTION_ADMT_PROFILE,
     DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_OFFSET},
/**
 * IPMF3
 */
    {DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_OFFSET, DBAL_ENUM_FVAL_IPMF3_ACTION_DP,
     DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_0},
/**
 * EPMF
 */
    {DBAL_ENUM_FVAL_EPMF_ACTION_DISCARD, DBAL_ENUM_FVAL_EPMF_ACTION_TC, DBAL_ENUM_FVAL_EPMF_ACTION_DP}
};
/**
 * Qualifier class param per stage
 */
static dnx_field_qual_class_e Qual_class[CTEST_DNX_FIELD_GROUP_ADD_NOF_STAGES] =
    { DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_QUAL_CLASS_META,
    DNX_FIELD_QUAL_CLASS_META
};
/**
 * FOR NEGATIVE TESTING
 */
/**
 * Adding quals larger than 320b
 * Used for negative testing for all stages
 */
static dnx_field_qual_t
    Too_big_qual_array[CTEST_DNX_FIELD_GROUP_ADD_NOF_STAGES][CTEST_DNX_FIELD_GROUP_ADD_NOF_TOO_MANY_QUALS] = {
/**
 * IPMF1
 */
    {DNX_FIELD_IPMF1_QUAL_GENERAL_DATA_2, DNX_FIELD_IPMF1_QUAL_LEARN_INFO, DNX_FIELD_IPMF1_QUAL_IN_PORT_KEY_GEN_VAR,
     DNX_FIELD_IPMF1_QUAL_GENERAL_DATA_1},
/**
 * IPMF2
 */
    {DNX_FIELD_IPMF2_QUAL_KEY_F_INITIAL, DNX_FIELD_IPMF2_QUAL_KEY_G_INITIAL, DNX_FIELD_IPMF2_QUAL_PMF1_DIRECT_ACTION,
     DNX_FIELD_IPMF2_QUAL_KEY_H_INITIAL},
/**
 * IPMF3
 */
    {DNX_FIELD_IPMF3_QUAL_LEARN_INFO, DNX_FIELD_IPMF3_QUAL_SLB_KEY, DNX_FIELD_IPMF3_QUAL_FER_STATISTICS_OBJ,
     DNX_FIELD_IPMF3_QUAL_IN_PORT_KEY_GEN_VAR},
/**
 * EPMF
 */
    {DNX_FIELD_EPMF_QUAL_LEARN_EXT, DNX_FIELD_EPMF_QUAL_LIF_EXT, DNX_FIELD_EPMF_QUAL_UPDATED_TPIDS_PACKET_DATA,
     DNX_FIELD_EPMF_QUAL_USER_HEADER_1}
};
/**
 * Adding actions larger than 128b
 * Used for negative testing for all stages exept EPMF
 * (For EPMF the sum of all of the actions is 104b)
 */
static dnx_field_action_t
    Too_big_action_array[CTEST_DNX_FIELD_GROUP_ADD_NOF_STAGES][CTEST_DNX_FIELD_GROUP_ADD_NOF_TOO_MANY_ACTIONS] = {
/**
 * IPMF1/2
 */
    {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0, DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA, DBAL_ENUM_FVAL_IPMF1_ACTION_FWD,
     DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_2, DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_4,
     DBAL_ENUM_FVAL_IPMF1_ACTION_EEI},
/**
 * IPMF1/2
 */
    {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0, DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA, DBAL_ENUM_FVAL_IPMF1_ACTION_FWD,
     DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_2, DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_4,
     DBAL_ENUM_FVAL_IPMF1_ACTION_EEI},
/**
 * IPMF3
 */
    {DBAL_ENUM_FVAL_IPMF3_ACTION_DST_DATA, DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_0,
     DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_1, DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_2,
     DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_3, DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_1},
/**
 * EPMF
 */
    {DBAL_ENUM_FVAL_EPMF_ACTION_CUD_OUTLIF_OR_MCDB_PTR, DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PTR,
     DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PTR, DBAL_ENUM_FVAL_EPMF_ACTION_INVALID,
     DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PROFILE, DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_STRENGTH}
};
/**
 * Used for creating different cases with different
 * number of qualifiers
 */
static uint32 Nof_quals_per_case[NUM_CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_E] = {
    CTEST_DNX_FIELD_GROUP_ADD_NOF_QUALS_AND_ACTIONS,
    CTEST_DNX_FIELD_GROUP_ADD_ONLY_ONE_QUAL_AND_ACTION,
    CTEST_DNX_FIELD_GROUP_ADD_NOF_TOO_MANY_QUALS,
    CTEST_DNX_FIELD_GROUP_ADD_NOF_QUALS_AND_ACTIONS,
    CTEST_DNX_FIELD_GROUP_ADD_SAME_QUAL_AND_ACTIONS,
    CTEST_DNX_FIELD_GROUP_ADD_NOF_QUALS_AND_ACTIONS,
    CTEST_DNX_FIELD_GROUP_ADD_NOF_QUALS_AND_ACTIONS,
    CTEST_DNX_FIELD_GROUP_ADD_ONLY_ONE_QUAL_AND_ACTION
};
/**
 * Used for creating FG in different cases with different
 * number of actions
 */
static uint32 Nof_actions_per_case[NUM_CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_E] = {
    CTEST_DNX_FIELD_GROUP_ADD_NOF_QUALS_AND_ACTIONS,
    CTEST_DNX_FIELD_GROUP_ADD_ONLY_ONE_QUAL_AND_ACTION,
    CTEST_DNX_FIELD_GROUP_ADD_NOF_QUALS_AND_ACTIONS,
    CTEST_DNX_FIELD_GROUP_ADD_NOF_TOO_MANY_ACTIONS,
    CTEST_DNX_FIELD_GROUP_ADD_NOF_QUALS_AND_ACTIONS,
    CTEST_DNX_FIELD_GROUP_ADD_SAME_QUAL_AND_ACTIONS,
    CTEST_DNX_FIELD_GROUP_ADD_NOF_QUALS_AND_ACTIONS,
    CTEST_DNX_FIELD_GROUP_ADD_ONLY_ONE_QUAL_AND_ACTION
};
/**
 * An array, which contains all needed signal information for the traffic testing:
 * core, block, from, to, signal name, size of the buffer and
 * to match on the qualifier.
 * Expected value isn't taken from here, and is merely initialized.
 */
static dnx_field_utils_signal_info_t Packet_sig_info =
    { 0, "IRPP", "IPMF1", "", "tc", 1, CTEST_DNX_FIELD_GROUP_ADD_ACTION_VALUE };
/**
 *  The packet to be sent.
 */
static dnx_field_utils_packet_info_t Packet_info = {
    {"PTCH_2", "ETH1", "IPv4", "IPv4.SIP", "0.0.0.2", "IPv4.DIP", "0.0.0.1", ""}
};
/**
 * The IDs used for tcam_bank preallocation
 */
static int Tcam_bank_ids[CTEST_DNX_FIELD_GROUP_ADD_NOF_TCAM_BANKS] = { 2, 3 };
/**
 * \brief
 *
 *  This function contains the Field_group_add
 *   application.This function sets all required HW
 *   configuration for field_group_add to be performed.
 *
 * \param [in] unit - The unit number.
 * \param [in] group_add_flags - Flags of field group add API, look at
 *    dnx_field_group_add_flags_e
 * \param [in] field_stage - Indicate the PMF stage
 * \param [in] quals_p - qualifiers used for field_group_add
 *                    size of array depends on negative test flag,
 *                    it can be:
 *                              CTEST_DNX_FIELD_GROUP_ADD_NOF_QUALS_AND_ACTIONS
 *                                     for positive test and negative cases 1,3,4,5,6
 *                              CTEST_DNX_FIELD_GROUP_ADD_NOF_TOO_MANY_QUALS
 *                                      for negative case2 (too big key)
 *                              and 1 only for negative_case7 (not created user qual)
 * \param [in] actions_p - actions used for field_group_add
 *                              CTEST_DNX_FIELD_GROUP_ADD_NOF_QUALS_AND_ACTIONS
 *                                     for positive test and negative cases 1,2,4,5,6
 *                              CTEST_DNX_FIELD_GROUP_ADD_NOF_TOO_MANY_ACTIONS
 *                                      for negative case3 (too big action buffer)
 *                              and 1 only for negative_case7 (not created user qual)
 * \param [in] 
 * \param [in] test_flag - Used for negative cases
 *                          For details see 'ctest_dnx_field_group_add_test_flag_e'
 *                          in the ctest_dnx_field_group_add.h file
 * \param [in] nof_quals_per_case_p - pointer to array which is keeping the number
 *                           of qualifiers per case (positive or one of negative)
 * \param [in] nof_actions_per_case_p - pointer to array which is keeping the number
 *                           of actions per case (positive or one of negative)
 * \param [in] qual_class_p - pointer to array which is keeping the qual_class value
 *                           per stage
 * \param [in,out] fg_id_p - Pointer to Field group ID
 *           as in - in case flag WITH_ID is set, will hold the field group ID
 *           as out - in case flag WITH_ID is not set, will return the created field group ID
 * \param [out] fg_info_p - pointer to database info that is to be filled in by this procedure.
 *                          For details, see 'dnx_field_group_info_t'
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 *
 */
static shr_error_e
ctest_dnx_field_group_add_test_func(
    int unit,
    dnx_field_group_add_flags_e group_add_flags,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t * quals_p,
    dnx_field_action_t * actions_p,
    ctest_dnx_field_group_add_test_flag_e test_flag,
    uint32 *nof_quals_per_case_p,
    uint32 *nof_actions_per_case_p,
    dnx_field_qual_class_e * qual_class_p,
    dnx_field_group_t * fg_id_p,
    dnx_field_group_info_t * fg_info_p)
{
    int assignment_index;
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Pointer verify
     */
    SHR_NULL_CHECK(quals_p, _SHR_E_PARAM, "quals_p");
    SHR_NULL_CHECK(actions_p, _SHR_E_PARAM, "actions_p");
    SHR_NULL_CHECK(fg_info_p, _SHR_E_PARAM, "fg_info_p");
    SHR_NULL_CHECK(fg_id_p, _SHR_E_PARAM, "fg_id_p");
    SHR_NULL_CHECK(nof_quals_per_case_p, _SHR_E_PARAM, "nof_quals_per_case_p");
    SHR_NULL_CHECK(nof_actions_per_case_p, _SHR_E_PARAM, "nof_actions_per_case_p");
    /**
     * Load 'fg_info_in'
     */
    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, fg_info_p));
    /**
     * Select stage from dnx_field_stage_e
     */
    fg_info_p->field_stage = field_stage;
    /**
     * Select type from dnx_field_group_type_e
     */
    fg_info_p->fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    sal_strncpy(fg_info_p->name, "FG_name", DBAL_MAX_STRING_LENGTH);
    /**
     * Qualifiers type: dnx_field_qual_type_e
     * Invalid value  : DNX_FIELD_QUAL_TYPE_INVALID
     * Action type:     dnx_field_action_type_e
     * Invalid value  : DNX_FIELD_ACTION_INVALID
     * The invalid (DNX_FIELD_QUAL_TYPE_INVALID and DNX_FIELD_ACTION_INVALID)
     * is being set at the init function
     */

    for (assignment_index = 0; assignment_index < nof_quals_per_case_p[test_flag]; assignment_index++)
    {
        fg_info_p->dnx_quals[assignment_index] =
            DNX_QUAL(qual_class_p[field_stage], fg_info_p->field_stage, quals_p[assignment_index]);
    }
    for (assignment_index = 0; assignment_index < nof_actions_per_case_p[test_flag]; assignment_index++)
    {
        fg_info_p->dnx_actions[assignment_index] =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, actions_p[assignment_index]);
    }
    /**
     * Used for negative case7 (User qual which is not created yet)
     * In this case are used only one qual and one action
     */
    if (test_flag == CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_NOT_CREATED_USER_QUAL)
    {
        fg_info_p->dnx_quals[0] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_USER, field_stage, *quals_p);
        fg_info_p->dnx_actions[0] = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, actions_p[0]);
    }

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing group add with ID: %d \r\n",
                __func__, __LINE__, dnx_field_stage_text(unit, field_stage), *fg_id_p);
    if (test_flag == CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_POSITIVE)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_add(unit, group_add_flags, fg_info_p, fg_id_p));
    }
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
 * \param [in] set_fg_info_p - pointer to SET database info from field_group_add
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 *
 */
static shr_error_e
field_group_compare(
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
    if (strncmp
        ((char *) set_fg_info_p->name, (char *) get_fg_info.group_basic_info.name,
         BCM_FIELD_MAX_SHORT_NAME_LEN - 1) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "dnx_field_group_get: Unexpected data returned.\n"
                     "name expected %s received %s.\n", set_fg_info_p->name, get_fg_info.group_basic_info.name);
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
#ifndef CTEST_DNX_FIELD_GROUP_ADD_NOT_IMPLEMENTED_YET
    /**
     * Compare the set and get bank_allocation_mode
     */
    if (set_fg_info_p->tcam_info.bank_allocation_mode != get_fg_info.group_basic_info.tcam_info.bank_allocation_mode)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "bank_allocation_mode (%d) is not the same as bank_allocation_mode (%d)\r\n",
                     set_fg_info_p->tcam_info.auto_bank_allocation_mode
                     get_fg_info.group_basic_info.tcam_info.bank_allocation_mode);
    }
    /**
     * Compare the set and get nof_tcam_banks
     */
    if (set_fg_info_p->tcam_info.nof_tcam_banks != get_fg_info.group_basic_info.tcam_info.nof_tcam_banks)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "nof_tcam_banks (%d) is not the same as nof_tcam_banks (%d)\r\n",
                     set_fg_info_p->tcam_info.nof_tcam_banks, get_fg_info.group_basic_info.tcam_info.nof_tcam_banks);
    }
    /**
     * Iterate between all of tcam banks and compare the set and get tcam_bank_ids
     */
    for (fg_iterator = 0; fg_iterator < set_fg_info_p->tcam_info.nof_tcam_banks; fg_iterator++)
    {
        if (set_fg_info_p->tcam_info.tcam_bank_ids[fg_iterator] !=
            get_fg_info.group_basic_info.tcam_info.tcam_bank_ids[fg_iterator])
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "tcam_bank_id (%d) is not the same as tcam_bank_id (%d)\r\n",
                         set_fg_info_p->tcam_info.tcam_bank_ids[fg_iterator],
                         get_fg_info.group_basic_info.tcam_info.tcam_bank_ids[fg_iterator]);
        }
    }
#endif
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function is using the ctest_dnx_field_utils_packet_tx
 * utility for traffic sending and sand_signal_verify to verify that the signals
 * are the same as expacted
 * \param [in] unit - The unit number.
 * \param [in] expected_value - buffer with expected signal value,
 *  should be of type uint32.
 * \param [in] packet_info_p - Poiner to the packet,
 *                      which will be transmitted. For more info, please search for
 *                      ctest_dnx_field_utils_packet_tx() in ctest_dnx_field_utils.h
 * \param [in] sig_info_p - Pointer to an array, which contains all needed information
 *                      about signals verification: core, block, from, to,
 *                      size of the buffer and expected value.
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
ctest_dnx_group_add_traffic_test(
    int unit,
    uint32 expected_value,
    dnx_field_utils_packet_info_t * packet_info_p,
    dnx_field_utils_signal_info_t * sig_info_p)
{
    int rv;
    char *return_value;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Send traffic.
     */
    SHR_IF_ERR_EXIT(ctest_dnx_field_utils_packet_tx(unit, 201, packet_info_p));
    /*
     * If this is a retry, wait a specified amount of time.
     * Note we do not check the return value of sal_sleep().
     */
    sal_sleep(CTEST_DNX_FIELD_GROUP_ADD_PACKET_WAIT);
    return_value = NULL;
    rv = sand_signal_verify(unit, sig_info_p->core,
                            sig_info_p->block,
                            sig_info_p->from,
                            sig_info_p->to,
                            sig_info_p->sig_name, &(expected_value), sig_info_p->size, NULL, return_value, 0);
    SHR_IF_ERR_EXIT_WITH_LOG(rv, "FG traffic test failed. Result of signal verify is %s.%s%s",
                             return_value, EMPTY, EMPTY);
    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Value of signal \"%s\" is as expected! %s \r\n",
                __func__, __LINE__, return_value, EMPTY);
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *
 * This function sets all required HW
 *   configuration for field_group_add, context_attach and entry_add
 *   with TCAM preallocation to be performed.
 * The entry template is:
 *     IPV4 src = 0.0.0.2
 *     Action TC = 3
 *     Entry priority = 5
 * Also this function contains traffic for IPMF1 only because 
 *   the qualifiers for other stages are not 100% implemented yet.
 *
 * \param [in] unit - The unit number.
 * \param [in] group_add_flags - Flags of field group add API, look at
 *    dnx_field_group_add_flags_e
 * \param [in] field_stage - Indicate the PMF stage
 * \param [in] quals_p - pointer to array with 1 qualifier per stage
 *                       used for field_group_add for IPMF2,IPMF3 and EPMF
 *                       For IPMF1 the qualifier is hard coded
 * \param [in] actions_p - pointer to array with 1 action per stage
 *                       used for field_group_add for IPMF2,IPMF3 and EPMF
 *                       For IPMF1 the action is hard coded
 * \param [in] group_add_test_traffic - One of test options.
 *                           Shows if the test is with or without traffic.
 *                           if group_add_test_traffic = 1 The traffic is provided
 * \param [in] bank_allocation_mode - Used like flag.
 *            if bank_allocation_mode = SELECT, user needs to supply which banks to allocate
 *            if bank_allocation_mode = AUTO, nothing needs to be done
 * \param [in] tcam_bank_ids_p - Pointer to TCAM bank IDs
 * \param [in] packet_info_p - Poiner to the packet,
 *                      which will be transmitted. For more info, please search for
 *                      ctest_dnx_field_utils_packet_tx() in ctest_dnx_field_utils.h
 * \param [in] sig_info_p - Pointer to an array, which contains all needed information
 *                      about signals verification: core, block, from, to,
 *                      size of the buffer and expected value.
 * \param [in,out] fg_id_p - Pointer to Field group ID
 *           It is related to group_add_flags:
 *           as in - in case group_add_flags = DNX_FIELD_GROUP_ADD_FLAG_WITH_ID, will hold the field group ID
 *           as out - in case group_add_flags = 0, will return the created field group ID
 *
 * \param [out] fg_info_p - pointer to database info that is to be filled in by this procedure.
 *                          For details, see 'dnx_field_group_info_t'
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 *
 */
static shr_error_e
ctest_dnx_field_group_add_prealloc(
    int unit,
    dnx_field_group_add_flags_e group_add_flags,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t * quals_p,
    dnx_field_action_t * actions_p,
    int group_add_test_traffic,
    int bank_allocation_mode_flag,
    int *tcam_bank_ids_p,
    dnx_field_utils_packet_info_t * packet_info_p,
    dnx_field_utils_signal_info_t * sig_info_p,
    dnx_field_group_t * fg_id_p,
    dnx_field_group_info_t * fg_info_p)
{
    int assignment_index;
    uint8 is_allocated;
    dnx_field_group_attach_info_t attach_info;
    uint32 context_flags;
    dnx_field_entry_t entry_info;
    dnx_field_qual_t dnx_qual;
    dnx_field_action_t dnx_action;
    dnx_field_context_t context_id;
    uint32 entry_handle;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Load 'fg_info_in'
     */
    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, fg_info_p));
    /**
     * Select stage from dnx_field_stage_e
     */
    fg_info_p->field_stage = field_stage;
    /**
     * Select type from dnx_field_group_type_e
     */
    fg_info_p->fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    /**
     * Fill the dnx_field_group_tcam_info_t structure
     * Used for preallocation
     */
    /**
     * If the bank_allocation_mode_flag is different than 0
     * set the bank_allocation_mode to SELECT
     * Else set bank_allocation_mode to AUTO
     */
    if (bank_allocation_mode_flag != 0)
    {
        fg_info_p->tcam_info.bank_allocation_mode = DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT;
        /**
         * Set the number of TCAM banks
         */
        fg_info_p->tcam_info.nof_tcam_banks = CTEST_DNX_FIELD_GROUP_ADD_NOF_TCAM_BANKS;
        /**
         * Iterate between all of the tcam_banks and set the values
         */
        for (assignment_index = 0; assignment_index < CTEST_DNX_FIELD_GROUP_ADD_NOF_TCAM_BANKS; assignment_index++)
        {
            fg_info_p->tcam_info.tcam_bank_ids[assignment_index] = tcam_bank_ids_p[assignment_index];
        }
    }
    else
    {
        fg_info_p->tcam_info.bank_allocation_mode = DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO;
    }

    if (group_add_flags == DNX_FIELD_GROUP_ADD_FLAG_WITH_ID)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing group add with prealloc with ID: %d \r\n",
                    __func__, __LINE__, dnx_field_stage_text(unit, field_stage), *fg_id_p);
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing group add with prealloc without ID %s \r\n",
                    __func__, __LINE__, dnx_field_stage_text(unit, field_stage), EMPTY);
    }
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        /**
         * For IPMF1 are used hard coded values, because
         *  fixed entry is created used for sending a traffic.
         *  The entry is with:
         *  IPV4 src = 0.0.0.2
         *  Action TC = 3
         *
         */
        dnx_qual = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, field_stage, DNX_FIELD_HEADER_QUAL_IPV4_SRC);
        dnx_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_IPMF1_ACTION_TC);
        fg_info_p->dnx_actions[0] = dnx_action;
        fg_info_p->dnx_quals[0] = dnx_qual;
        /**
         * Init the attach_info structure
         */
        context_flags = 0;
        SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info));
        /**
         * Copy the FG info qualifiers and actions to the attach info
         */
        attach_info.dnx_quals[0] = dnx_qual;
        attach_info.dnx_actions[0] = dnx_action;
        /**
         * Give random mid-range priority, the value can be changed, it will not influence the test
         */
        attach_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 6);
        /**
         * Set the header qualifiers to use ABSOLUTE layer base - Legal hard coded values for Semantic testing
         */
        attach_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
        attach_info.qual_info[0].input_arg = 1;
        attach_info.qual_info[0].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;
        /**
         * Create the FG
         */
        SHR_IF_ERR_EXIT(dnx_field_group_add(unit, group_add_flags, fg_info_p, fg_id_p));
        /**
         * Attach the context to a FG
         */
        context_id = CTEST_DNX_FIELD_GROUP_ADD_FIELD_CONTEXT_ID;
        SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, context_flags, *fg_id_p, context_id, &attach_info));
        /**
         * Init the entry_info structure
         */
        SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
        /**
         * Fill the entry_info structure
         */
        entry_info.priority = CTEST_DNX_FIELD_GROUP_ADD_ENTRY_PRIORITY;
        entry_info.key_info.qual_info[0].dnx_qual = dnx_qual;
        entry_info.key_info.qual_info[0].qual_value[0] = 0x2;
        entry_info.key_info.qual_info[0].qual_mask[0] = 0xFFFFFFFF;
        entry_info.payload_info.action_info[0].dnx_action = dnx_action;
        entry_info.payload_info.action_info[0].action_value[0] = CTEST_DNX_FIELD_GROUP_ADD_ACTION_VALUE;
        /**
         * Create an entry
         */
        SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, *fg_id_p, &entry_info, &entry_handle));
        /**
         * Perform traffic test if required by 'traffic' option .
         */
        if (group_add_test_traffic)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_group_add_traffic_test
                            (unit, CTEST_DNX_FIELD_GROUP_ADD_ACTION_VALUE, packet_info_p, sig_info_p));
        }
    }
    else
    {
        /**
         * Qualifiers type: dnx_field_qual_type_e
         * Invalid value  : DNX_FIELD_QUAL_TYPE_INVALID
         * Action type:     dnx_field_action_type_e
         * Invalid value  : DNX_FIELD_ACTION_INVALID
         */
        fg_info_p->dnx_actions[0] = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, actions_p[0]);
        fg_info_p->dnx_quals[0] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, field_stage, quals_p[0]);
        /**
         * Call the API dnx_field_group_add
         */
        SHR_IF_ERR_EXIT(dnx_field_group_add(unit, group_add_flags, fg_info_p, fg_id_p));
    }
    /**
     * Delete the created FG
     */
    SHR_IF_ERR_EXIT(ctest_dnx_field_util_clean(unit));
    /**
     * Verify after delete
     */
    LOG_INFO_EX(BSL_LOG_MODULE,
                "%s(), line %d, Stage: %s .Performing compare after delete of the FG with ID: %d \r\n",
                __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), *fg_id_p);
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, *fg_id_p, &is_allocated));
    if (!is_allocated)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s FG: %d has been deleted successfully \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), *fg_id_p);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Field group (%d) exists after having been deleted!\r\n", *fg_id_p);
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function is the basic 'field_group_add'
 *   testing application.
 *
 *  \param [in] unit - The unit number.
 *  \param [in] args - Null terminated string.
 *             'Options': List of arguments as shown on screen (typed by caller/user)
 *  \param [in] sand_control - Control information related
 *              to each of the 'options' entered by the caller (and contained in 'args')
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 *
 *  The test flow:
 *  1. Create a field group (without ID)
 *      with the field_group_add function
 *  2. Compare the added fields and the get fields
 *  3. delete the field group
 *  4. Verify after delete
 *  5. Create the field group with ID and do 2-6 for it
 *  6. Create 2 FGs with preallocation and send traffic for IPMF1
 *  7. Negative testing: 7 different cases
 *    7.1 Illegal FG id
 *    7.2 Too big key
 *    7.3 Too big action buffer
 *    7.4 Using same qual twice
 *    7.5 Using same action twice
 *    7.6 Encoding one qual with wrong stage
 *    7.7 Using user qual that is not created
 */
static shr_error_e
appl_dnx_field_group_add_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e field_stage;
    int field_group_test_type_name;
    dnx_field_group_info_t fg_info;
    dnx_field_group_add_flags_e group_add_flags;
    dnx_field_group_t fg_id;
    int group_add_test_traffic;
    /**
     * Used for case4 (Using same qual twice)
     */
    dnx_field_qual_t quals[CTEST_DNX_FIELD_GROUP_ADD_SAME_QUAL_AND_ACTIONS];
    /**
     * Used for case5 (Using same action twice)
     */
    dnx_field_action_t actions[CTEST_DNX_FIELD_GROUP_ADD_SAME_QUAL_AND_ACTIONS];
    /**
     * This variable is used for negative case7
     * (User qualifier which is not created)
     */
    dnx_field_qual_t user_qual_invalid_id;
    int auto_select_mode_flag;
    int fg_itterator;
    int assignment_index;
    int rv;
    /**
     * This variables stores the severity of the Field processor dnx
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
     * This variables stores the sevirity of the swstate
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
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    /**
     * User qualifier 110 does not exist
     */
    user_qual_invalid_id = 110;
    /**
     * Giving the default value of the flag. 0 means it is not in the auto select mode.
     */
    auto_select_mode_flag = FALSE;
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_GROUP_ADD_OPTION_TEST_TYPE, field_group_test_type_name);
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_GROUP_ADD_OPTION_TEST_STAGE, field_stage);
    SH_SAND_GET_BOOL(CTEST_DNX_FIELD_GROUP_ADD_OPTION_TEST_TRAFFIC, group_add_test_traffic);
    /*
     * For now, a fail to match the input parameter result by just a message.
     * Procedure returns with 'success'.
     */
    if (field_group_test_type_name == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, %s level has not been implemented yet for %s. \r\n",
                    __func__, __LINE__, "BCM", dnx_field_stage_e_get_name(field_stage));
    }
    else if (field_group_test_type_name == 1)
    {
        group_add_flags = 0;
        /**
         *  1. Create a field group (without ID)
         *      with the field_group_add function
         *  2. Compare the added fields and the get fields
         *  3. delete the field group
         *  4. Verify after delete
         *  5. Create the field group with ID and do 2-6 for it
         */
        for (fg_itterator = 0; fg_itterator < CTEST_DNX_FIELD_GROUP_ADD_NOF_FG; fg_itterator++)
        {
            uint8 is_allocated;
            /*
             * Creating group without ID
             */
            SHR_IF_ERR_EXIT(ctest_dnx_field_group_add_test_func
                            (unit, group_add_flags, field_stage, Qual_array[field_stage], Action_array[field_stage],
                             CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_POSITIVE,
                             Nof_quals_per_case, Nof_actions_per_case, Qual_class, &fg_id, &fg_info));
            /*
             * Compare the get elements and the set elements
             */
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Stage: %s .Performing compare between the add and get: %d \r\n", __func__,
                        __LINE__, dnx_field_stage_e_get_name(field_stage), fg_id);
            SHR_IF_ERR_EXIT(field_group_compare(unit, fg_id, &fg_info));
            /**
             * Delete the created FG
             */
            SHR_IF_ERR_EXIT(ctest_dnx_field_util_clean(unit));
            /**
             * Verify after delete
             */
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Stage: %s .Performing compare after delete of the FG with ID: %d \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), fg_id);
            SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
            if (!is_allocated)
            {
                LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s FG: %d has been deleted successfully \r\n",
                            __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), fg_id);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Field group (%d) exists after having been deleted!\r\n", fg_id);
            }
            /**
             * Creating group with ID for next iteration
             */
            group_add_flags = DNX_FIELD_GROUP_ADD_FLAG_WITH_ID;
            fg_id = CTEST_DNX_FIELD_GROUP_ADD_FIELD_GROUP_ID;
        }
        /**
         * Group with PREALLOCAION
         */
        group_add_flags = 0;
        for (fg_itterator = 0; fg_itterator < CTEST_DNX_FIELD_GROUP_ADD_NOF_TCAM_BANKS; fg_itterator++)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_field_group_add_prealloc(unit, group_add_flags,
                                                               field_stage, Qual_array[field_stage],
                                                               Action_array[field_stage], group_add_test_traffic,
                                                               auto_select_mode_flag, Tcam_bank_ids,
                                                               &Packet_info, &Packet_sig_info, &fg_id, &fg_info));
            /**
             * Creating an bank with auto selected ID for next iteration
             */
            auto_select_mode_flag = TRUE;
        }
        /**
         * NEGATIVE TESTING
         * {
         */
        /**
         * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
        /**
         * Case1: Illegal FG ID
         * FG out of the range [0:127]
         */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s NEGATIVE TESTING: CASE1 (ILLEGAL FG ID) %s \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY);
        group_add_flags = DNX_FIELD_GROUP_ADD_FLAG_WITH_ID;

        DNX_ROLLBACK_JOURNAL_START(unit);

        fg_id = CTEST_DNX_FIELD_GROUP_ADD_INVALID_FG_ID;
        rv = ctest_dnx_field_group_add_test_func(unit, group_add_flags, field_stage, Qual_array[field_stage],
                                                 Action_array[field_stage],
                                                 CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_ILLEGAL_FG_ID,
                                                 Nof_quals_per_case, Nof_actions_per_case, Qual_class,
                                                 &fg_id, &fg_info);

        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);

        if (rv == _SHR_E_PARAM)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Stage: %s NEGATIVE TEST CASE1 (illegal FG ID: %d) was successful! \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), fg_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Field group (%d) was created although a 'Illegal FG ID' was used. Test has failed!\r\n",
                         fg_id);
        }
        /**
         * Case2: Too big key
         *  Using Too_big_qual_array which contains qualifiers with amount more then 320
         */
        group_add_flags = 0;
        fg_id = 0;
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %s NEGATIVE TESTING: CASE2 (TOO MANY QUALS) %s \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY);

        DNX_ROLLBACK_JOURNAL_START(unit);

        rv = ctest_dnx_field_group_add_test_func(unit, group_add_flags, field_stage, Too_big_qual_array[field_stage],
                                                 Action_array[field_stage],
                                                 CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_TOO_BIG_KEY,
                                                 Nof_quals_per_case, Nof_actions_per_case, Qual_class,
                                                 &fg_id, &fg_info);

        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);

        if (rv == _SHR_E_PARAM)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Stage: %s  NEGATIVE TEST CASE2 (Too big key) was successful! %s \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Field group (%d) was created although a 'too big key' was used. Test has failed!\r\n", fg_id);
        }
       /**
        * Case3: Too big action buffer
        * using Too_big_action_array to create too big action buffer
        */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s NEGATIVE TESTING: CASE3 (TOO MANY ACTIONS) %s \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY);
        if (field_stage == DNX_FIELD_STAGE_EPMF)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, The NEGATIVE CASE3 for %s can't be implemented. The max sum of all actions is 104b! %s \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY);
        }
        else
        {

            DNX_ROLLBACK_JOURNAL_START(unit);

            rv = ctest_dnx_field_group_add_test_func(unit, group_add_flags, field_stage, Qual_array[field_stage],
                                                     Too_big_action_array[field_stage],
                                                     CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_TOO_BIG_ACTION_BUFFER,
                                                     Nof_quals_per_case, Nof_actions_per_case, Qual_class,
                                                     &fg_id, &fg_info);

            DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);

            if (rv == _SHR_E_PARAM)
            {
                LOG_INFO_EX(BSL_LOG_MODULE,
                            "%s(), line %d, Stage: %s  NEGATIVE TEST CASE3 (Too big action buffer) was successful! %s \r\n",
                            __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "Filed group (%d) was created although a 'Too big action buffer' was used!The test has failed!\r\n",
                             fg_id);
            }
        }
       /**
        * Case4: Using same qualifier
        * Getting the qual with index 0 from the Qual_array more then once
        */
        fg_id = 0;
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s NEGATIVE TESTING: CASE4 (USING SAME QUAL) %s \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY);

        for (assignment_index = 0; assignment_index < CTEST_DNX_FIELD_GROUP_ADD_SAME_QUAL_AND_ACTIONS;
             assignment_index++)
        {
            quals[assignment_index] = Qual_array[field_stage][0];
        }

        DNX_ROLLBACK_JOURNAL_START(unit);

        rv = ctest_dnx_field_group_add_test_func(unit, group_add_flags, field_stage, quals, Action_array[field_stage],
                                                 CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_USING_SAME_QUAL_TWICE,
                                                 Nof_quals_per_case, Nof_actions_per_case, Qual_class,
                                                 &fg_id, &fg_info);

        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);

        if (rv == _SHR_E_PARAM)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Stage: %s  NEGATIVE TEST CASE4 (Using the same qual twice) was successful! %s \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Filed group (%d) using same qual twice was created!The test has failed!\r\n",
                         fg_id);
        }
       /**
        * Case5: Using same action more then once
        * Getting the action with index 0 from the Action_array more then once
        */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s NEGATIVE TESTING: CASE5 (USING SAME ACTION) %s \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY);
        for (assignment_index = 0; assignment_index < CTEST_DNX_FIELD_GROUP_ADD_SAME_QUAL_AND_ACTIONS;
             assignment_index++)
        {
            actions[assignment_index] = Action_array[field_stage][0];
        }

        DNX_ROLLBACK_JOURNAL_START(unit);

        rv = ctest_dnx_field_group_add_test_func(unit, group_add_flags, field_stage, Qual_array[field_stage], actions,
                                                 CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_USING_SAME_ACTION_TWICE,
                                                 Nof_quals_per_case, Nof_actions_per_case, Qual_class,
                                                 &fg_id, &fg_info);

        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);

        if (rv == _SHR_E_PARAM)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Stage: %s  NEGATIVE TEST CASE5 (Using same action twice) was successful! %s \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Filed group (%d) using same action twice was created!The test has failed!\r\n",
                         fg_id);
        }
       /**
        * Delete the created FG
        */

#ifndef CTEST_DNX_FIELD_GROUP_ADD_NOT_IMPLEMENTED_YET
       

        DNX_ROLLBACK_JOURNAL_START(unit);

        SHR_IF_ERR_EXIT(ctest_dnx_field_group_add_test_func
                        (unit, group_add_flags, field_stage, Qual_array[field_stage + 1], Action_array[field_stage],
                         CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_WRONG_STAGE_QUAL,
                         Nof_quals_per_case, Nof_actions_per_case, Qual_class, &fg_id, &fg_info));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %s NEGATIVE TESTING: CASE6 (WRONG QUAL STAGE) FOR FG WITH ID: %d \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), fg_id);

        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);

       /**
        * Delete the created FG
        */

#endif
       /**
        * Case7: Using user qualifier that is not created
        */
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %s NEGATIVE TESTING: CASE7 (USING NOT CREATED USER QUAL)%s \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY);

        DNX_ROLLBACK_JOURNAL_START(unit);

        rv = ctest_dnx_field_group_add_test_func(unit, group_add_flags, field_stage, &user_qual_invalid_id,
                                                 Action_array[field_stage],
                                                 CTEST_DNX_FIELD_GROUP_ADD_TEST_FLAG_NOT_CREATED_USER_QUAL,
                                                 Nof_quals_per_case, Nof_actions_per_case, Qual_class,
                                                 &fg_id, &fg_info);

        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);

        if (rv == _SHR_E_PARAM)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Stage: %s  NEGATIVE TEST CASE7 (Using user qualifier that is not created) was successful! %s \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Filed group (%d) using user qualifier that is not created was created!The test has failed!\r\n",
                         fg_id);
        }

        /**
        * Delete the created FG
        */

        /**
         *  Restore the original severity after the end of Negative test.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_resmngr);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
        /**
         * }
         */
    }
exit:

    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_resmngr);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    run TCAM Look-up sequence in diag shell
 * \param [in] unit - the unit number in system
 * \param [in] args - pointer to args_t, not passed down
 * \param [in] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 */
shr_error_e
sh_dnx_field_group_add_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SH_SAND_GET_UINT32(CTEST_DNX_FIELD_GROUP_ADD_OPTION_TEST_COUNT, count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));
        SHR_IF_ERR_EXIT(appl_dnx_field_group_add_run(unit, args, sand_control));
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 0));
    }
exit:
    SHR_FUNC_EXIT;
}

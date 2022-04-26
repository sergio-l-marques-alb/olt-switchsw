/**
 * $Id: ctest_dnx_field_efes.c,v 1.20 Broadcom SDK $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * \file ctest_dnx_field_efes.c
 *
 * EFES Algorithm testing.
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
#include "ctest_dnx_field_efes.h"
#include "ctest_dnx_field_utils.h"
#include <appl/diag/dnx/diag_dnx_field.h>

/*
 * }
 */

/* *INDENT-OFF* */

/**
 * \brief
 *   Options list for 'EFES' shell command
 * \remark
 */
sh_sand_option_t Sh_dnx_field_efes_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'EFES' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_efes_tests[] = {
    {"EFES_algo", "", CTEST_POSTCOMMIT},
    {NULL}
};

/**
 *  EFES shell command leaf details
 */
sh_sand_man_t Sh_dnx_field_efes_man = {
    .brief =
        "Field EFES Algorithm tests",
    .full =
        "This Ctest presents different positive and negative scenarios.\n"
        "Creating contexts and field groups with UDH dummy qualifier and\n"
        "different actions to show EFES usage for all different scenarios.\n",
    .synopsis =
        ""
};

static bcm_field_qualify_t Ctest_dnx_field_efes_user_qual_id = bcmFieldQualifyCount;

/** Context Selection info per field stage (IPMF1-2-3 and EPMF). */
static ctest_dnx_field_efes_cs_info_t Ctest_dnx_field_efes_cs_info_array[CTEST_DNX_FIELD_EFES_NOF_CS] = {
/** context_id  | context_name | presel_id */
    {0, "efes_cs0", 50},    {0, "efes_cs1", 51},    {0, "efes_cs2", 52},    {0, "efes_cs3", 52},
    {0, "efes_cs4", 52},    {0, "efes_cs5", 52},    {0, "efes_cs6", 52},    {0, "efes_cs7", 52},
    {0, "efes_cs8", 52},    {0, "efes_cs9", 52},    {0, "efes_cs10", 52},    {0, "efes_cs11", 52},
    {0, "efes_cs12", 52},    {0, "efes_cs13", 52},    {0, "efes_cs14", 52},    {0, "efes_cs15", 52},
    {0, "efes_cs16", 52},    {0, "efes_cs17", 52},    {0, "efes_cs18", 52},    {0, "efes_cs19", 52},
    {0, "efes_cs20", 52},    {0, "efes_cs21", 52},    {0, "efes_cs22", 52},    {0, "efes_cs23", 52},
    {0, "efes_cs24", 52},    {0, "efes_cs25", 52},    {0, "efes_cs26", 52},    {0, "efes_cs27", 52},
    {0, "efes_cs28", 52},    {0, "efes_cs29", 52},    {0, "efes_cs30", 52},    {0, "efes_cs31", 52}
};

/**
 * Following arrays are global, used for all test cases. They contain information for all actions needed for
 * the different test cases (positive and negative).
 */
static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_position_action_info_array[CTEST_DNX_FIELD_EFES_POS_PRIO_NOF_ACTIONS] = {
    {NULL, bcmFieldActionPrioIntNew, {PRIORITY_BY_POSITION, 0, 2, 2}, {0}},
    {NULL, bcmFieldActionDropPrecedence, {PRIORITY_BY_POSITION, 0, 3, 3}, {0}},
    {NULL, bcmFieldActionTrap, {PRIORITY_BY_POSITION, 2, 2, 18}, {0}},
    {NULL, bcmFieldActionSnoop, {PRIORITY_BY_POSITION, 2, 3, 19}, {0}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_priority_action_info_array[CTEST_DNX_FIELD_EFES_POS_PRIO_NOF_ACTIONS] = {
    {NULL, bcmFieldActionPrioIntNew, {PRIORITY_BY_NUM_PRIO, 0, 10, 0}, {0}},
    {NULL, bcmFieldActionDropPrecedence, {PRIORITY_BY_NUM_PRIO, 0, 15, 1}, {0}},
    {NULL, bcmFieldActionTrap, {PRIORITY_BY_NUM_PRIO, 2, 10, 16}, {0}},
    {NULL, bcmFieldActionSnoop, {PRIORITY_BY_NUM_PRIO, 2, 15, 17}, {0}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_sharing_first_action_info_array[CTEST_DNX_FIELD_EFES_SHARING_NOF_ACTIONS] = {
    {NULL, bcmFieldActionPrioIntNew, {PRIORITY_BY_NUM_PRIO, 0, 2, 0}, {0}},
    {NULL, bcmFieldActionDropPrecedence, {PRIORITY_BY_NUM_PRIO, 0, 8, 1}, {0}},
    {NULL, bcmFieldActionTrap, {PRIORITY_BY_NUM_PRIO, 0, 8, 2}, {0}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_sharing_second_action_info_array[CTEST_DNX_FIELD_EFES_SHARING_NOF_ACTIONS] = {
    {NULL, bcmFieldActionPrioIntNew, {PRIORITY_BY_NUM_PRIO, 0, 3, 0}, {0}},
    {NULL, bcmFieldActionDropPrecedence, {PRIORITY_BY_NUM_PRIO, 0, 11, 1}, {0}},
    {NULL, bcmFieldActionTrap, {PRIORITY_BY_NUM_PRIO, 0, 12, 2}, {0}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_dont_care_first_action_info_array[CTEST_DNX_FIELD_EFES_DONT_CARE_NOF_ACTIONS] = {
    {"dont_care_0", 0, {PRIORITY_DONT_CARE, 0, 0, 0}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"dont_care_1", 0, {PRIORITY_DONT_CARE, 0, 0, 1}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"dont_care_2", 0, {PRIORITY_DONT_CARE, 0, 0, 2}, {0, bcmFieldActionPphPresentSet, 1, 1, 0x1, {0}}},
    {"dont_care_3", 0, {PRIORITY_DONT_CARE, 0, 0, 3}, {0, bcmFieldActionInVportClass0, 7, 1, 0x1, {0}}},
    {"dont_care_4", 0, {PRIORITY_DONT_CARE, 0, 0, 4}, {0, bcmFieldActionUDHBase0, 1, 1, 0x1, {0}}},
    {"dont_care_5", 0, {PRIORITY_DONT_CARE, 0, 0, 5}, {0, bcmFieldActionUDHBase1, 1, 1, 0x1, {0}}},
    {"dont_care_6", 0, {PRIORITY_DONT_CARE, 0, 0, 6}, {0, bcmFieldActionUDHBase2, 1, 1, 0x1, {0}}},
    {"dont_care_7", 0, {PRIORITY_DONT_CARE, 0, 0, 7}, {0, bcmFieldActionUDHBase3, 1, 1, 0x1, {0}}},
    {"dont_care_8", 0, {PRIORITY_DONT_CARE, 0, 0, 8}, {0, bcmFieldActionStatProfile0, 4, 1, 0x1, {0}}},
    {"dont_care_9", 0, {PRIORITY_DONT_CARE, 0, 0, 9}, {0, bcmFieldActionStatProfile1, 4, 1, 0x1, {0}}},
    {"dont_care_10", 0, {PRIORITY_DONT_CARE, 0, 0, 10}, {0, bcmFieldActionStatProfile2, 4, 1, 0x1, {0}}},
    {"dont_care_11", 0, {PRIORITY_DONT_CARE, 0, 0, 11}, {0, bcmFieldActionIPTProfile, 2, 1, 0x1, {0}}},
    {"dont_care_12", 0, {PRIORITY_DONT_CARE, 0, 0, 12}, {0, bcmFieldActionFabricHeaderSet, 3, 1, 0x1, {0}}},
    {"dont_care_13", 0, {PRIORITY_DONT_CARE, 0, 0, 13}, {0, bcmFieldActionOuterVlanPrioNew, 3, 1, 0x1, {0}}},
    {"dont_care_14", 0, {PRIORITY_DONT_CARE, 0, 0, 14}, {0, bcmFieldActionAdmitProfile, 2, 1, 0x1, {0}}},
    {"dont_care_15", 0, {PRIORITY_DONT_CARE, 0, 0, 15}, {0, bcmFieldActionPphSnoopCode, 2, 1, 0x1, {0}}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_dont_care_position_action_info_array[CTEST_DNX_FIELD_EFES_DONT_CARE_POS_PRIO_NOF_ACTIONS] = {
    {"d_care_pos", 0, {PRIORITY_BY_POSITION, 0, 0, 0}, {0, bcmFieldActionUDHData0, 2, 30, 0x1, {0}}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_dont_care_priority_action_info_array[CTEST_DNX_FIELD_EFES_DONT_CARE_POS_PRIO_NOF_ACTIONS] = {
    {"d_care_prio", 0, {PRIORITY_BY_NUM_PRIO, 0, 15, 0}, {0, bcmFieldActionUDHData1, 2, 30, 0x1, {0}}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_combination_first_action_info_array[CTEST_DNX_FIELD_EFES_COMB_FIRST_NOF_ACTIONS] = {
    {"efes_comb_0", 0, {PRIORITY_BY_NUM_PRIO, 0, 10, 0}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"efes_comb_1", 0, {PRIORITY_BY_NUM_PRIO, 0, 15, 1}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"efes_comb_3", 0, {PRIORITY_BY_NUM_PRIO, 0, 20, 2}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"efes_comb_4", 0, {PRIORITY_BY_NUM_PRIO, 0, 30, 3}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"efes_comb_2", 0, {PRIORITY_BY_POSITION, 0, 6, 6}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"efes_comb_5", 0, {PRIORITY_DONT_CARE, 0, 0, 7}, {0, bcmFieldActionStatId0, 19, 1, 0x1, {0}}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_combination_second_action_info_array[CTEST_DNX_FIELD_EFES_COMB_SECOND_NOF_ACTIONS] = {
    {"efes_comb_6", 0, {PRIORITY_BY_POSITION, 0, 4, 4}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"efes_comb_7", 0, {PRIORITY_BY_POSITION, 0, 5, 5}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_exhaustive_prio_action_info_array[CTEST_DNX_FIELD_EFES_EXHAUSTIVE_NOF_ACTIONS] = {
    {"exhaustive_prio_0", 0, {PRIORITY_BY_NUM_PRIO, 0, 0, 0}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_1", 0, {PRIORITY_BY_NUM_PRIO, 0, 10, 1}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_2", 0, {PRIORITY_BY_NUM_PRIO, 0, 20, 2}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_3", 0, {PRIORITY_BY_NUM_PRIO, 0, 30, 3}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_4", 0, {PRIORITY_BY_NUM_PRIO, 0, 40, 4}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_5", 0, {PRIORITY_BY_NUM_PRIO, 0, 50, 5}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_6", 0, {PRIORITY_BY_NUM_PRIO, 0, 60, 6}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_7", 0, {PRIORITY_BY_NUM_PRIO, 0, 70, 7}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_8", 0, {PRIORITY_BY_NUM_PRIO, 0, 80, 8}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_9", 0, {PRIORITY_BY_NUM_PRIO, 0, 90, 9}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_10", 0, {PRIORITY_BY_NUM_PRIO, 0, 100, 10}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_11", 0, {PRIORITY_BY_NUM_PRIO, 0, 110, 11}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_12", 0, {PRIORITY_BY_NUM_PRIO, 0, 120, 12}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_13", 0, {PRIORITY_BY_NUM_PRIO, 0, 130, 13}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_14", 0, {PRIORITY_BY_NUM_PRIO, 0, 140, 14}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_15", 0, {PRIORITY_BY_NUM_PRIO, 0, 150, 15}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_exhaustive_pos_action_info_array[CTEST_DNX_FIELD_EFES_EXHAUSTIVE_NOF_ACTIONS] = {
    {"exhaustive_pos_0", 0, {PRIORITY_BY_POSITION, 2, 0, 16}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_1", 0, {PRIORITY_BY_POSITION, 2, 1, 17}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_2", 0, {PRIORITY_BY_POSITION, 2, 2, 18}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_3", 0, {PRIORITY_BY_POSITION, 2, 3, 19}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_4", 0, {PRIORITY_BY_POSITION, 2, 4, 20}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_5", 0, {PRIORITY_BY_POSITION, 2, 5, 21}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_6", 0, {PRIORITY_BY_POSITION, 2, 6, 22}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_7", 0, {PRIORITY_BY_POSITION, 2, 7, 23}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_8", 0, {PRIORITY_BY_POSITION, 2, 8, 24}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_9", 0, {PRIORITY_BY_POSITION, 2, 9, 25}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_10", 0, {PRIORITY_BY_POSITION, 2, 10, 26}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_11", 0, {PRIORITY_BY_POSITION, 2, 11, 27}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_12", 0, {PRIORITY_BY_POSITION, 2, 12, 28}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_13", 0, {PRIORITY_BY_POSITION, 2, 13, 29}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_14", 0, {PRIORITY_BY_POSITION, 2, 14, 30}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_15", 0, {PRIORITY_BY_POSITION, 2, 15, 31}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_exhaustive_pos_ipmf3_epmf_action_info_array[CTEST_DNX_FIELD_EFES_EXHAUSTIVE_IPMF3_NOF_ACTIONS_PER_FG] = {
    {"exhaustive_pos_0", 0, {PRIORITY_BY_POSITION, 0, 8, 8}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_1", 0, {PRIORITY_BY_POSITION, 0, 9, 9}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_2", 0, {PRIORITY_BY_POSITION, 0, 10, 10}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_3", 0, {PRIORITY_BY_POSITION, 0, 11, 11}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_4", 0, {PRIORITY_BY_POSITION, 0, 12, 12}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_5", 0, {PRIORITY_BY_POSITION, 0, 13, 13}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_6", 0, {PRIORITY_BY_POSITION, 0, 14, 14}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_7", 0, {PRIORITY_BY_POSITION, 0, 15, 15}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_exhaustive_pos_epmf_action_info_array[CTEST_DNX_FIELD_EFES_EXHAUSTIVE_EPMF_NOF_ACTIONS_PER_FG] = {
    {"exhaustive_pos_0", 0, {PRIORITY_BY_POSITION, 0, 6, 6}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_1", 0, {PRIORITY_BY_POSITION, 0, 7, 7}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_2", 0, {PRIORITY_BY_POSITION, 0, 8, 8}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_3", 0, {PRIORITY_BY_POSITION, 0, 9, 9}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_4", 0, {PRIORITY_BY_POSITION, 0, 10, 10}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
    {"exhaustive_pos_5", 0, {PRIORITY_BY_POSITION, 0, 11, 11}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_exhaustive_pgm_action_info_array[CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS] = {
    {NULL, bcmFieldActionPrioIntNew, {PRIORITY_BY_NUM_PRIO, 0, 15, 0}, {0}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_exhaustive_masks_action_info_array[CTEST_DNX_FIELD_EFES_EXHAUSTIVE_NOF_ACTIONS] = {
    {"exhaustive_prio_16", 0, {PRIORITY_BY_NUM_PRIO, 0, 0, 0}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_17", 0, {PRIORITY_BY_NUM_PRIO, 0, 10, 1}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_18", 0, {PRIORITY_BY_NUM_PRIO, 0, 20, 2}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_19", 0, {PRIORITY_BY_NUM_PRIO, 0, 30, 3}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_20", 0, {PRIORITY_BY_NUM_PRIO, 0, 40, 4}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_21", 0, {PRIORITY_BY_NUM_PRIO, 0, 50, 5}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_22", 0, {PRIORITY_BY_NUM_PRIO, 0, 60, 6}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_23", 0, {PRIORITY_BY_NUM_PRIO, 0, 70, 7}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_24", 0, {PRIORITY_BY_NUM_PRIO, 0, 80, 8}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_25", 0, {PRIORITY_BY_NUM_PRIO, 0, 90, 9}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_26", 0, {PRIORITY_BY_NUM_PRIO, 0, 100, 10}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_27", 0, {PRIORITY_BY_NUM_PRIO, 0, 110, 11}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_28", 0, {PRIORITY_BY_NUM_PRIO, 0, 120, 12}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_29", 0, {PRIORITY_BY_NUM_PRIO, 0, 130, 13}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_30", 0, {PRIORITY_BY_NUM_PRIO, 0, 140, 14}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_31", 0, {PRIORITY_BY_NUM_PRIO, 0, 150, 15}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_exhaustive_masks_fg3_action_info_array[CTEST_DNX_FIELD_EFES_EXHAUSTIVE_MASKS_FG3_NOF_ACTIONS] = {
    {"exhaustive_prio_32", 0, {PRIORITY_BY_NUM_PRIO, 0, 0, 0}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_33", 0, {PRIORITY_BY_NUM_PRIO, 0, 10, 1}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_34", 0, {PRIORITY_BY_NUM_PRIO, 0, 20, 2}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_35", 0, {PRIORITY_BY_NUM_PRIO, 0, 30, 3}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_36", 0, {PRIORITY_BY_NUM_PRIO, 0, 40, 4}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_37", 0, {PRIORITY_BY_NUM_PRIO, 0, 50, 5}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_38", 0, {PRIORITY_BY_NUM_PRIO, 0, 60, 6}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_39", 0, {PRIORITY_BY_NUM_PRIO, 0, 70, 7}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_40", 0, {PRIORITY_BY_NUM_PRIO, 0, 80, 8}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_41", 0, {PRIORITY_BY_NUM_PRIO, 0, 90, 9}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_42", 0, {PRIORITY_BY_NUM_PRIO, 0, 100, 10}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_43", 0, {PRIORITY_BY_NUM_PRIO, 0, 110, 11}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"exhaustive_prio_44", 0, {PRIORITY_BY_NUM_PRIO, 0, 120, 12}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_descending_action_info_array[CTEST_DNX_FIELD_EFES_DESCENDING_NOF_ACTIONS] = {
    {"descending_act_1", 0, {PRIORITY_BY_NUM_PRIO, 0, 100, 0}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"descending_act_2", 0, {PRIORITY_BY_NUM_PRIO, 0, 50, 1}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"descending_act_3", 0, {PRIORITY_BY_NUM_PRIO, 0, 5, 2}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"descending_act_4", 0, {PRIORITY_BY_NUM_PRIO, 0, 4, 2}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x0, {0}}},
    {"descending_act_5", 0, {PRIORITY_BY_NUM_PRIO, 0, 1, 2}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_evacuated_only_place_fg1_action_info_array[CTEST_DNX_FIELD_EFES_EVACUATED_ONLY_PLACE_NOF_ACTIONS] = {
    {"evac_pos_0", 0, {PRIORITY_BY_NUM_PRIO, 0, 0, 0}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_evacuated_only_place_fg2_action_info_array[CTEST_DNX_FIELD_EFES_EVACUATED_ONLY_PLACE_NOF_ACTIONS] = {
    {"evac_pos_1", 0, {PRIORITY_BY_NUM_PRIO, 0, 2, 2}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_evacuated_only_place_fg4_action_info_array[CTEST_DNX_FIELD_EFES_EVACUATED_ONLY_PLACE_NOF_ACTIONS] = {
    {"evac_pos_2", 0, {PRIORITY_BY_NUM_PRIO, 0, 1, 1}, {0, bcmFieldActionDropPrecedence, 1, 1, 0x1, {0}}},
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_evacuated_only_place_fg3_action_info_array[CTEST_DNX_FIELD_EFES_EVACUATED_ONLY_PLACE_FG3_NOF_ACTIONS] = {
    {"dont_care_0", 0, {PRIORITY_DONT_CARE, 0, 0, 16}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"dont_care_1", 0, {PRIORITY_DONT_CARE, 0, 0, 3}, {0, bcmFieldActionPphSnoopCode, 2, 1, 0x1, {0}}},
    {"dont_care_2", 0, {PRIORITY_DONT_CARE, 0, 0, 4}, {0, bcmFieldActionPphPresentSet, 1, 1, 0x1, {0}}},
    {"dont_care_3", 0, {PRIORITY_DONT_CARE, 0, 0, 5}, {0, bcmFieldActionInVportClass0, 7, 1, 0x1, {0}}},
    {"dont_care_4", 0, {PRIORITY_DONT_CARE, 0, 0, 6}, {0, bcmFieldActionUDHBase0, 1, 1, 0x1, {0}}},
    {"dont_care_5", 0, {PRIORITY_DONT_CARE, 0, 0, 7}, {0, bcmFieldActionUDHBase1, 1, 1, 0x1, {0}}},
    {"dont_care_6", 0, {PRIORITY_DONT_CARE, 0, 0, 8}, {0, bcmFieldActionUDHBase2, 1, 1, 0x1, {0}}},
    {"dont_care_7", 0, {PRIORITY_DONT_CARE, 0, 0, 9}, {0, bcmFieldActionUDHBase3, 1, 1, 0x1, {0}}},
    {"dont_care_8", 0, {PRIORITY_DONT_CARE, 0, 0, 10}, {0, bcmFieldActionStatProfile0, 4, 1, 0x1, {0}}},
    {"dont_care_9", 0, {PRIORITY_DONT_CARE, 0, 0, 11}, {0, bcmFieldActionStatProfile1, 4, 1, 0x1, {0}}},
    {"dont_care_10", 0, {PRIORITY_DONT_CARE, 0, 0, 12}, {0, bcmFieldActionStatProfile2, 4, 1, 0x1, {0}}},
    {"dont_care_11", 0, {PRIORITY_DONT_CARE, 0, 0, 13}, {0, bcmFieldActionIPTProfile, 2, 1, 0x1, {0}}},
    {"dont_care_12", 0, {PRIORITY_DONT_CARE, 0, 0, 14}, {0, bcmFieldActionFabricHeaderSet, 3, 1, 0x1, {0}}},
    {"dont_care_13", 0, {PRIORITY_DONT_CARE, 0, 0, 15}, {0, bcmFieldActionOuterVlanPrioNew, 3, 1, 0x1, {0}}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_neg_diff_prio_types_predef_action_info_array[CTEST_DNX_FIELD_EFES_NEGATIVE_DIFF_PRIO_TYPES_NOF_ACTIONS] = {
    {NULL, bcmFieldActionPrioIntNew, {PRIORITY_BY_NUM_PRIO, 0, 3, 0}, {0}},
    {NULL, bcmFieldActionPrioIntNew, {PRIORITY_BY_POSITION, 0, 5, 1}, {0}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_neg_diff_prio_types_predef_udf_action_info_array[CTEST_DNX_FIELD_EFES_NEGATIVE_DIFF_PRIO_TYPES_NOF_ACTIONS] = {
    {NULL, bcmFieldActionPrioIntNew, {PRIORITY_BY_NUM_PRIO, 0, 3, 0}, {0}},
    {"diff_pri_types", 0, {PRIORITY_BY_POSITION, 0, 5, 1}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}}
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_neg_same_type_prio_action_info_array[CTEST_DNX_FIELD_EFES_NEGATIVE_SAME_TYPE_PRIO_NOF_ACTIONS] = {
    {NULL, bcmFieldActionPrioIntNew, {PRIORITY_BY_NUM_PRIO, 0, 5, 0}, {0}},
    {NULL, bcmFieldActionPrioIntNew, {PRIORITY_BY_NUM_PRIO, 0, 15, 1}, {0}},
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_neg_same_pos_action_info_array[CTEST_DNX_FIELD_EFES_NEGATIVE_SAME_POS_NOF_ACTIONS] = {
    {NULL, bcmFieldActionPrioIntNew, {PRIORITY_BY_POSITION, 0, 5, 0}, {0}},
    {NULL, bcmFieldActionDropPrecedence, {PRIORITY_BY_POSITION, 0, 5, 1}, {0}},
};

static ctest_dnx_field_efes_action_info_t Ctest_dnx_field_efes_neg_same_act_diff_prio_action_info_array[CTEST_DNX_FIELD_EFES_NEGATIVE_SAME_ACT_DIFF_PRIO_NOF_ACTIONS] = {
    {"same_act_diff_pri_1", 0, {PRIORITY_BY_NUM_PRIO, 0, 10, 6}, {0, bcmFieldActionPrioIntNew, 2, 1, 0x1, {0}}},
    {"same_act_diff_pri_2", 0, {PRIORITY_BY_NUM_PRIO, 0, 10, 6}, {0, bcmFieldActionPrioIntNew, 1, 2, 0x1, {0}}}
};

/**
 * Following arrays are global, used for all test cases. They contain information per field group. Like:
 * fg_id, fg_name, nof_actions and actions.
 */
static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_position_fg_info_array[1] = {
/**    Field group ID    |   Field group Name   |            Number of tested actions            |                Actions info        */
    {BCM_FIELD_ID_INVALID, "efes_position", CTEST_DNX_FIELD_EFES_POS_PRIO_NOF_ACTIONS, Ctest_dnx_field_efes_position_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_priority_fg_info_array[1] = {
/**    Field group ID    |   Field group Name   |            Number of tested actions            |                Actions info        */
    {BCM_FIELD_ID_INVALID, "efes_priority", CTEST_DNX_FIELD_EFES_POS_PRIO_NOF_ACTIONS, Ctest_dnx_field_efes_priority_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_sharing_fg_info_array[1] = {
/**    Field group ID    |   Field group Name   |            Number of tested actions            |                Actions info        */
    {BCM_FIELD_ID_INVALID,"efes_sharing", CTEST_DNX_FIELD_EFES_SHARING_NOF_ACTIONS, Ctest_dnx_field_efes_sharing_first_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_dont_care_pos_prio_fg_info_array[3] = {
/**    Field group ID    |   Field group Name   |            Number of tested actions            |                Actions info        */
    {BCM_FIELD_ID_INVALID, "efes_dont_care", CTEST_DNX_FIELD_EFES_DONT_CARE_NOF_ACTIONS, Ctest_dnx_field_efes_dont_care_first_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_d_care_pos", CTEST_DNX_FIELD_EFES_DONT_CARE_POS_PRIO_NOF_ACTIONS, Ctest_dnx_field_efes_dont_care_position_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_d_care_prio", CTEST_DNX_FIELD_EFES_DONT_CARE_POS_PRIO_NOF_ACTIONS, Ctest_dnx_field_efes_dont_care_priority_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_combination_fg_info_array[2] = {
/**    Field group ID    |   Field group Name   |            Number of tested actions            |                Actions info        */
    {BCM_FIELD_ID_INVALID, "efes_comb_1", CTEST_DNX_FIELD_EFES_COMB_FIRST_NOF_ACTIONS, Ctest_dnx_field_efes_combination_first_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_comb_2", CTEST_DNX_FIELD_EFES_COMB_SECOND_NOF_ACTIONS, Ctest_dnx_field_efes_combination_second_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_exhaustive_fg_info_array[2] = {
/**    Field group ID    |   Field group Name   |            Number of tested actions            |                Actions info        */
    {BCM_FIELD_ID_INVALID, "efes_exhaustive_1", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_prio_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaustive_2", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pos_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_exhaustive_pgm_fg_info_array[CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_FGS] = {
/**    Field group ID    |   Field group Name   |            Number of tested actions            |                Actions info        */
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_1", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_2", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_3", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_4", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_5", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_6", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_7", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_8", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_9", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_10", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_11", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_12", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_13", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_14", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_15", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_16", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_17", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_18", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_19", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_20", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_21", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_22", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_23", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_24", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_25", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_26", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_27", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_28", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_29", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_30", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_31", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_pgm_32", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_exhaustive_masks_fg_info_array[CTEST_DNX_FIELD_EFES_EXHAUSTIVE_MASKS_NOF_FGS] = {
/** Field group ID    |   Field group Name     |    Number of tested actions   |    Actions info        */
    {BCM_FIELD_ID_INVALID, "efes_exhaust_mask_1", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_prio_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_mask_2", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_masks_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_exhaust_mask_3", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_MASKS_FG3_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_masks_fg3_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_evacuated_only_place_fg_info_array[CTEST_DNX_FIELD_EFES_EVACUATED_ONLY_PLACE_NOF_FGS] = {
/** Field group ID    |   Field group Name     |    Number of tested actions   |    Actions info        */
    {BCM_FIELD_ID_INVALID, "efes_evac_place_1", CTEST_DNX_FIELD_EFES_EVACUATED_ONLY_PLACE_NOF_ACTIONS, Ctest_dnx_field_efes_evacuated_only_place_fg1_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_evac_place_2", CTEST_DNX_FIELD_EFES_EVACUATED_ONLY_PLACE_NOF_ACTIONS, Ctest_dnx_field_efes_evacuated_only_place_fg2_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_evac_place_3", CTEST_DNX_FIELD_EFES_EVACUATED_ONLY_PLACE_FG3_NOF_ACTIONS, Ctest_dnx_field_efes_evacuated_only_place_fg3_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_evac_place_4", CTEST_DNX_FIELD_EFES_EVACUATED_ONLY_PLACE_NOF_ACTIONS, Ctest_dnx_field_efes_evacuated_only_place_fg4_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_descending_fg_info_array[1] = {
/** Field group ID    |   Field group Name     |    Number of tested actions   |    Actions info        */
    {BCM_FIELD_ID_INVALID, "efes_neg_descend", CTEST_DNX_FIELD_EFES_DESCENDING_NOF_ACTIONS, Ctest_dnx_field_efes_descending_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_negative_all_efes_prio_fg_info_array[2] = {
/** Field group ID    |   Field group Name     |    Number of tested actions   |    Actions info        */
    {BCM_FIELD_ID_INVALID, "efes_neg_all_1", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_prio_action_info_array},
    {BCM_FIELD_ID_INVALID, "efes_neg_all_2", CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS, Ctest_dnx_field_efes_exhaustive_pgm_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_negative_diff_prio_types_predef_fg_info_array[1] = {
/** Field group ID    |   Field group Name     |    Number of tested actions   |    Actions info        */
    {BCM_FIELD_ID_INVALID, "efes_neg_diff_pri", CTEST_DNX_FIELD_EFES_NEGATIVE_DIFF_PRIO_TYPES_NOF_ACTIONS, Ctest_dnx_field_efes_neg_diff_prio_types_predef_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_negative_diff_prio_types_predef_udf_fg_info_array[1] = {
/** Field group ID    |   Field group Name     |    Number of tested actions   |    Actions info        */
    {BCM_FIELD_ID_INVALID, "efes_neg_diff_pri", CTEST_DNX_FIELD_EFES_NEGATIVE_DIFF_PRIO_TYPES_NOF_ACTIONS, Ctest_dnx_field_efes_neg_diff_prio_types_predef_udf_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_negative_same_type_num_prio_fg_info_array[1] = {
/** Field group ID    |   Field group Name     |    Number of tested actions   |    Actions info        */
    {BCM_FIELD_ID_INVALID, "efes_neg_same_type_pri", CTEST_DNX_FIELD_EFES_NEGATIVE_SAME_TYPE_PRIO_NOF_ACTIONS, Ctest_dnx_field_efes_neg_same_type_prio_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_negative_same_pos_fg_info_array[1] = {
/** Field group ID    |   Field group Name     |    Number of tested actions   |    Actions info        */
    {BCM_FIELD_ID_INVALID, "efes_neg_same_pos", CTEST_DNX_FIELD_EFES_NEGATIVE_SAME_POS_NOF_ACTIONS, Ctest_dnx_field_efes_neg_same_pos_action_info_array}
};

static ctest_dnx_field_efes_fg_info_t Ctest_field_efes_negative_same_act_diff_prio_fg_info_array[1] = {
    {BCM_FIELD_ID_INVALID, "efes_neg_same_prio", CTEST_DNX_FIELD_EFES_NEGATIVE_SAME_ACT_DIFF_PRIO_NOF_ACTIONS, Ctest_dnx_field_efes_neg_same_act_diff_prio_action_info_array}
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
ctest_dnx_field_efes_global_arrays_reset(
    void)
{
    int fg_iter;

    /**
     * Reset the FG ID, in the Global arrays, after finishing all of the scenarios.
     * This should be done in case we want to run the test more then ones.
     * Manually reset the Global arrays, because they are reset only during the init.
     */
    Ctest_field_efes_position_fg_info_array[0].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_priority_fg_info_array[0].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_sharing_fg_info_array[0].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_dont_care_pos_prio_fg_info_array[0].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_dont_care_pos_prio_fg_info_array[1].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_dont_care_pos_prio_fg_info_array[2].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_combination_fg_info_array[0].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_combination_fg_info_array[1].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_exhaustive_fg_info_array[0].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_exhaustive_fg_info_array[1].fg_id = BCM_FIELD_ID_INVALID;

    for (fg_iter = 0; fg_iter < CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_FGS; fg_iter++)
    {
        Ctest_field_efes_exhaustive_pgm_fg_info_array[fg_iter].fg_id = BCM_FIELD_ID_INVALID;
    }

    for (fg_iter = 0; fg_iter < CTEST_DNX_FIELD_EFES_EXHAUSTIVE_MASKS_NOF_FGS; fg_iter++)
    {
        Ctest_field_efes_exhaustive_masks_fg_info_array[fg_iter].fg_id = BCM_FIELD_ID_INVALID;
    }

    for (fg_iter = 0; fg_iter < CTEST_DNX_FIELD_EFES_EVACUATED_ONLY_PLACE_NOF_FGS; fg_iter++)
    {
        Ctest_field_efes_evacuated_only_place_fg_info_array[fg_iter].fg_id = BCM_FIELD_ID_INVALID;
    }

    Ctest_field_efes_descending_fg_info_array[0].fg_id = BCM_FIELD_ID_INVALID;

    Ctest_field_efes_negative_all_efes_prio_fg_info_array[0].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_negative_all_efes_prio_fg_info_array[1].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_negative_diff_prio_types_predef_fg_info_array[0].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_negative_diff_prio_types_predef_udf_fg_info_array[0].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_negative_same_type_num_prio_fg_info_array[0].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_negative_same_pos_fg_info_array[0].fg_id = BCM_FIELD_ID_INVALID;
    Ctest_field_efes_negative_same_act_diff_prio_fg_info_array[0].fg_id = BCM_FIELD_ID_INVALID;
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
ctest_dnx_field_efes_cs_config(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * cs_info)
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
 *  This function is used to configure an user defined action.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which the action
 *  will be created (iPMF1/2/3, ePMF.....)
 * \param [in] efes_action_info - Array which contains information per action.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_user_action_config(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_action_info_t * efes_action_info)
{
    bcm_field_action_info_t action_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_action_info_t_init(&action_info);

    action_info.action_type = efes_action_info->action_info.action_type;
    sal_strncpy_s((char *) action_info.name, efes_action_info->name, sizeof(action_info.name));
    action_info.size = efes_action_info->action_info.size;
    action_info.stage = field_stage;
    action_info.prefix_size = efes_action_info->action_info.prefix_size;
    action_info.prefix_value = efes_action_info->action_info.prefix_value;

    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &(efes_action_info->action_type)));

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
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like name, actions.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_fg_config(
    int unit,
    bcm_field_stage_t field_stage,
    bcm_field_context_t context_id,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{
    int action_index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_attach_info_t_init(&attach_info);

    /**
     * In case the FG_ID is invalid, which is the default value in the global arrays,
     * we should perform group_create(). Otherwise we it means that we want to attach,
     * already created FG, to the given context.
     */
    if (efes_fg_info->fg_id == BCM_FIELD_ID_INVALID)
    {
        bcm_field_group_info_t_init(&fg_info);

        fg_info.fg_type = bcmFieldGroupTypeTcam;
        fg_info.stage = field_stage;

        sal_strncpy_s((char *) fg_info.name, efes_fg_info->fg_name, sizeof(fg_info.name));
        fg_info.nof_quals = CTEST_DNX_FIELD_EFES_NOF_QUALS;
        fg_info.nof_actions = efes_fg_info->nof_actions;

        /**
         * For test cases, where we are going to have more then one field group,
         * we doun't need create same qualifier again and again, just
         * reuse the created one.
         */
        if (Ctest_dnx_field_efes_user_qual_id == bcmFieldQualifyCount)
        {
            /** Create a qualifier to be used for all stages. */
            bcm_field_qualifier_info_create_t_init(&qual_info);
            qual_info.size = 1;
            sal_strncpy_s((char *) qual_info.name, "efes_const_qual", sizeof(qual_info.name));
            SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &Ctest_dnx_field_efes_user_qual_id));
        }

        fg_info.qual_types[0] = Ctest_dnx_field_efes_user_qual_id;

        for (action_index = 0; action_index < efes_fg_info->nof_actions; action_index++)
        {
            /**
             * Check if the current action name is not NULL, it means that
             * this is an user define action and should be created as follow.
             */
            if (efes_fg_info->efes_action_info[action_index].name != NULL)
            {
                SHR_IF_ERR_EXIT(ctest_dnx_field_efes_user_action_config(unit, fg_info.stage,
                                                                        &(efes_fg_info->efes_action_info
                                                                          [action_index])));
            }
            fg_info.action_types[action_index] = efes_fg_info->efes_action_info[action_index].action_type;
            fg_info.action_with_valid_bit[action_index] = TRUE;
        }

        /** Create the field group. */
        SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &(efes_fg_info->fg_id)));

        LOG_INFO_EX(BSL_LOG_MODULE, "Field Group ID %d was created. \n %s%s%s", efes_fg_info->fg_id, EMPTY, EMPTY,
                    EMPTY);
    }

    attach_info.key_info.nof_quals = CTEST_DNX_FIELD_EFES_NOF_QUALS;
    attach_info.payload_info.nof_actions = efes_fg_info->nof_actions;
    attach_info.key_info.qual_types[0] = Ctest_dnx_field_efes_user_qual_id;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = 0;

    for (action_index = 0; action_index < efes_fg_info->nof_actions; action_index++)
    {
        attach_info.payload_info.action_types[action_index] = efes_fg_info->efes_action_info[action_index].action_type;

        /**
         * Do a switch-case for different types of priorities, which we have in the different scenarios.
         * Call relevant macro per priority type.
         */
        switch (efes_fg_info->efes_action_info[action_index].action_prio_info.priority_type)
        {
            case PRIORITY_BY_POSITION:
            {
                attach_info.payload_info.action_info[action_index].priority =
                    BCM_FIELD_ACTION_POSITION(efes_fg_info->efes_action_info[action_index].action_prio_info.array_id,
                                              efes_fg_info->efes_action_info[action_index].
                                              action_prio_info.position_priority);
                break;
            }
            case PRIORITY_BY_NUM_PRIO:
            {
                attach_info.payload_info.action_info[action_index].priority =
                    BCM_FIELD_ACTION_PRIORITY(efes_fg_info->efes_action_info[action_index].action_prio_info.array_id,
                                              efes_fg_info->efes_action_info[action_index].
                                              action_prio_info.position_priority);
                break;
            }
            case PRIORITY_DONT_CARE:
            {
                attach_info.payload_info.action_info[action_index].priority = BCM_FIELD_ACTION_DONT_CARE;
                break;
            }
            default:
            {
                attach_info.payload_info.action_info[action_index].priority = BCM_FIELD_ACTION_DONT_CARE;
                break;
            }
        }
    }

    /** Attach the created field group to the context. */
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, efes_fg_info->fg_id, context_id, &attach_info));

    LOG_INFO_EX(BSL_LOG_MODULE, "Field Group ID %d was attached to Context ID %d. \n %s%s", efes_fg_info->fg_id,
                context_id, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function compares returned data (FG_ID and EFES_ID) from the DNX layer, with
 *  the expected one, which is given in efes_fg_info.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, of which the context EFES
 *  info will be retrieve (iPMF1/2/3, ePMF.....)
 * \param [in] context_id - Context id for which the EFES info will
 *  be retrieve.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like name, actions and going to be compared with
 *  the retrieved context EFES info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_compare(
    int unit,
    bcm_field_stage_t field_stage,
    bcm_field_context_t context_id,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{
    int action_index;
    dnx_field_stage_e dnx_field_stage;
    dnx_field_actions_fes_context_get_info_t context_fes_get_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Convert the BCM field stage to DNX one. */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, field_stage, &dnx_field_stage));

    /** Gets the information about the FES quartets allocated to the context ID. */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, dnx_field_stage, context_id, &context_fes_get_info));

    for (action_index = 0; action_index < efes_fg_info->nof_actions; action_index++)
    {
        /**
         * Check if the retrieved FG ID matches on the given one. In case 'no' then
         * current action is not valid for the given FG and we should continue to the
         * next action.
         */
        if (context_fes_get_info.fes_quartets[action_index].fg_id != efes_fg_info->fg_id)
        {
            continue;
        }

        /** Check if the retrieved EFES ID is same as the expected EFES ID. */
        if (context_fes_get_info.fes_quartets[action_index].fes_id !=
            efes_fg_info->efes_action_info[action_index].action_prio_info.efes_id_expected)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "EFES ID compare operation failed for CTX %d (FG %d). Expected EFES ID %d | Actual EFES ID %d! \n",
                        context_id, efes_fg_info->fg_id,
                        efes_fg_info->efes_action_info[action_index].action_prio_info.efes_id_expected,
                        context_fes_get_info.fes_quartets[action_index].fes_id);
            SHR_EXIT();
        }
    }

    LOG_INFO_EX(BSL_LOG_MODULE, "EFES ID compare operation for CTX %d (FG %d) was successful! \n %s%s",
                context_id, efes_fg_info->fg_id, EMPTY, EMPTY);

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
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_cs_destroy(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info)
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
    p_id.presel_id = efes_cs_info->presel_id;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data));

    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, field_stage_internal, efes_cs_info->context_id));
    efes_cs_info->context_id = BCM_FIELD_CONTEXT_ID_INVALID;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function destroys given FG and Context, and all of the
 *  configured actions and qualifiers.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, for which
 *  the EFES configuration have to be destroyed (iPMF1/2/3, ePMF.....).
 * \param [in] more_than_one_ctx_per_fg - Flag to indicate if we have
 *  more then one context per given field group. It can be FALSE or TRUE.
 *  In case of TRUE, we will not delete the attached field group,
 *  otherwise it will be destroyed.
 * \param [in] more_than_one_fg_per_ctx - Flag to indicate if we have
 *  more then one field group per given context. It can be FALSE or TRUE.
 *  In case of TRUE, we will not delete the context,
 *  otherwise it will be destroyed.
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 * \remark
 *  Note that both flags can be TRUE! In such case the functions will
 *  destroy only FG and its User Actions. Mainly used for negative cases.
 */
static shr_error_e
ctest_dnx_field_efes_destroy(
    int unit,
    bcm_field_stage_t field_stage,
    int more_than_one_ctx_per_fg,
    int more_than_one_fg_per_ctx,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{
    int action_index;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * In case we are negative test and the test fails on context_attach,
     * we should destroy only FG and its User Actions.
     */
    if (!more_than_one_ctx_per_fg || !more_than_one_fg_per_ctx)
    {
        /** In case context_id or fg_id is invalid then, we should not perform detach. */
        if (efes_cs_info->context_id != BCM_FIELD_CONTEXT_ID_INVALID && efes_fg_info->fg_id != BCM_FIELD_ID_INVALID)
        {
            SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, efes_fg_info->fg_id, efes_cs_info->context_id));
        }

        /** In case we have more then one FG per context, then we should destroy first those FGs. */
        if (!more_than_one_fg_per_ctx)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_destroy(unit, field_stage, efes_cs_info));
        }

        /** In case we have more then one CS per field group, then we should skip the FG destroy. */
        if (!more_than_one_ctx_per_fg)
        {
            /** In case fg_id is invalid then, we should not delete it. */
            if (efes_fg_info->fg_id != BCM_FIELD_ID_INVALID)
            {
                SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, efes_fg_info->fg_id));
            }

            /**
             * If there are more then one FG per context, it means that the UDF qualifier is,
             * being reused by them. Destroy first the FGs and on the last one destroy the qualifier.
             */
            if (!more_than_one_fg_per_ctx)
            {
                if (Ctest_dnx_field_efes_user_qual_id != bcmFieldQualifyCount)
                {
                    SHR_IF_ERR_EXIT(bcm_field_qualifier_destroy(unit, Ctest_dnx_field_efes_user_qual_id));
                    /** Update the Global variable for user qualifier to be used for the next FG. */
                    Ctest_dnx_field_efes_user_qual_id = bcmFieldQualifyCount;
                }
            }

            for (action_index = 0; action_index < efes_fg_info->nof_actions; action_index++)
            {
                if (efes_fg_info->efes_action_info[action_index].name != NULL)
                {
                    SHR_IF_ERR_EXIT(bcm_field_action_destroy
                                    (unit, efes_fg_info->efes_action_info[action_index].action_type));
                }
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, efes_fg_info->fg_id));

        for (action_index = 0; action_index < efes_fg_info->nof_actions; action_index++)
        {
            if (efes_fg_info->efes_action_info[action_index].name != NULL)
            {
                SHR_IF_ERR_EXIT(bcm_field_action_destroy
                                (unit, efes_fg_info->efes_action_info[action_index].action_type));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents positive case, which is testing the
 *  action position macro.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_position(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[0]));

    /** Create a field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Compare the results for the given context with the expected one. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Destroy group and context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, FALSE, &efes_cs_info[0], &efes_fg_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents positive case, which is testing the
 *  action priority macro.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_priority(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[0]));

    /** Create a field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Compare the results for the given context with the expected one. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Destroy group and context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, FALSE, &efes_cs_info[0], &efes_fg_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents positive case, which is testing the
 *  sharing between EFESs.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_sharing(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[0]));

    /** Create a field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Compare the results for the given context with the expected one. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Create another context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[1]));

    /** Use different action info for the second context, to perform the sharing. */
    efes_fg_info[0].efes_action_info = Ctest_dnx_field_efes_sharing_second_action_info_array;

    /** Create a field group and attach it to the second created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[1].context_id, &efes_fg_info[0]));

    /** Compare the results for the given context with the expected one. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[1].context_id, &efes_fg_info[0]));

    /** Destroy group and context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, TRUE, FALSE, &efes_cs_info[0], &efes_fg_info[0]));

    /** Destroy group and context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, FALSE, &efes_cs_info[1], &efes_fg_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents positive case, where we are creating a
 *  context and 2 FGs, to which we add 16 actions with don't care priority,
 *  then trying to add another action (FG 2) with numerical or position priority.
 *  Then compares the results of the test case.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_dont_care_pos_prio(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[0]));

    /** Create a field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Compare the results for the given context with the expected one. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Create a field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[1]));

    /** Compare the results for the given context with the expected one. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[1]));

    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, TRUE, efes_cs_info, &efes_fg_info[1]));

    /** Create a field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[2]));

    /** Compare the results for the given context with the expected one. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[2]));

    /** Destroy group and context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, TRUE, &efes_cs_info[0], &efes_fg_info[2]));

    /** Destroy group and context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, FALSE, &efes_cs_info[0], &efes_fg_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents positive case, where we are creating a
 *  context and 2 FGs, to which we add different number of actions with
 *  different priority types (position, num_priority, dont_care) to test
 *  combination of the different priority types together.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_combination(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[0]));

    /** Create a field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Create a field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[1]));

    /** Compare the results for the given context with the expected one. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Compare the results for the given context with the expected one. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[1]));

    /** Destroy group and context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, TRUE, &efes_cs_info[0], &efes_fg_info[1]));

    /** Destroy group and context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, FALSE, &efes_cs_info[0], &efes_fg_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents positive case, where we are creating a
 *  context and 2 FGs, to which we add different number of actions,
 *  depends on stage, and trying to allocate all of the EFESs per stage.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_exhaustive(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[0]));

    /** Create a field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Create a field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[1]));

    /** Compare the results for the given context with the expected one. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Compare the results for the given context with the expected one. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[1]));

    /** Destroy group and context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, TRUE, &efes_cs_info[0], &efes_fg_info[1]));

    /** Destroy group and context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, FALSE, &efes_cs_info[0], &efes_fg_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents positive case, where we are creating a
 *  several contexts and FGs, to which we add different number of actions,
 *  depends on stage, and trying to used all of the EFES PGMs per stage.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_exhaustive_pgm(
    int unit,
    bcm_field_stage_t field_stage,
    int nof_fgs,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{
    int ctx_index;

    SHR_FUNC_INIT_VARS(unit);

    for (ctx_index = 0; ctx_index < nof_fgs; ctx_index++)
    {
        /** Create a context selection. */
        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[ctx_index]));

        /** Create a field group and attach it to the created context selection. */
        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config
                        (unit, field_stage, efes_cs_info[ctx_index].context_id, &efes_fg_info[ctx_index]));
    }

    for (ctx_index = 0; ctx_index < nof_fgs; ctx_index++)
    {
        if (ctx_index < nof_fgs - 1)
        {
            /** Destroy context configurations. */
            SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy
                            (unit, field_stage, TRUE, FALSE, &efes_cs_info[ctx_index], &efes_fg_info[ctx_index]));
            /** Destroy group configurations. */
            SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy
                            (unit, field_stage, FALSE, TRUE, &efes_cs_info[ctx_index], &efes_fg_info[ctx_index]));
        }
        else
        {
            /** Destroy group and context configurations. */
            SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy
                            (unit, field_stage, FALSE, FALSE, &efes_cs_info[ctx_index], &efes_fg_info[ctx_index]));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents positive case, where we are creating a
 *  several contexts and FGs, to which we add different number of actions,
 *  depends on stage, and trying to used all of the EFES MASKs per stage.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_exhaustive_masks(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{
    int fg_index;

    SHR_FUNC_INIT_VARS(unit);

    for (fg_index = 0; fg_index < CTEST_DNX_FIELD_EFES_EXHAUSTIVE_MASKS_NOF_FGS; fg_index++)
    {
        /** Create a context selection. */
        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[fg_index]));

        /** Create a field group and attach it to the created context selection. */
        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config
                        (unit, field_stage, efes_cs_info[fg_index].context_id, &efes_fg_info[fg_index]));
    }

    for (fg_index = 0; fg_index < CTEST_DNX_FIELD_EFES_EXHAUSTIVE_MASKS_NOF_FGS; fg_index++)
    {
        if (fg_index < CTEST_DNX_FIELD_EFES_EXHAUSTIVE_MASKS_NOF_FGS - 1)
        {
            /** Destroy context configurations. */
            SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy
                            (unit, field_stage, TRUE, FALSE, &efes_cs_info[fg_index], &efes_fg_info[fg_index]));
            /** Destroy group configurations. */
            SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy
                            (unit, field_stage, FALSE, TRUE, &efes_cs_info[fg_index], &efes_fg_info[fg_index]));
        }
        else
        {
            /** Destroy group and context configurations. */
            SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy
                            (unit, field_stage, FALSE, FALSE, &efes_cs_info[fg_index], &efes_fg_info[fg_index]));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents a positive case,
 *  Set 5 EFESs of same action type, with descending priorities.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_descending_actions(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[0]));

    /** Create a field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Destroy group and context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, FALSE, &efes_cs_info[0], &efes_fg_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents a positive case,
 *  Set 4 FGs:
 *      - FG one - 1 FES with priority 0
 *      - FG two - 1 FES with priority 2
 *      - FG three - 14 FESs with priority don't case
 *      - FG four - 1 FES with priority 1
 *  Expecting the FG four to push the FES of FG two.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_evacuated_only_place_actions(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[0]));

    /** Create the first field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Create the second field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[1]));

    /** Create the third field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[2]));

    /** Create the forth field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[3]));

    /** Compare the results for the given context with the expected one for the first FG. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Compare the results for the given context with the expected one for the second FG. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[1]));

    /** Compare the results for the given context with the expected one for the third FG. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[2]));

    /** Compare the results for the given context with the expected one for the forth FG. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_compare(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[3]));

    /** Destroy group one configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, TRUE, &efes_cs_info[0], &efes_fg_info[0]));

    /** Destroy group two configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, TRUE, &efes_cs_info[0], &efes_fg_info[1]));

    /** Destroy group three configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, TRUE, &efes_cs_info[0], &efes_fg_info[2]));

    /** Destroy group four and context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, FALSE, &efes_cs_info[0], &efes_fg_info[3]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents negative case,
 *  Allocate all EFESs per stage, using numerical priority,
 *  in the first array and then try to add an action with numerical priority.
 *
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_negative_all_efes_prio(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = BCM_E_NONE;

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[0]));

    /** Create a field group and attach it to the created context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]));

    /** Create a field group and attach it to the created context selection. */
    rv = ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[1]);
    if (rv == BCM_E_RESOURCE)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "NEGATIVE TEST CASE 1 was successful! \r\n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "NEGATIVE TEST CASE 1 failed!\r\n");
    }

    /** Destroy group configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, TRUE, TRUE, &efes_cs_info[0], &efes_fg_info[1]));
    /** Destroy group and context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, FALSE, FALSE, &efes_cs_info[0], &efes_fg_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents negative case,
 *  Set 2 EFESs of same action type, but with different
 *  type of priorities (numerical priority and position).
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_negative_diff_prio_types_predef(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = BCM_E_NONE;

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[0]));

    /** Create a field group and attach it to the created context selection. */
    rv = ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]);
    if (rv == BCM_E_PARAM)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "NEGATIVE TEST CASE 2 was successful! \r\n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "NEGATIVE TEST CASE 2 failed!\r\n");
    }

    /** Destroy context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_destroy(unit, field_stage, &efes_cs_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents negative case,
 *  Set 2 EFESs of same action type (user define and predefine), but with different
 *  type of priorities (numerical priority and position).
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_negative_diff_prio_types_predef_udf(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = BCM_E_NONE;

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[0]));

    /** Create a field group and attach it to the created context selection. */
    rv = ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]);
    if (rv == BCM_E_PARAM)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "NEGATIVE TEST CASE 3 was successful! \r\n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "NEGATIVE TEST CASE 3 failed!\r\n");
    }

    /** Destroy context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_destroy(unit, field_stage, &efes_cs_info[0]));

    /** Destroy group configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, TRUE, TRUE, &efes_cs_info[0], &efes_fg_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents negative case,
 *  Set 2 EFESs of same action type, with numerical priorities.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_negative_same_type_prio(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = BCM_E_NONE;

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[0]));

    /** Create a field group and attach it to the created context selection. */
    rv = ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]);
    if (rv == BCM_E_PARAM)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "NEGATIVE TEST CASE 4 was successful! \r\n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "NEGATIVE TEST CASE 4 failed!\r\n");
    }

    /** Destroy context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_destroy(unit, field_stage, &efes_cs_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents negative case,
 *  Set 2 EFESs of same action type, with same positions.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_negative_same_pos(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = BCM_E_NONE;

    /** Create a context selection. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[0]));

    /** Create a field group and attach it to the created context selection. */
    rv = ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]);
    if (rv == BCM_E_PARAM)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "NEGATIVE TEST CASE 5 was successful! \r\n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "NEGATIVE TEST CASE 5 failed!\r\n");
    }

    /** Destroy context configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_destroy(unit, field_stage, &efes_cs_info[0]));

    /** Destroy group configurations. */
    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, TRUE, TRUE, &efes_cs_info[0], &efes_fg_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function presents negative case,
 *  Set 2 EFESs of different action type, with same numerical priorities.
 *
 * \param [in] unit - Device ID
 * \param [in] field_stage - Field stage, on which
 *  the EFES configuration will be done (iPMF1/2/3, ePMF.....).
 * \param [in] efes_cs_info - Array which contains information per CS,
 *  like cs_id, name and presel_id.
 * \param [in] efes_fg_info - Array which contains information per FG,
 *  like fg_id, name and action_info.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */

static shr_error_e
ctest_dnx_field_efes_negative_same_act_diff_prio(
    int unit,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_efes_cs_info_t * efes_cs_info,
    ctest_dnx_field_efes_fg_info_t * efes_fg_info)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = BCM_E_NONE;

    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_config(unit, field_stage, &efes_cs_info[0]));

    rv = ctest_dnx_field_efes_fg_config(unit, field_stage, efes_cs_info[0].context_id, &efes_fg_info[0]);
    if (rv == BCM_E_PARAM)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "NEGATIVE TEST CASE 6 was successful! \r\n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "NEGATIVE TEST CASE 6 failed!\r\n");
    }

    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_cs_destroy(unit, field_stage, &efes_cs_info[0]));

    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_destroy(unit, field_stage, TRUE, TRUE, &efes_cs_info[0], &efes_fg_info[0]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This functions tests the EFES usage, positive and negative cases.
 *
 * \param [in] unit - Device ID
 * \param [in] args - Extra arguments
 * \param [in] sand_control - Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_efes_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_field_stage_t field_stage, stage_index;
    int exhaustive_pgm_nof_fgs;
    bsl_severity_t original_severity_fld_proc = BSL_INFO;
    bsl_severity_t original_severity_dbal_dnx = BSL_INFO;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Save the original severity level of the fld proc and dbal.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbal_dnx);

    /** Do the testing for all field stages IPMF1/2/3/EPMF. */
    for (stage_index = 0; stage_index < CTEST_DNX_FIELD_EFES_NOF_STAGES; stage_index++)
    {
        switch (stage_index)
        {
            case 0:
            {
                field_stage = bcmFieldStageIngressPMF1;
                exhaustive_pgm_nof_fgs = CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_FGS;
                break;
            }
            case 1:
            {
                field_stage = bcmFieldStageIngressPMF2;
                exhaustive_pgm_nof_fgs = CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_FGS;
                break;
            }
            case 2:
            {
                field_stage = bcmFieldStageIngressPMF3;
                Ctest_field_efes_position_fg_info_array[0].nof_actions =
                    CTEST_DNX_FIELD_EFES_POS_PRIO_IPMF3_EPMF_NOF_ACTIONS;
                Ctest_field_efes_priority_fg_info_array[0].nof_actions =
                    CTEST_DNX_FIELD_EFES_POS_PRIO_IPMF3_EPMF_NOF_ACTIONS;
                Ctest_field_efes_exhaustive_fg_info_array[0].nof_actions =
                    CTEST_DNX_FIELD_EFES_EXHAUSTIVE_IPMF3_NOF_ACTIONS_PER_FG;
                Ctest_field_efes_exhaustive_fg_info_array[1].efes_action_info =
                    Ctest_dnx_field_efes_exhaustive_pos_ipmf3_epmf_action_info_array;
                Ctest_field_efes_exhaustive_fg_info_array[1].nof_actions =
                    CTEST_DNX_FIELD_EFES_EXHAUSTIVE_IPMF3_NOF_ACTIONS_PER_FG;
                exhaustive_pgm_nof_fgs = CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_FGS;
                break;
            }
            case 3:
            {
                field_stage = bcmFieldStageEgress;
                Ctest_field_efes_position_fg_info_array[0].nof_actions =
                    CTEST_DNX_FIELD_EFES_POS_PRIO_IPMF3_EPMF_NOF_ACTIONS;
                Ctest_field_efes_priority_fg_info_array[0].nof_actions =
                    CTEST_DNX_FIELD_EFES_POS_PRIO_IPMF3_EPMF_NOF_ACTIONS;
                Ctest_field_efes_exhaustive_fg_info_array[0].nof_actions =
                    CTEST_DNX_FIELD_EFES_EXHAUSTIVE_EPMF_NOF_ACTIONS_PER_FG;
                Ctest_field_efes_exhaustive_fg_info_array[1].efes_action_info =
                    Ctest_dnx_field_efes_exhaustive_pos_epmf_action_info_array;
                Ctest_field_efes_exhaustive_fg_info_array[1].nof_actions =
                    CTEST_DNX_FIELD_EFES_EXHAUSTIVE_EPMF_NOF_ACTIONS_PER_FG;
                exhaustive_pgm_nof_fgs = CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_FGS;
                Ctest_field_efes_exhaustive_masks_fg_info_array[0].nof_actions =
                    CTEST_DNX_FIELD_EFES_EXHAUSTIVE_MASKS_EPMF_NOF_ACTIONS;
                Ctest_field_efes_exhaustive_masks_fg_info_array[1].nof_actions =
                    CTEST_DNX_FIELD_EFES_EXHAUSTIVE_MASKS_EPMF_NOF_ACTIONS;
                Ctest_field_efes_exhaustive_masks_fg_info_array[2].nof_actions =
                    CTEST_DNX_FIELD_EFES_EXHAUSTIVE_MASKS_EPMF_NOF_ACTIONS;
                Ctest_field_efes_negative_all_efes_prio_fg_info_array[0].nof_actions =
                    CTEST_DNX_FIELD_EFES_EXHAUSTIVE_MASKS_EPMF_NOF_ACTIONS;
                break;
            }
            default:
            {
                field_stage = bcmFieldStageIngressPMF1;
                exhaustive_pgm_nof_fgs = CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_FGS;
                break;
            }
        }

        /** Semantic positive tests. */

        LOG_INFO_EX(BSL_LOG_MODULE, "\nSTAGE %s \n %s%s%s", dnx_field_bcm_stage_text(field_stage), EMPTY, EMPTY, EMPTY);

        LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 1: EFES position \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_position(unit, field_stage,
                                                      Ctest_dnx_field_efes_cs_info_array,
                                                      Ctest_field_efes_position_fg_info_array));

        LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 2: EFES priority \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_priority(unit, field_stage,
                                                      Ctest_dnx_field_efes_cs_info_array,
                                                      Ctest_field_efes_priority_fg_info_array));

        LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 3: EFES sharing \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_sharing(unit, field_stage,
                                                     Ctest_dnx_field_efes_cs_info_array,
                                                     Ctest_field_efes_sharing_fg_info_array));

        /**
         * Following case is not relevant for IPMF3 and EPMF, because they have only
         * one FES array and can't verify jump between arrays.
         */
        if (field_stage != bcmFieldStageIngressPMF3 && field_stage != bcmFieldStageEgress)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 4: EFES Don't care priority/position \n %s%s%s%s", EMPTY, EMPTY, EMPTY,
                        EMPTY);

            SHR_IF_ERR_EXIT(ctest_dnx_field_efes_dont_care_pos_prio(unit, field_stage,
                                                                    Ctest_dnx_field_efes_cs_info_array,
                                                                    Ctest_field_efes_dont_care_pos_prio_fg_info_array));
        }

        LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 5: EFES combination \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_combination(unit, field_stage,
                                                         Ctest_dnx_field_efes_cs_info_array,
                                                         Ctest_field_efes_combination_fg_info_array));

        LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 6: EFES exhaustive \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_exhaustive(unit, field_stage,
                                                        Ctest_dnx_field_efes_cs_info_array,
                                                        Ctest_field_efes_exhaustive_fg_info_array));

        LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 7: EFES exhaustive programs \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_exhaustive_pgm(unit, field_stage, exhaustive_pgm_nof_fgs,
                                                            Ctest_dnx_field_efes_cs_info_array,
                                                            Ctest_field_efes_exhaustive_pgm_fg_info_array));

        LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 8: EFES exhaustive masks \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_exhaustive_masks(unit, field_stage,
                                                              Ctest_dnx_field_efes_cs_info_array,
                                                              Ctest_field_efes_exhaustive_masks_fg_info_array));

        LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 9: EFES descending priorities \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_descending_actions(unit, field_stage,
                                                                Ctest_dnx_field_efes_cs_info_array,
                                                                Ctest_field_efes_descending_fg_info_array));

        /**
         * Following case is not relevant for IPMF3 and EPMF, because they have only
         * one FES array and can't verify jump between arrays.
         */
        if (field_stage != bcmFieldStageIngressPMF3 && field_stage != bcmFieldStageEgress)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "\nCASE 10: EFES Evacuated only place \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

            SHR_IF_ERR_EXIT(ctest_dnx_field_efes_evacuated_only_place_actions(unit, field_stage,
                                                                              Ctest_dnx_field_efes_cs_info_array,
                                                                              Ctest_field_efes_evacuated_only_place_fg_info_array));
        }

        /** Reset global arrays values to the default one, for every stage. */
        ctest_dnx_field_efes_global_arrays_reset();

        /** Semantic negative tests. */

        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal);

        LOG_INFO_EX(BSL_LOG_MODULE, "\nNEGATIVE CASE 1: EFES \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_negative_all_efes_prio(unit, field_stage,
                                                                    Ctest_dnx_field_efes_cs_info_array,
                                                                    Ctest_field_efes_negative_all_efes_prio_fg_info_array));

        LOG_INFO_EX(BSL_LOG_MODULE, "\nNEGATIVE CASE 2: EFES \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_negative_diff_prio_types_predef(unit, field_stage,
                                                                             Ctest_dnx_field_efes_cs_info_array,
                                                                             Ctest_field_efes_negative_diff_prio_types_predef_fg_info_array));

        LOG_INFO_EX(BSL_LOG_MODULE, "\nNEGATIVE CASE 3: EFES \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_negative_diff_prio_types_predef_udf(unit, field_stage,
                                                                                 Ctest_dnx_field_efes_cs_info_array,
                                                                                 Ctest_field_efes_negative_diff_prio_types_predef_udf_fg_info_array));

        LOG_INFO_EX(BSL_LOG_MODULE, "\nNEGATIVE CASE 4: EFES \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_negative_same_type_prio(unit, field_stage,
                                                                     Ctest_dnx_field_efes_cs_info_array,
                                                                     Ctest_field_efes_negative_same_type_num_prio_fg_info_array));

        LOG_INFO_EX(BSL_LOG_MODULE, "\nNEGATIVE CASE 5: EFES \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_negative_same_pos(unit, field_stage,
                                                               Ctest_dnx_field_efes_cs_info_array,
                                                               Ctest_field_efes_negative_same_pos_fg_info_array));

        LOG_INFO_EX(BSL_LOG_MODULE, "\nNEGATIVE CASE 6: EFES \n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(ctest_dnx_field_efes_negative_same_act_diff_prio(unit, field_stage,
                                                                         Ctest_dnx_field_efes_cs_info_array,
                                                                         Ctest_field_efes_negative_same_act_diff_prio_fg_info_array));

        /** Reset global arrays values to the default one, for every stage. */
        ctest_dnx_field_efes_global_arrays_reset();

        /**
         * Set back the severity level of the field proc and dbal to the original values.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbal_dnx);
    }

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbal_dnx);
    SHR_FUNC_EXIT;
}

/**
 * \brief - runs the EFES ctest
 *
 * \param [in] unit - the unit number in system
 * \param [in] args - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "ctest field efes"
 */
shr_error_e
sh_dnx_field_efes_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnx_field_efes_run(unit, args, sand_control));

exit:
    SHR_FUNC_EXIT;
}

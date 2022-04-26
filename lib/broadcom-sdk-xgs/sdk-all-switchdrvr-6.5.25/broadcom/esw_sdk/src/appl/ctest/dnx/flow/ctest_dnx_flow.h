
/*! \file diag_dnx_flow.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef CTEST_DNX_FLOW_H_INCLUDED
#define CTEST_DNX_FLOW_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <src/bcm/dnx/flow/flow_def.h>

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

#define CTEST_DNX_FLOW_SH_CMD_EXEC(_cmd_) \
    sh_process_command(unit, _cmd_)

/** the field will skipped and not tested by the ctest */
#define CTEST_FLOW_SKIP_FIELD -2

/*************
 * VARIBALES *
 *************/

/*************
 * GLOBALS   *
 *************/

/*************
 * FUNCTIONS *
 *************/
extern sh_sand_cmd_t dnx_flow_test_cmds[];

shr_error_e ctest_flow_ut_dyn_tests_creation(
    int unit,
    rhlist_t * test_list);

shr_error_e cmd_dnx_flow_ut(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e ctest_flow_set_by_enabler(
    int unit,
    dnx_flow_app_type_e app_type,
    int enabler,
    void *app_data,
    int value);

shr_error_e ctest_flow_get_by_enabler(
    int unit,
    dnx_flow_app_type_e app_type,
    int enabler,
    void *app_data,
    int *value);

shr_error_e ctest_flow_entry_delete(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * key_special_fields);

shr_error_e ctest_flow_special_field_value_get(
    int unit,
    bcm_flow_special_fields_t * special_fields,
    flow_special_fields_e field_id,
    bcm_flow_special_field_t * special_field_data);

shr_error_e ctest_flow_ut_special_field_legal_value_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    flow_special_fields_e special_field_id,
    bcm_flow_special_field_t * value);

/** get common field with a legal value */
shr_error_e ctest_flow_ut_common_field_legal_value_get(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 result_type,
    int field_enabler,
    int *legal_value);

shr_error_e ctest_flow_match_related_lif_entry_add(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_t * related_lif_app_handle_id,
    bcm_gport_t * flow_id);

/*************
 * UNIT TESTS APIs *
 *************/

#endif /* CTEST_DNX_DBAL_H_INCLUDED */

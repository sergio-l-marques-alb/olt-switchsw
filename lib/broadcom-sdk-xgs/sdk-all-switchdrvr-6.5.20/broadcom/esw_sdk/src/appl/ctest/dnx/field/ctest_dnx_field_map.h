/** \file ctest_dnx_field_map.h
 * Tests for BCM-to-DNX DNX-to-HW mapping data
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
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm/field.h>
/*
 * }
 */

#ifndef CTEST_DNX_FIELD_MAP_H_INCLUDED
/*
 * {
 */
#define CTEST_DNX_FIELD_MAP_H_INCLUDED
#define CTEST_DNX_FIELD_MAP_NEGATIVE_IPMF1_ACTIONS 20
#define CTEST_DNX_FIELD_MAP_NEGATIVE_NOF_STAGES 4
/*
 * }
 */
/**
 * \brief
 *   Command for map tests
 */
extern sh_sand_cmd_t dnx_field_map_test_cmds[];

/*
 * Struct for negative cb testing.
 */
typedef struct
{
    bcm_field_action_t action_type;

    _shr_error_t error_type;

    int value;
} ctest_field_map_neg_testing_t;

#endif /* CTEST_DNX_FIELD_MAP_H_INCLUDED */

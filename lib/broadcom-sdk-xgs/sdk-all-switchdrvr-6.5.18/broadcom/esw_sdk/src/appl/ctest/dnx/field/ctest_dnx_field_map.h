/** \file ctest_dnx_field_map.h
 * Tests for BCM-to-DNX DNX-to-HW mapping data
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
/*
 * }
 */

#ifndef CTEST_DNX_FIELD_MAP_H_INCLUDED
/*
 * {
 */
#define CTEST_DNX_FIELD_MAP_H_INCLUDED

/*
 * }
 */
/**
 * \brief
 *   Command for map tests
 */
extern sh_sand_cmd_t dnx_field_map_test_cmds[];

#endif /* CTEST_DNX_FIELD_MAP_H_INCLUDED */

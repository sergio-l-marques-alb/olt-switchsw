/** \file ctest_dnx_inlif_wide_data.h
 * Purpose: testing of in lif wide data feature
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef CTEST_DNX_IN_LIF_WIDE_DATA_H_INCLUDED
#define CTEST_DNX_IN_LIF_WIDE_DATA_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <appl/diag/sand/diag_sand_framework.h>

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

/*************
 * GLOBALS   *
 *************/
extern sh_sand_cmd_t sh_dnx_inlif_wide_data_test_cmds[];
#if defined(BCM_DNX2_SUPPORT)
extern sh_sand_man_t dnx_inlif_wide_data_man;
#endif
/*************
 * FUNCTIONS *
 *************/

#endif /* CTEST_DNX_IN_LIF_WIDE_DATA_H_INCLUDED */

/** \file resource_manager_unit_test.h
 *
 * Internal DNX resource manager unit test
 *
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifndef ALGO_RES_MNGR_API_UNIT_TEST
/*
 * { 
 */
#define ALGO_RES_MNGR_API_UNIT_TEST

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>

/*
 * structs to support bcm shell command
 */
extern sh_sand_option_t dnx_resource_manager_test_options[];
extern sh_sand_man_t dnx_resource_manager_test_man;

/**
 * \brief
 * run resource manager algorithm unit test
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] args - added to match with shell cmds structure
 *  \param [in] sand_control - added to match with shell cmds structure
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT INPUT
 *  \param [in] *args - added to match with shell cmds structure
 *  \param [in] *sand_control - added to match with shell cmds structure
 * \par INDIRECT OUTPUT
 *   the executed test
 */
shr_error_e dnx_algo_res_unit_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * } 
 */
#endif /* ALGO_RES_MNGR_API_UNIT_TEST */

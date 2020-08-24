/** \file diag_dnx_field_tcam_hit_bit_indication.h
 *
 *      TCAM HIT BIT INDICATION testing.
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
#include <bcm_int/dnx/field/field_map.h>
/*
 * }
 */

#ifndef DIAG_DNX_FIELD_TCAM_HIT_BIT_INDICATION_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_FIELD_TCAM_HIT_BIT_INDICATION_H_INCLUDED

/**
 * \brief
 *   Options list for 'tcam_hit' shell command
 */
extern sh_sand_option_t Sh_dnx_field_tcam_hit_bit_indication_options[];

/**
 * \brief
 *   man for 'tcam_hit' command
 */
extern sh_sand_man_t Sh_dnx_field_tcam_hit_bit_indication_man;
/**
 * \brief
 *   Options list for 'tcam_hit' tests to run on regression, precommit, ...
 */
extern sh_sand_invoke_t Sh_dnx_field_tcam_hit_bit_indication_tests[];

/**
 * \brief - run tcam_hit init sequence in diag shell
 *
 * \param [in] unit - Number of hardware unit used
 *      [in] args - Pointer to args_t struct, not used
 *      [in] sand_control - Pointer to comamnd control structure used for parameter delivery
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
shr_error_e sh_dnx_field_tcam_hit_bit_indication_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* DIAG_DNX_FIELD_TCAM_HIT_BIT_INDICATION_H_INCLUDED */

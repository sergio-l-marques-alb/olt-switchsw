/**
 * \file algo_out_lif_profile.h
 * Internal DNX L3 Management APIs
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef ALGO_OUT_LIF_PROFILE_H_INCLUDED
/*
 * {
 */
#define ALGO_OUT_LIF_PROFILE_H_INCLUDED

#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * Resource name defines for algo out_lif_profile.
 * \see
 * dnx_algo_out_lif_profile_template_create
 * {
 */
#define DNX_ALGO_OUT_LIF_PROFILE                 "out lif profile table"
/**
 * }
 */

/**
 * \brief
 *   Initialize out_lif_profile algorithms.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     The unit number
 * \par INDIRECT INPUT
 *    \see
 *    * dnx_algo_out_lif_profile_template_create header
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *    \see
 *    * dnx_algo_out_lif_profile_template_create header
 * \see
 *   * None
 *****************************************************/
shr_error_e dnx_algo_out_lif_profile_init(
    int unit);

/**
 * \brief
 *   DeInitialize out_lif_profile algorithms.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     The unit number
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   * None
 *****************************************************/
shr_error_e dnx_algo_out_lif_profile_deinit(
    int unit);

/**
 * \brief
 *   Print an out_lif_profile template entry.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     the unit number
 *   \param [in] data -
 *     Pointer to data \n
 *     \b As \b input - \n
 *       pointer to data of type out_lif_template_data_t which
 *       includes properties of outlif profile
 *   \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \par INDIRECT INPUT
 *   * \b *data \n
 *     See 'data' in DIRECT INPUT above
 * \par DIRECT OUTPUT
 *   None
 * \par INDIRECT OUTPUT
 *    the printout
 * \see
 *   * None
*****************************************************/
void dnx_algo_out_lif_profile_print_entry_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/*
 * }
 */
#endif /* ALGO_OUT_LIF_PROFILE_H_INCLUDED */

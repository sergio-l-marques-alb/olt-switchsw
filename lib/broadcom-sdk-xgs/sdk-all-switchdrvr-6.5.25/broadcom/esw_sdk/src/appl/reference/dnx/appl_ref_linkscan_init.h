/** \file appl_ref_linkscan_init.h
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef APPL_REF_LINKSCAN_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_LINKSCAN_INIT_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>

/*
 * }
 */

/**
 * \brief
 * This function initialize linkscan application.
 *
 * \param [in] unit - unit ID
 * \return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 * \see
 *   bcm_error_e
 */
shr_error_e appl_dnx_linkscan_init(
    int unit);

shr_error_e appl_dnx_linkscan_deinit(
    int unit);
/*
 * }
 */
#endif /* APPL_REF_LINKSCAN_INIT_H_INCLUDED */

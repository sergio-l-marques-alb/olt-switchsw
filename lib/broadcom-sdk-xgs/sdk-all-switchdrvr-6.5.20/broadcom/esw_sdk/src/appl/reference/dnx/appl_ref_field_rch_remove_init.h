/** \file appl_ref_field_rch_remove_init.h
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef APPL_REF_FIELD_RCH_REMOVE_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_FIELD_RCH_REMOVE_INIT_H_INCLUDED

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
 * configure PMF context to remove RCH header from packets
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_field_rch_remove_init(
    int unit);
/*
 * }
 */
#endif /* APPL_REF_FIELD_RCH_REMOVE_INIT_H_INCLUDED */

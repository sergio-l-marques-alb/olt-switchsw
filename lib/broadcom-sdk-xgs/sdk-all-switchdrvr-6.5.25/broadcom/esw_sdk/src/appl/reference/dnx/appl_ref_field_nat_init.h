/** \file appl_ref_field_nat_init.h
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef APPL_REF_FIELD_NAT_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_FIELD_NAT_INIT_H_INCLUDED

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
 * configure PMF context for NAT
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_field_nat_init(
    int unit);
/*
 * }
 */
#endif /* APPL_REF_FIELD_NAT_INIT_H_INCLUDED */

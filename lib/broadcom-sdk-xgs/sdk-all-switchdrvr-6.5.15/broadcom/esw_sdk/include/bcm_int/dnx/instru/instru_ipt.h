/**
 * \file bcm_int/dnx/instru/instru_ipt.h
 * Internal DNX INSTRU APIs
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef INSTRU_IPT_H_INCLUDED
/*
 * {
 */
#define INSTRU_IPT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm/types.h>
/*
 * }
 */

/*
 * DEFINES
 * {
 */

/*
 * }
 */

/*
  * Internal functions.
  * {
  */

/**
 * \brief - Set Switch ID for IPT.
 *
 * \param [in] unit - unit id.
 * \param [in] switch_id - IPT switch ID
 *
 * \return
 *   shr_error_e
 *
 */
shr_error_e dnx_instru_ipt_switch_id_set(
    int unit,
    int switch_id);

/**
 * \brief - Get Switch ID for IPT.
 *
 * \param [in] unit - unit id.
 * \param [out] switch_id - IPT switch ID
 *
 * \return
 *   shr_error_e
 *
 */
shr_error_e dnx_instru_ipt_switch_id_get(
    int unit,
    int *switch_id);

/**
 * \brief -
 * Init procedure for intru ipt module
 *
 * \param [in] unit -  Unit-ID
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_instru_ipt_init(
    int unit);

/*
 * }
 */
#endif /* INSTRU_IPT_H_INCLUDED */

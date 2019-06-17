/** \file appl_ref_rx_init.h
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef APPL_REF_RX_INIT_H_INCLUDED
#define APPL_REF_RX_INIT_H_INCLUDED

/**
 * \brief
 * This function initialize and active CPU RX application.
 *
 *\param [in] unit - Relevant unit.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e dnx_rx_appl_active(
    int unit);

#endif /* APPL_REF_RX_INIT_H_INCLUDED */

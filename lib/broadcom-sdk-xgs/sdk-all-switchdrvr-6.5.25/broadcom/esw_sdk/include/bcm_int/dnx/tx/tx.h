/**
 *  \file bcm_int/dnx/tx/tx.h
 *  $Id$
 *  Internal DNX TX APIs
PIs $Copyright: (c) 2021 Broadcom.
PIs Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _TX_API_INCLUDED__
/*
 * {
 */
#define _TX_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief
 *   Initialize BCM TX API
 * \par DIRECT INPUT:
 *   \param [in] unit  -  Unit ID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e - Error Type
 * \par INDIRECT OUTPUT:
 *   * None
 * \remark
 *   Please see bcm_common_tx_init
 * \see
 *   * None
 */
shr_error_e dnx_tx_init(
    int unit);

/**
 * \brief
 *   De-initialize BCM TX API
 * \par DIRECT INPUT:
 *   \param [in] unit  -  Unit ID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e - Error Type
 * \par INDIRECT OUTPUT:
 *   * None
 * \remark
 *   free resource allocated in bcm_tx_init
 * \see
 *   * None
 */
shr_error_e dnx_tx_deinit(
    int unit);

/*
 * }
 */
#endif/*_TX_API_INCLUDED__*/

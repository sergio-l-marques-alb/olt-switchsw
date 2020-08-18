/**
 * \file bcm_int/dnx/l2/l2_traverse.h 
 * Internal DNX L2 APIs 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef L2_TRAVERSE_H_INCLUDED
/*
 * {
 */
#define L2_TRAVERSE_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */

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
 * \brief - Init the DMA channel of the flush machine
 * 
 * \param [in] unit - unit id
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_flush_dma_init(
    int unit);

/**
 * \brief - Stop and free the dma channel of the flush
 * 
 * \param [in] unit - unit id
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_flush_dma_deinit(
    int unit);

/*
 * }
 */
#endif /* L2_TRAVERSE_H_INCLUDED */

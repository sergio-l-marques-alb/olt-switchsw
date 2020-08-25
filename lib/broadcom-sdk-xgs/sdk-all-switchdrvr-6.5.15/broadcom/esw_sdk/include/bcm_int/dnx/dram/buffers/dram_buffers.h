/** \file bcm_int/dnx/dram/buffers/dram_buffers.h
 *
 * This file contains dram buffers main structure and routine declarations for the Dram buffers operation.
 *
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _BCMINT_DNX_DRAM_BUFFERS_DRAMBUFFERS_H_INCLUDED
/*
 * {
 */
#define _BCMINT_DNX_DRAM_BUFFERS_DRAMBUFFERS_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/**
 * \brief - init sequence for dram buffers
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dram_buffers_init(
    int unit);

/**
 * \brief - deinit sequence for dram buffers
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dram_buffers_deinit(
    int unit);

/**
 * \brief - returns array with either deleted or quarantined buffers for given core.
 *
 * \param [in] unit - unit number
 * \param [in] core - core index
 * \param [in] deleted - if TRUE, returned result is deleted buffers, else - return result is quarantined buffers.
 * \param [out] buffers_array - filled array with buffers that match criteria (deleted/quarantined)
 * \param [in] max_size - buffers_array max size
 * \param [out] size - number of buffer found that match criteria (deleted/quarantined), if bigger than max_size,
 *                     buffer_array will contain only the first max_size buffers found
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * buffers that are about to be deleted (will be deleted after next time they are used) are counted as deleted buffers.
 * \see
 *   * None
 */
shr_error_e dnx_dram_buffers_status_get(
    int unit,
    int core,
    uint8 deleted,
    uint32 *buffers_array,
    int max_size,
    int *size);

/*
 * }
 */
#endif /* _BCMINT_DNX_DRAM_BUFFERS_DRAMBUFFERS_H_INCLUDED */

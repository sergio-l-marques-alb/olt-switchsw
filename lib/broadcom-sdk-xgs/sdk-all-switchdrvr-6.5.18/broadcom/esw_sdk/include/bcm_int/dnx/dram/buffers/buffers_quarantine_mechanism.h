/** \file bcm_int/dnx/dram/buffers/buffers_quarantine_mechanism.h
 *
 * This file contains dram buffers quarantine mechanism functions that are exposed to other modules beside dram.
 *
 */

/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _BCMINT_DNX_DRAM_BUFFERS_BUFFERSQUARANTINEMECHANISM_H_INCLUDED
/*
 * {
 */
#define _BCMINT_DNX_DRAM_BUFFERS_BUFFERSQUARANTINEMECHANISM_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/**
 * \brief - schedule a buffer to be quarantined the next time it is read.
 *
 * \param [in] unit - unit number
 * \param [in] core - core number
 * \param [in] buffer - bdb to schedule to quarantine
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dram_buffers_quarantine_buffer_to_quarantine_schedule(
    int unit,
    bcm_core_t core,
    uint32 buffer);

/**
 * \brief - get number of free BDBs
 *
 * \param [in] unit - unit number
 * \param [out] nof_free_buffers - number of free buffers
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dram_buffers_quarantine_nof_free_buffers_get(
    int unit,
    uint32 *nof_free_buffers);

/**
 * \brief - check that a buffer which was scheduled to move to quarantine FIFO was moved.
 *          if buffer was moved should return success, else failure.
 *
 * \param [in] unit - unit number
 * \param [in] core - core number
 *
 * \return
 *   _SHR_E_NONE - on success.
 *   _SHR_E_TIMEOUT - when buffer did not moved to quarantine fifo
 *   _SHR_E...      - on failure
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dram_buffers_quarantine_buffer_to_quarantine_done(
    int unit,
    bcm_core_t core);

/**
 * \brief - restore all buffers currently found in quarantine fifo
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
shr_error_e dnx_dram_buffers_quarantine_buffer_flush_all(
    int unit);

/**
 * \brief - get number of deleted BDBs
 *
 * \param [in] unit - unit number
 * \param [out] nof_deleted_buffers - number of deleted buffers
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dram_buffers_quarantine_nof_deleted_buffers_get(
    int unit,
    uint32 *nof_deleted_buffers);

/**
 * \brief - corrective action for interrupt handler when quarantine FIFO is not empty. this function counts that the buffer was quarantined and releases it back to the main pool of buffers - if however the buffer was quarantined above a defined number of times, it is deleted.
 *
 * \param [in] unit - unit number
 * \param [in] core - core of the quarantine fifo which is currently not empty
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_dram_buffers_quarantine_fifo_not_empty_interrupt_handler(
    int unit,
    bcm_core_t core);

/**
 * \brief - Restore deleted dram buffers after soft init.
 *   When running soft init, the list of deleted dram buffers is reset
 *   in HW, so we want to reconstruct.
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * This function is called at the end of soft init (soc_dnx_soft_init).
 *   * Shouldn't run when drams are not loaded.
 *   * Shouldn't run before dram buffers are initialized.
 * \see
 *   * None
 */
shr_error_e dnx_dram_buffers_quarantine_restore_deleted_buffers_after_soft_init(
    int unit);

/*
 * }
 */
#endif /* _BCMINT_DNX_DRAM_BUFFERS_BUFFERSQUARANTINEMECHANISM_H_INCLUDED */

/** \file src/bcm/dnx/dram/buffers/buffers_quarantine_mechanism.h
 * 
 * Internal DNX dram buffers APIs
 * 
 */

/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _BCM_DNX_DRAM_BUFFERS_BUFFERSQUARANTINEMECHANISM_H_INCLUDED
/*
 * { 
 */
#define _BCM_DNX_DRAM_BUFFERS_BUFFERSQUARANTINEMECHANISM_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <shared/shrextend/shrextend_debug.h>
/**
 * Number of times to poll total number of free buffers while it is
 * not as expected (dnx_data_dram.buffers.nof_bdbs_get(unit)).
 * See dnx_dram_buffers_bdbs_pre_post_deletion_enable().
 */
#define NUM_POLL_TOTAL_COUNT 10
/**
 * \brief - init buffers quarantine mechanism
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
shr_error_e dnx_dram_buffers_quarantine_init(
    int unit);

#endif /* _BCM_DNX_DRAM_BUFFERS_BUFFERSQUARANTINEMECHANISM_H_INCLUDED */

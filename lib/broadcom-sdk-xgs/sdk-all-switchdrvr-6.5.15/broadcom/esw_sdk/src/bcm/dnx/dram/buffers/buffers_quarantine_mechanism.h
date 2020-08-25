/** \file src/bcm/dnx/dram/buffers/buffers_quarantine_mechanism.h
 * 
 * Internal DNX dram buffers APIs
 * 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
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

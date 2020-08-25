/*
 * \file src/bcm/dnx/cosq/ipq.h
 *
 *  Created on: May 30, 2018
 *      Author: si888124
 */

#ifndef _LOCAL_DNX_IPQ_H_INCLUDED_
#define _LOCAL_DNX_IPQ_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <bcm/cosq.h>

/**
 * \brief -
 * Configure the queue region interdigitated mode.
 * (whether the queues in the region are mapped to continous connectors on egress side or not)
 * this method also resets all queues in the region, in order to prevent illegal
 * interdigitated/composite state.
 * Queue Quartets that configured to be in interdigitated mode should only be configured with
 * interdigitated flow quartets, and the other-way around.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] queue_region - queue region index
 * \param [in] is_non_contiguous - specify whether the region should be in interdigitated mode.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_region_config_set(
    int unit,
    int queue_region,
    int is_non_contiguous);

/**
 * \brief -
 * Map system TC (TC on the packet) to VOQ offset, according to destination type.
 *
 * \param [in] unit - Unit-ID
 * \param [in] port - Gport for the type of destination (system port or queue group)
 * \param [in] priority - system TC
 * \param [in] cosq - VOQ offset
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_tc_mapping_set(
    int unit,
    bcm_port_t port,
    bcm_cos_t priority,
    bcm_cos_queue_t cosq);

/**
 * \brief -
 * Get system TC (TC on the packet) to VOQ offset according to destination type
 *
 * \param [in] unit - Unit-ID
 * \param [in] port - Gport for the type of destination (system port or queue group)
 * \param [in] priority - system TC
 * \param [out] cosq - VOQ offset
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_tc_mapping_get(
    int unit,
    bcm_port_t port,
    bcm_cos_t priority,
    bcm_cos_queue_t * cosq);

/**
 * \brief
 *   Initialize IPQ allocation manager.
 *
 * \param [in] unit -
 *   Relevant unit.
 * \retval shr_error_e -
 *   Error return value
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
shr_error_e dnx_cosq_ipq_alloc_mngr_init(
    int unit);

/**
 * \brief
 *   Deinitialize IPQ allocation manager.
 *   Currently doens't do anything, since resource manager doesn't require
 *    deinitialization per resource, and the sw state is deinitialized in
 *    dnx_ipq_deinit.
 *
 * \param [in] unit -
 *   Relevant unit.
 * \retval shr_error_e -
 *   Error return value
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
shr_error_e dnx_cosq_ipq_alloc_mngr_deinit(
    int unit);

/*
 * }
 */

#endif /** _LOCAL_DNX_IPQ_H_INCLUDED_ */

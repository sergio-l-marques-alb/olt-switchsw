/** \file bcm_int/dnx/cosq/ingress/iqs_api.h
 * $Id$
 *
 * APIs and High Level functions of IQS
 * Module IQS - Ingress Queue Scheduling 
 *  
 * Configuration of Ingress TM queue scheduling, includes:
 *  * Credit request profiles
 *  * Credit worth
 *  * FSM reorder mechanism
 *  
 *  Not including:
 *  * FMQ Credit generation(legacy implementation)
 * 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNX_IQS_API_INCLUDED__
#define _DNX_IQS_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <bcm/cosq.h>
/*
 * Defines:
 * {
 */

/*
 * } 
 */
/*
 * Typedefs:
 * {
 */

 /*
  * } 
  */
/*
 * Functions
 * {
 */
/**
 * \brief - 
 * Map delay tolerance level to actual credit request profile-id
 
 * \param [in] unit -  Unit-ID 
 * \param [in] delay_tolerance_level - delay tolerance level defines or the actual profile id
 * \param [in] profile_id - credit request profile id
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_iqs_api_delay_tolerance_level_to_profile_id_get(
    int unit,
    int delay_tolerance_level,
    int *profile_id);
/**
 * \brief -
 * Range of queues supporting credit watchdog mechanism.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] range - is_enabled - enable / disable watchdog mechanism
 *                     range_min - min queue id.
 *                     range_max - max queue id.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_api_wd_range_set(
    int unit,
    bcm_cosq_range_t * range);

/**
 * \brief -
 * Range of queues supporting credit watchdog mechanism.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] range - is_enabled - enable / disable watchdog mechanism
 *                     range_min - min queue id.
 *                     range_max - max queue id.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_api_wd_range_get(
    int unit,
    bcm_cosq_range_t * range);

/**
 * \brief -
 *  Assign ingress queue to credit request profile.
 *  When creating a ingress queue using API bcm_cosq_ingress_queue_bundle_gport_add() the queue assigned to credit request profile according to user input.
 *  This API will allow modifying the credit request profile.
 *
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - Queue bundle gport. Created by API bcm_cosq_ingress_queue_bundle_gport_add().
 * \param [in] cosq - queue offset in the bundle
 * \param [in] delay_tolerance_id - credit request profile (defined by API bcm_cosq_delay_tolerance_level_set())
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_api_queue_to_request_profile_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int delay_tolerance_id);

/**
 * \brief -
 *  Get mapping of ingress queue to credit request profile.
 *  When creating a ingress queue using API bcm_cosq_ingress_queue_bundle_gport_add() the queue assigned to credit request profile according to user input.
 *  This API will allow modifying the credit request profile.
 *
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - Queue bundle gport. Created by API bcm_cosq_ingress_queue_bundle_gport_add().
 * \param [in] cosq - queue offset in the bundle
 * \param [in] delay_tolerance_id - credit request profile (defined by API bcm_cosq_delay_tolerance_level_set())
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_api_queue_to_request_profile_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *delay_tolerance_id);

/*
 * }
 */

#endif/*_DNX_COSQ_INCLUDED__*/

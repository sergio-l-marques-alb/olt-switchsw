
/** \file bcm_int/dnx/cosq/ingress/compensation.h
 * $Id$
 * 
 * Compansation APIs
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _COMPENSATION_H_INCLUDED_
#define _COMPENSATION_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <shared/gport.h>

/**
 * \brief
 * Set per-in-port enable for Scheduler Compensation
 * \param [in] unit - unit
 * \param [in] gport - null gport
 * \param [in] cosq - cosq (unused)
 * \param [in] enable - enable
 * \return
 *    shr_error_e
 * \remark
 *   None
 * \see
 *   NONE
 */
shr_error_e dnx_cosq_ingress_compensation_scheduler_per_port_enable_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int enable);

/**
 * \brief
 * Get per-in-port enable for Scheduler Compensation
 * \param [in] unit - unit
 * \param [in] gport - null gport
 * \param [in] cosq - cosq (unused)
 * \param [out] enable - enable
 * \return
 *    shr_error_e
 * \remark
 *   None
 * \see
 *   NONE
 */
shr_error_e dnx_cosq_ingress_compensation_scheduler_per_port_enable_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *enable);

#endif /** _COMPENSATION_H_INCLUDED_ */

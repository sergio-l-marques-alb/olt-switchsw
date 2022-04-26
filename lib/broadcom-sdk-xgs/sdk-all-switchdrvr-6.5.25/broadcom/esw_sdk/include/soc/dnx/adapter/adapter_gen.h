/** \file adapter_gen.h
 * This file handles general adapter functionality
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$ $
 */

#ifndef ADAPTER_GEN_H_INCLUDED
/* { */
#define ADAPTER_GEN_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <include/soc/register.h>
#include <shared/shrextend/shrextend_debug.h>

/**
 * \brief - map egr interface to first lane. and write it to reserved table.
 * This is implementation for adapter TM issue -
 * adapter can't get the first lane information based on egr_if, if the port is ilkn.
 * Therfore, we created specific adapter database for this information. (for all egr_ifs)
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [in] egr_if -  egr_if
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e adapter_egq_interface_port_map_set(
    int unit,
    bcm_port_t port,
    int egr_if);

/**ADAPTER_REG_ACCESS_H*/
#endif

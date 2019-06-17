/*! \file bcm_int/dnx/srv6/srv6.h
 *
 * Internal DNX SRv6 APIs
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#include <bcm/types.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnx/dbal/dbal.h>

#ifndef _SRV6_API_INCLUDED__
/*
 * {
 */
#define _SRV6_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files
 * {
 */

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * }
 */
/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * INERNAL TYPES
 * {
 */
/*
 * }
 */

/*
 * APIS
 * {
 */
/**
 * \brief 
 *      Initialize DNX SRv6 module 
 * \param [in] unit -
 *     Relevant unit.
 * \return
 *   shr_error_e -\n
 *     See error code description.
 * \see
 *   shr_error_e
 */
shr_error_e dnx_srv6_init(
    int unit);

/**
 * \brief 
 *      De-initialize DNX SRv6 module 
 * \param [in] unit -
 *     Relevant unit.
 * \return
 *   shr_error_e -\n
 *     See error code description.
 * \see
 *   shr_error_e
 */
shr_error_e dnx_srv6_deinit(
    int unit);
/*
 * } 
 */
#endif/*_SRv6_API_INCLUDED__*/

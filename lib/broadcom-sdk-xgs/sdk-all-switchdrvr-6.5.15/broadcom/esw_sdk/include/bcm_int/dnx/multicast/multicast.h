/*
 * ! \file bcm_int/dnx/multicast/multicast.h Internal DNX VLAN APIs
PIs $Copyright: (c) 2018 Broadcom.
PIs Broadcom Proprietary and Confidential. All rights reserved.$ 
 */

#ifndef _DNX_MULTICAST_API_INCLUDED__
/*
 * { 
 */
#define _DNX_MULTICAST_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/multicast.h>
#include <soc/dnx/swstate/auto_generated/access/multicast_access.h>

#define DNX_MULTICAST_ALGO_RES_STR                     "MCDB"

#define DNX_MULTICAST_EGR_INVALID_DESTINATION           0x1ff
#define DNX_MULTICAST_ING_INVALID_DESTINATION           0x1fffff
#define DNX_MULTICAST_INVALID_CUD                       0x3fffff

#define DNX_MULTICAST_IS_BIER(flags)  (((flags & BCM_MULTICAST_BIER_64_GROUP) || \
                                        (flags & BCM_MULTICAST_BIER_128_GROUP) || \
                                        (flags & BCM_MULTICAST_BIER_256_GROUP)) ? 1 : 0)
#define DNX_MULTICAST_IS_INGRESS(flags) ((flags & BCM_MULTICAST_INGRESS_GROUP) ? 1 : 0)
#define DNX_MULTICAST_IS_EGRESS(flags) ((flags & BCM_MULTICAST_EGRESS_GROUP) ? 1 : 0)

/**
 * \brief - Initialize dnx multicast module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * DNX data related multicast module information
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_init(
    int unit);

/**
 * \brief - Deinitialize dnx multicast module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_deinit(
    int unit);

/*
 * MACROs
 * {
 */

/*
 * }
 */

/*
 * } 
 */
#endif/*_DNX_MULTICAST_API_INCLUDED__*/

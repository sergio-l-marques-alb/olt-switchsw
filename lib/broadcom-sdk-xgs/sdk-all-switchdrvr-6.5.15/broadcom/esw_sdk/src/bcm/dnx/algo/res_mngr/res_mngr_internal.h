/** \file res_mngr_internal.h
 * 
 * Internal DNX resource manager APIs 
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifndef ALGO_RES_MNGR_INTERNAL_INCLUDED
/* { */
#define ALGO_RES_MNGR_INTERNAL_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
* INCLUDE FILES:
* {
*/
#include <bcm_int/dnx/algo/res_mngr/res_mngr_callbacks.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
/**
 * }
 */

#define DNX_ALGO_RES_MNGR_NOF_RESOURCES (100)

#define DNX_ALGO_RES_INVALID_INDEX -1

/**
 *  
 *   General types for internal resource manager management.
 */

extern const dnx_algo_res_advanced_alogrithm_cb_t Resource_callbacks_map[DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_COUNT];

/* } */
#endif/*_ALGO_RES_MNGR_INTERNAL_INCLUDED__*/

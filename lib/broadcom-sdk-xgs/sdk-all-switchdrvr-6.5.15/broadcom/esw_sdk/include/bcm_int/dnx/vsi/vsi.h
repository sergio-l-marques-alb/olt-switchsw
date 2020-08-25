/*! \file bcm_int/dnx/vsi/vsi.h
 *
 * Internal DNX VSI APIs
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _VSI_API_INCLUDED__
/*
 * {
 */
#define _VSI_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/** Default vsi value*/
#define DNX_VSI_DEFAULT (0)

/*
 * }
 */
/*
 * MACROs
 * {
 */

/**
 * \brief
 * Verify vsi is within the valid range.
 */
#define DNX_VSI_VALID(_unit, _vsi) (_vsi < dnx_data_l2.vsi.nof_vsis_get(_unit))

/*
 * }
 */

/*
 * { 
 */

/*
 * } 
 */
#endif/*_VSI_API_INCLUDED__*/

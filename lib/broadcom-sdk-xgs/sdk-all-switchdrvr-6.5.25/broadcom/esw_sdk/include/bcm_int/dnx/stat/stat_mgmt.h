 /** \file bcm_int/dnx/stat/stat_mgmt.h
 * 
 * Internal DNX STAT MGMT
 * 
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _DNX_STAT_MGMT_INCLUDED__
/*
 * { 
 */
#define _DNX_STAT_MGMT_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief
 *      initialize reject bitmap filter masks
 * \param [in] unit - unit id 
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_stat_mgmt_reject_filter_masks_init(
    int unit);

#endif/*_DNX_STAT_MGMT_INCLUDED__*/

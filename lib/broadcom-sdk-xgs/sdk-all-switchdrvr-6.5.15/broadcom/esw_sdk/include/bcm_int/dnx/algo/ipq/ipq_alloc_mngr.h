/**
 * \file algo/ipq/ipq_alloc_mngr.h 
 *
 * Internal DNX IPQ Alloc Mngr APIs
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef __IPQ_ALLOC_MNGR_H_INCLUDED__
#define __IPQ_ALLOC_MNGR_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Includes
 * {
 */
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnxc/swstate/types/sw_state_resource_tag_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_res_mngr.h>
#include <soc/dnxc/swstate/types/sw_state_template_mngr.h>
/*
 * }
 */

/*
 * Structs:
 * {
 */

/*
 * Functions:
 * {
 */

/**
 * \brief
 *   Callback function used to print the TC profile mapping
 *
 * \param [in] unit - Relevant unit.
 * \param [in] data - data to print. here it is the TC to VOQ offset mapping
 * \param [in] print_cb_data - template manager print structure.
 *
 * \retval shr_error_e -
 *   Error return value
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
void dnx_algo_ipq_tc_map_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/*
 * } 
 */

#endif /* __IPQ_ALLOC_MNGR_H_INCLUDED__ */

/** \file algo/ipq/ipq_alloc_mngr.c
 *
 * Implementation for advanced IPQ resource manager.
 *
 * Manage queue allocation.
 * Split queues bitmap to regions, and have the ability to allocate a
 * queue from a specific region.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/**
* Include files:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ipq_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ipq_alloc_mngr_access.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <bcm_int/dnx/algo/res_mngr/simple_bitmap_wrap.h>
#include <bcm_int/dnx/algo/ipq/ipq_alloc_mngr.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>

/** 
 * } 
 */

/**
* Functions:
* {
*/

/**
 * See .h file.
 */
void
dnx_algo_ipq_tc_map_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_ipq_tc_map_t *tc_mapping = (dnx_ipq_tc_map_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_ARR(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "In-TC -> COSQ", tc_mapping->tc, NULL, NULL, 8);

    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
}

/**
 * }
 */

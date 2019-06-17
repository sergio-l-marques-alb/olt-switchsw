/** \file template_mngr_callbacks.c
 *
 * Callbacks functions for template manager.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/template_mngr/multi_set_template.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <bcm_int/dnx/algo/rx/trap_mtu_profile_allocation.h>
#include <bcm_int/dnx/algo/rx/trap_etpp_lif_profile_allocation.h>

/*
 * }
 */
/* *INDENT-OFF* */
const sw_state_dnx_algo_template_advanced_alogrithm_cb_t Template_callbacks_map_sw[DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_COUNT] = {
    /*
     * DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC
     */
    {multi_set_template_create,
     multi_set_template_allocate,
     NULL,
     multi_set_template_profile_data_get,
     multi_set_template_profile_get,
     multi_set_template_free,
     multi_set_template_destroy,
     NULL,
     multi_set_template_clear,
     NULL,
     NULL},
     /*
      * DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_SMART_TEMPLATE
      */
     {dnx_algo_smart_template_create,
     dnx_algo_smart_template_allocate,
     NULL,
     multi_set_template_profile_data_get,
     multi_set_template_profile_get,
     dnx_algo_smart_template_free,
     NULL,
     NULL,
     dnx_algo_smart_template_clear,
     NULL,
     NULL},
     /*
      * DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_L3_SOURCE_TABLE
      */
     {dnx_algo_l3_source_address_table_create,
      dnx_algo_l3_source_address_table_allocate,
      NULL,
      multi_set_template_profile_data_get,
      multi_set_template_profile_get,
      dnx_algo_l3_source_address_table_free,
      NULL,
      NULL,
      dnx_algo_l3_source_address_table_clear,
      NULL,
      NULL},
    /*
     * DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_TRAP_MTU_PROFILE
     */
    {multi_set_template_create,
     dnx_algo_rx_trap_mtu_profile_allocate,
     NULL,
     multi_set_template_profile_data_get,
     multi_set_template_profile_get,
     multi_set_template_free,
     multi_set_template_destroy,
     NULL,
     multi_set_template_clear,
     NULL,
     NULL},
    /*
     * DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_TRAP_ETPP_LIF_PROFILE
     */
    {multi_set_template_create,
     dnx_algo_rx_trap_etpp_lif_profile_allocate,
     NULL,
     multi_set_template_profile_data_get,
     multi_set_template_profile_get,
     multi_set_template_free,
     multi_set_template_destroy,
     NULL,
     multi_set_template_clear,
     NULL,
     NULL}
};
/* *INDENT-ON* */

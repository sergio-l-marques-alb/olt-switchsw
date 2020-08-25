/** \file template_mngr_print_callbacks.c
 *
 * Print functions for template manager.
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

/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/algo/bfd/algo_bfd.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <bcm_int/dnx/algo/trunk/algo_trunk.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/mirror/mirror.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/sat/sat.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
/*
 * }
 */

/*
 * Other include files. 
 * { 
 */
#include <bcm/types.h>
#include <shared/swstate/sw_state.h>
/*
 * }
 */

/*
 * Include files for the templates: 
 * { 
 */
#include <soc/dnx/swstate/auto_generated/types/example_temp_mngr_types.h>

/*
 * }
 */
/**
 * }
 */

typedef struct
{
    dnx_algo_template_name_t template_name;
    dnx_algo_template_print_data_cb print_cb;
} dnx_algo_template_print_map_t;

void
dnx_algo_template_test_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    template_mngr_example_data_t *template_data = (template_mngr_example_data_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "element_8", template_data->element_8, NULL,
                                        "0x%x");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT16, "element_16", template_data->element_16, NULL,
                                        "0x%x");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "element_32", template_data->element_32, NULL,
                                        "0x%x");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "elemrnt_int", template_data->element_int, NULL,
                                        "0x%x");
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
    return;
}

void
dnx_algo_template_print_cb_add_line_data(
    void *data,
    dnx_algo_template_print_t * print_cb_data)
{

    switch (print_cb_data->type)
    {
        case TEMPLATE_MNGR_PRINT_TYPE_UINT8:
        {
            sal_memcpy(&print_cb_data->data, (uint8 *) data, sizeof(uint8));
            break;
        }
        case TEMPLATE_MNGR_PRINT_TYPE_UINT16:
        {
            sal_memcpy(&print_cb_data->data, (uint8 *) data, sizeof(uint16));
            break;
        }
        case TEMPLATE_MNGR_PRINT_TYPE_UINT32:
        {
            sal_memcpy(&print_cb_data->data, (uint8 *) data, sizeof(uint32));
            break;
        }
        case TEMPLATE_MNGR_PRINT_TYPE_CHAR:
        {
            sal_memcpy(&print_cb_data->data, (uint8 *) data, sizeof(char));
            break;
        }
        case TEMPLATE_MNGR_PRINT_TYPE_SHORT:
        {
            sal_memcpy(&print_cb_data->data, (uint8 *) data, sizeof(short));
            break;
        }
        case TEMPLATE_MNGR_PRINT_TYPE_INT:
        {
            sal_memcpy(&print_cb_data->data, (uint8 *) data, sizeof(int));
            break;
        }
        case TEMPLATE_MNGR_PRINT_TYPE_MAC:
        {
            sal_memcpy(&print_cb_data->data, (uint8 *) data, sizeof(bcm_mac_t));
            break;
        }
        case TEMPLATE_MNGR_PRINT_TYPE_IPV4:
        {
            sal_memcpy(&print_cb_data->data, (uint8 *) data, sizeof(bcm_ip_t));
            break;
        }
        case TEMPLATE_MNGR_PRINT_TYPE_IPV6:
        {
            sal_memcpy(&print_cb_data->data, (uint8 *) data, sizeof(bcm_ip6_t));
            break;
        }
        case TEMPLATE_MNGR_PRINT_TYPE_STRING:
        {
            sal_memcpy(&print_cb_data->data, (uint8 *) data, DNX_ALGO_TEMPLATE_PRINT_CB_MAX_PRINT_STRING_SIZE - 1);
            break;
        }
        default:
        {
            break;
        }
    }

    return;
}

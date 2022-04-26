
/*
 * $Id: ramon_intr_init.c, v1 16/06/2014 09:55:39 azarrin $
 *
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Purpose:    Implement soc interrupt handler.
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <soc/intr.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_intr.h>
#include <soc/dnxc/dnxc_intr_handler.h>
#include <soc/dnxc/error.h>
#include <soc/drv.h>
#include <soc/intr_cmicx.h>
#include <soc/dnxc/intr.h>
#include <soc/dnxc/dnxc_intr.h>
#include <soc/dnxf/cmn/dnxf_intr.h>

/*************
 * DEFINES   *
 *************/
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INTR

/*************
 * DECLARATIONS *
 *************/

/*
* Function:
*      soc_dnxf_ser_init
* Purpose:
*      List all mem_mask reigster, unmask all memory for SER
* Parameters:
*      unit                    - (IN)  Device number
*
* Returns:
*      _SHR_E_XXX
*/
int
soc_dnxf_ser_init(
    int unit)
{
    const dnxf_data_intr_ser_mem_mask_t *mem_mask;
    const dnxc_data_table_info_t *table_info;
    int mask_index;
    SHR_FUNC_INIT_VARS(unit);

    table_info = dnxf_data_intr.ser.mem_mask_info_get(unit);
    for (mask_index = 0; mask_index < table_info->key_size[0]; mask_index++)
    {
        mem_mask = dnxf_data_intr.ser.mem_mask_get(unit, mask_index);
        if (mem_mask->reg != INVALIDr)
        {
            switch (mem_mask->mode)
            {
                case dnxc_mem_mask_mode_none:
                {
                    /** do nothing */
                    break;
                }
                case dnxc_mem_mask_mode_zero:
                {
                    SHR_IF_ERR_EXIT(soc_dnxc_set_mem_mask(unit, mem_mask->reg, mem_mask->field, 0));
                    break;
                }
                case dnxc_mem_mask_mode_one:
                {
                    SHR_IF_ERR_EXIT(soc_dnxc_set_mem_mask(unit, mem_mask->reg, mem_mask->field, 1));
                    break;
                }
                default:
                {
                    LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Wrong memory mask mode %d, table index %d\n"),
                                                mem_mask->mode, mask_index));
                    break;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE

/*
 * $Id: dfe_intr.c,v 1.87 Broadcom SDK $

 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

/*************
 * INCLUDES  *
 *************/
#include <sal/core/dpc.h>

#include <soc/error.h>
#include <soc/ipoll.h>
#include <soc/drv.h>

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/fe1600/fe1600_interrupts.h> 

/*************
 * DEFINES   *
 *************/
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR

/*************
 * FUNCTIONS *
 *************/
int soc_fe1600_interrupt_deinit(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    /* before deinit interrupt, make sure that DPC interrupt handler was completed */
    sal_dpc_disable_and_wait(INT_TO_PTR(unit));

    /* mask all interrupts in cmic (even in WB mode). This Masking updates the WB DB, although we don't use WB DB. */ 
    if (soc_fe1600_interrupt_all_enable_set(unit, 0)) {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "error disabling interrupts\n")));
    }

    if(!SAL_BOOT_NO_INTERRUPTS) {
        if (soc_property_get(unit, spn_POLLED_IRQ_MODE, 1)) {
            if (soc_ipoll_disconnect(unit) < 0) {
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "error disconnecting polled interrupt mode\n")));
            }
        } else {
            /* unit # is ISR arg */
            if (soc_cm_interrupt_disconnect(unit) < 0) {
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "could not disconnect interrupt line\n")));
            }
        }
    }



    if(!SAL_BOOT_NO_INTERRUPTS) 
    {
        fe1600_interrupts_dfe_control_data_deinit(unit);
        
        fe1600_interrupts_array_deinit(unit);       
    }

    SOCDNX_FUNC_RETURN;
}

int soc_fe1600_interrupt_init(int unit) 
{
    int rv;
    soc_control_t* soc;

    SOCDNX_INIT_FUNC_DEFS;
    /* Start to allow this unit to run DPCs */
    sal_dpc_enable(INT_TO_PTR(unit));

    soc = SOC_CONTROL(unit);

    if(!SAL_BOOT_NO_INTERRUPTS) {
        rv = fe1600_interrupts_array_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = fe1600_interrupts_dfe_control_data_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }
 

    /* take from HW the interrupts mask */
    /* note: there is al so high level of cmic mask that dosn't saved in wormboot. */

    soc_intr_block_lo_disable(unit, 0xFFFFFFFF);
    soc_intr_block_hi_disable(unit, 0xFFFFFFFF);

    if(!SAL_BOOT_NO_INTERRUPTS) {
        if (soc_property_get(unit, spn_POLLED_IRQ_MODE, 1)) {
            if (soc_ipoll_connect(unit, dfe_local_soc_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error initializing polled interrupt mode")));
            }
            soc->soc_flags |= SOC_F_POLLED;
        } else {
            /* unit # is ISR arg */
            if (soc_cm_interrupt_connect(unit, dfe_local_soc_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("could not connect interrupt line")));
            }
        }
    }
exit:
    if (SOCDNX_FUNC_ERROR)
    {
        soc_fe1600_interrupt_deinit(unit);
    }
    SOCDNX_FUNC_RETURN;
}


/** \file intr.c
 * Slim SoC module to allow bcm actions.
 * 
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 */

/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_INIT

/*
 * INCLUDE FILES:
 * {
 */
#include <sal/core/dpc.h>
#include <soc/cm.h>
#include <shared/bsl.h>
#include <soc/intr.h>
#include <soc/ipoll.h>
#include <soc/iproc.h>
#include <soc/drv.h>
#include <soc/intr_cmicx.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/intr.h>
#include <soc/dnxc/dnxc_intr_handler.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#endif
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_intr.h>
#endif

/*
 * }
 */

/**
 * \brief - init interrupt at soc layer.
 *
 * \param [in] unit - unit #
 * \return
 * \see
 *   * None
 */
int
soc_dnxc_intr_init(
    int unit)
{
    int irq_polled = 1;
    SHR_FUNC_INIT_VARS(unit);

    if (soc_cmic_intr_init(unit) < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error initializing cmic interrupt");
    }
    /*
     * Disable all interrupts
     */
    soc_cmic_intr_all_disable(unit);

    /*
     * Start to allow this unit to run DPCs 
     */
    if (sal_dpc_enable(INT_TO_PTR(unit)) < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error enable dpc");
    }
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        irq_polled = dnx_data_intr.general.polled_irq_mode_get(unit);
    }
    else
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        irq_polled = dnxf_data_intr.general.polled_irq_mode_get(unit);
    }
    else
#endif
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Should be dnx or dnxf device");
    }
    /*
     * connect interrupts / start interrupt thread
     */
    if (irq_polled)
    {
        if (soc_ipoll_connect(unit, soc_cmicx_intr, INT_TO_PTR(unit)) < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "error initializing polled interrupt mode");
        }
        SOC_CONTROL(unit)->soc_flags |= SOC_F_POLLED;
#ifdef SEPARATE_PKTDMA_INTR_HANDLER
        if (soc_ipoll_pktdma_connect(unit, soc_cmicx_pktdma_intr, INT_TO_PTR(unit)) < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "error initializing polled interrupt mode");
        }
#endif
    }
    else
    {
#ifdef SEPARATE_PKTDMA_INTR_HANDLER
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error separate pktdma thread cann't be created under interrupt mode");
#endif
        if (soc_cm_interrupt_connect(unit, soc_cmicx_intr, INT_TO_PTR(unit)) < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "could not connect interrupt line");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - deinit interrupt at soc layer, remove interrupt main entry
 *
 * \param [in] unit - unit #
 * \return
 * \see
 *   * None
 */

int
soc_dnxc_intr_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Disable all interrupts
     */
    soc_cmic_intr_all_disable(unit);

    /*
     * Shutdown polled interrupt mode if active 
     */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED)
    {
        if (soc_ipoll_disconnect(unit) < 0)
        {
            LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "could not disconnect interrupt polling\n")));
        }
#ifdef SEPARATE_PKTDMA_INTR_HANDLER
        if (soc_ipoll_pktdma_disconnect(unit) < 0)
        {
            LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "could not disconnect Packet DMA interrupt polling\n")));
        }
#endif
        SOC_CONTROL(unit)->soc_flags &= ~SOC_F_POLLED;
    }
    else
    {
        if (soc_cm_interrupt_disconnect(unit) < 0)
        {
            LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "could not disconnect interrupt line\n")));
        }
    }

    /*
     * Disable shutting down unit to run new DPCs and wait until all existing DPCs are completed. 
     */
    sal_dpc_disable_and_wait(INT_TO_PTR(unit));

    SHR_FUNC_EXIT;
}
/**
 * \brief - entry for interrupt handler for SER and interrupt at chip inside
 *
 * \param [in] unit_vp - unit #
 * \param [in] d1 - arg1 #
 * \param [in] d2 - arg2 #
 * \param [in] d3 - arg3 #
 * \param [in] d4 - arg4 #
 * \return
 * \see
 *   * None
 */
STATIC void
soc_dnxc_ser_intr_handler(
    void *unit_vp,
    void *d1,
    void *d2,
    void *d3,
    void *d4)
{
    int rc = SOC_E_NONE, i;
    int unit = PTR_TO_INT(unit_vp);
    int flags = 0;
    int max_interrupts_size = INTR_ERROR_MAX_INTERRUPTS_SIZE;
    soc_interrupt_cause_t interrupts[INTR_ERROR_MAX_INTERRUPTS_SIZE];
    int total_interrupts = 0;
    int interrupt_num = INTR_ERROR_MAX_INTERRUPTS_SIZE;
#ifdef PLISIM
    soc_reg_t ser_status_reg[4] = { ICFG_CHIP_LP_INTR_RAW_STATUS_REG0r,
        ICFG_CHIP_LP_INTR_RAW_STATUS_REG1r,
        ICFG_CHIP_LP_INTR_RAW_STATUS_REG2r,
        ICFG_CHIP_LP_INTR_RAW_STATUS_REG3r
    };
#endif

    sal_memset(interrupts, 0x0, INTR_ERROR_MAX_INTERRUPTS_SIZE * sizeof(soc_interrupt_cause_t));

    /*
     * Get all current Active interrupts 
     */
    flags = SOC_ACTIVE_INTERRUPTS_GET_UNMASKED_ONLY;
    rc = soc_active_interrupts_get(unit, flags, max_interrupts_size, interrupts, &total_interrupts);
    if (SOC_FAILURE(rc))
    {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s\n"), soc_errmsg(rc)));
        return;
    }

    LOG_VERBOSE(BSL_LS_SOC_INTR,
                (BSL_META_U(unit,
                            "interrupt_num=%d, max_interrupts_size=%d, total_interrupts=%d\n"),
                 interrupt_num, max_interrupts_size, total_interrupts));

    if (interrupt_num > total_interrupts)
    {
        interrupt_num = total_interrupts;
    }

    /*
     * sort interrupts according to priority 
     */
    if (interrupt_num > 1)
    {
        rc = soc_sort_interrupts_according_to_priority(unit, interrupts, interrupt_num);
        if (SOC_FAILURE(rc))
        {
            LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s\n"), soc_errmsg(rc)));
            return;
        }
    }

    /*
     * Call CB for every Active interrupt 
     */
    for (i = 0; i < interrupt_num; i++)
    {
        uint32 interrupt_flags;
        /*
         * we need to call bcm cb (dnxc_intr_switch_event_cb) and also to user cb only if applicable 
         */
        dnxc_intr_switch_event_cb(unit, SOC_SWITCH_EVENT_DEVICE_INTERRUPT, interrupts[i].id, interrupts[i].index, 0, 0);
        rc = soc_interrupt_flags_get(unit, interrupts[i].id, &interrupt_flags);
        if (SOC_FAILURE(rc))
        {
            LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s\n"), soc_errmsg(rc)));
            return;
        }
        if (((interrupt_flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB) == 0)
            && (SHR_BITGET(&interrupt_flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE) == 0))
        {
            continue;
        }
        soc_event_generate(unit, SOC_SWITCH_EVENT_DEVICE_INTERRUPT, interrupts[i].id, interrupts[i].index, 0);
    }

    /*
     * Enable interrups 
     */
#ifdef PLISIM
    if (SAL_BOOT_PLISIM)
    {
        uint32 mask, irqMask;
        uint32 addr;
        int s;

        mask = 0x01 << (CHIP_INTR_LOW_PRIORITY % (sizeof(uint32) * 8));
        addr = soc_reg_addr(unit, INTC_INTR_ENABLE_REG3r, REG_PORT_ANY, 0);
        s = sal_splhi();
        irqMask = soc_pci_read(unit, addr);
        irqMask |= mask;
        soc_pci_write(unit, addr, irqMask);
        sal_spl(s);
    }
#endif
    soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);

#ifdef PLISIM
    /*
     * Turn off Interrupts in PCID - to avoid endless loop 
     */
    for (i = 0; i < 4; i++)
    {
        (void) soc_pci_write(unit, soc_reg_addr(unit, ser_status_reg[i], REG_PORT_ANY, 0), 0);
    }
#endif

}

/**
 * \brief - interrupt handler for SER and interrupt at chip inside
 *
 * \param [in] unit - unit #
 * \param [in] data - unused #
 * \return
 * \see
 *   * None
 */
void
soc_dnxc_intr_handler(
    int unit,
    void *data)
{
    if (sal_dpc(soc_dnxc_ser_intr_handler, INT_TO_PTR(unit), 0, 0, 0, 0) < 0)
    {
        /*
         * dpc full unmask all interrupt at top level 
         */
#ifdef PLISIM
        if (SAL_BOOT_PLISIM)
        {
            uint32 mask, irqMask;
            uint32 addr;
            int s;

            mask = 0x01 << (CHIP_INTR_LOW_PRIORITY % (sizeof(uint32) * 8));
            addr = soc_reg_addr(unit, INTC_INTR_ENABLE_REG3r, REG_PORT_ANY, 0);
            s = sal_splhi();
            irqMask = soc_pci_read(unit, addr);
            irqMask &= ~mask;
            soc_pci_write(unit, addr, irqMask);
            sal_spl(s);
        }
#endif
        soc_cmic_intr_disable(unit, CHIP_INTR_LOW_PRIORITY);
    }
}

#undef BSL_LOG_MODULE

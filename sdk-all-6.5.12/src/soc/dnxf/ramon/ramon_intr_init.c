
/*
 * $Id: ramon_intr_init.c, v1 16/06/2014 09:55:39 azarrin $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Purpose:    Implement soc interrupt handler.
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>

#include <soc/intr.h>
#include <soc/ipoll.h>
#include <soc/iproc.h>

#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

#include <soc/dnxf/ramon/ramon_intr_cb_func.h>
#include <soc/dnxf/ramon/ramon_intr.h>
#include <soc/dnxf/ramon/ramon_defs.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>

#include <soc/dnxc/legacy/dnxc_intr_handler.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/dnxc/legacy/dnxc_dev_feature_manager.h>
#include <soc/drv.h>
#include <soc/intr_cmicx.h>
#include <soc/dnxc/intr.h>
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
static 
soc_reg_t soc_ramon_interrupt_monitor_mem_reg[] = {
    BRDC_DCH_ECC_ERR_1B_MONITOR_MEM_MASKr,
    BRDC_DCH_ECC_ERR_2B_MONITOR_MEM_MASKr,
    BRDC_CCH_ECC_ERR_1B_MONITOR_MEM_MASKr,
    BRDC_CCH_ECC_ERR_2B_MONITOR_MEM_MASKr,
    BRDC_DCML_ECC_ERR_1B_MONITOR_MEM_MASKr,
    BRDC_DCML_ECC_ERR_2B_MONITOR_MEM_MASKr,
    BRDC_FMAC_ECC_ERR_1B_MONITOR_MEM_MASKr,
    BRDC_FMAC_ECC_ERR_2B_MONITOR_MEM_MASKr,
    BRDC_FMAC_PAR_ERR_MEM_MASKr,
    BRDC_LCM_ECC_ERR_1B_MONITOR_MEM_MASKr,
    BRDC_LCM_ECC_ERR_2B_MONITOR_MEM_MASKr,
    BRDC_QRH_ECC_ERR_1B_MONITOR_MEM_MASKr,
    BRDC_QRH_ECC_ERR_2B_MONITOR_MEM_MASKr,

    CCH_ECC_ERR_1B_MONITOR_MEM_MASKr,
    CCH_ECC_ERR_2B_MONITOR_MEM_MASKr,
    DCH_ECC_ERR_1B_MONITOR_MEM_MASKr,
    DCH_ECC_ERR_2B_MONITOR_MEM_MASKr,
    DCML_ECC_ERR_1B_MONITOR_MEM_MASKr,
    DCML_ECC_ERR_2B_MONITOR_MEM_MASKr,
    ECI_ECC_ERR_1B_MONITOR_MEM_MASKr,
    ECI_ECC_ERR_2B_MONITOR_MEM_MASKr,
    FMAC_ECC_ERR_1B_MONITOR_MEM_MASKr,
    FMAC_ECC_ERR_2B_MONITOR_MEM_MASKr,
    FMAC_PAR_ERR_MEM_MASKr,
    /* no FSRD memory protected by SER */
    LCM_ECC_ERR_1B_MONITOR_MEM_MASKr,
    LCM_ECC_ERR_2B_MONITOR_MEM_MASKr,
    MCT_ECC_ERR_1B_MONITOR_MEM_MASKr,
    MCT_ECC_ERR_2B_MONITOR_MEM_MASKr,
    QRH_ECC_ERR_1B_MONITOR_MEM_MASKr,
    QRH_ECC_ERR_2B_MONITOR_MEM_MASKr,
    RTP_ECC_ERR_1B_MONITOR_MEM_MASKr,
    RTP_ECC_ERR_2B_MONITOR_MEM_MASKr,
    INVALIDr
};

/*************
 * FUNCTIONS *
 *************/
int 
soc_ramon_interrupt_all_enable_set(int unit, int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    /* Enable/Disable low priority interrupt line */
    if (enable) {
        soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
    } else {
        soc_cmic_intr_disable(unit, CHIP_INTR_LOW_PRIORITY);
    }
#ifdef PLISIM
    if (SAL_BOOT_PLISIM) {
        uint32 mask, irqMask;
        uint32 addr;
        int s;

        mask = 0x01 << (CHIP_INTR_LOW_PRIORITY % (sizeof(uint32)*8));
        addr = soc_reg_addr(unit, INTC_INTR_ENABLE_REG3r, REG_PORT_ANY, 0);
        s = sal_splhi();
        irqMask = soc_pci_read(unit, addr);
        if (enable) {
            irqMask |= mask;
        } else {
            irqMask &= ~mask;
        }
        soc_pci_write(unit, addr, irqMask);
        sal_spl(s);
    }
#endif
    SHR_FUNC_EXIT;
}

int
soc_ramon_interrupt_all_enable_get(int unit, int *enable)
{
    int mask;
    int rc;

    SHR_FUNC_INIT_VARS(unit);
    /* interrupt number is CHIP_INTR_LOW_PRIORITY=119, mapping to INTC_INTR_ENABLE_REG3r */
    rc = soc_interrupt_is_all_mask(unit, &mask);
    SHR_IF_ERR_EXIT(rc);
    *enable = mask ? 0 : 1;
exit:
    SHR_FUNC_EXIT;
}

int soc_ramon_ser_init(int unit)
{
    int idx, array_index, array_index_max;
    int rc;
    soc_reg_above_64_val_t above_64;
    int blk;
    int inst;
    soc_reg_t reg;
    soc_block_types_t block;
    SHR_FUNC_INIT_VARS(unit);

    /* unmask SER monitor registers*/
    SOC_REG_ABOVE_64_ALLONES(above_64);
    for(idx=0; soc_ramon_interrupt_monitor_mem_reg[idx] != INVALIDr; idx++) {
        reg = soc_ramon_interrupt_monitor_mem_reg[idx];
        array_index_max = 1;
        inst = 0;
        block = SOC_REG_INFO(unit, reg).block;
        SOC_BLOCKS_ITER(unit, blk, block) {
            if (SOC_REG_IS_ARRAY(unit, reg)){
                array_index_max = SOC_REG_NUMELS(unit, reg);
            }
            for (array_index = 0; array_index < array_index_max; array_index++) {
                rc = soc_reg_above_64_set(unit, reg, inst, array_index, above_64);
                SHR_IF_ERR_EXIT(rc);
            }
            inst++;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

int soc_ramon_interrupts_disable(int unit)
{
    int rc;
    int i, copy_no;
    soc_interrupt_db_t* interrupts;
    soc_block_types_t  block;
    soc_reg_t reg;
    int blk;
    int nof_interrupts;
    soc_reg_above_64_val_t data;

    SHR_FUNC_INIT_VARS(unit);

    rc = soc_ramon_nof_interrupts(unit, &nof_interrupts);
    SHR_IF_ERR_EXIT(rc);

    if (!SAL_BOOT_NO_INTERRUPTS) {

        /* disable all block interrupts */
        SOC_REG_ABOVE_64_CLEAR(data);
        for (i = 0; i < SOC_RAMON_NOF_BLK; i++) {
            if (SOC_CONTROL(unit)->interrupts_info->interrupt_tree_info[i].int_mask_reg != INVALIDr) {
                rc = soc_reg_above_64_set(unit, SOC_CONTROL(unit)->interrupts_info->interrupt_tree_info[i].int_mask_reg, 0,  0, data); 
                SHR_IF_ERR_EXIT(rc);
            }
        }
        /* disable all interrupt vectors */
        interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
        for (i=0 ; i < nof_interrupts; i++) { 
            reg = interrupts[i].reg;
            /* Unsupported interrupts */
            if (!SOC_REG_IS_VALID(unit, reg))
            {
               continue;
            }
            block = SOC_REG_INFO(unit, reg).block;
            SOC_REG_ABOVE_64_CLEAR(data);
            SOC_BLOCKS_ITER(unit, blk, block) {
                copy_no = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : SOC_BLOCK_NUMBER(unit, blk);
                if (interrupts[i].vector_info) {
                    rc = soc_reg_above_64_set(unit, interrupts[i].vector_info->int_mask_reg, copy_no,  0, data);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int soc_ramon_interrupts_deinit(int unit)
{
    int ii;
    soc_reg_t reg[4] = {ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                        ICFG_CHIP_LP_INTR_ENABLE_REG1r,
                        ICFG_CHIP_LP_INTR_ENABLE_REG2r,
                        ICFG_CHIP_LP_INTR_ENABLE_REG3r};

    SHR_FUNC_INIT_VARS(unit);
    /* 
     * disable interrupt
     */
    for (ii = 0; ii < 4; ii++) {
        (void)soc_cmic_or_iproc_setreg(unit, reg[ii], 0);
    }

    /* mask all interrupts in cmic (even in WB mode). This Masking update WB DB, althoght we dont use WB DB. */ 
    soc_ramon_interrupt_all_enable_set(unit, 0);

    if (!(dnxf_data_device.interrupts.intr_all_disable_get(unit)))
    {
        /* Shutdown polled interrupt mode if active */
        if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED)
        {
            if (soc_ipoll_disconnect(unit) < 0)
            {
                LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "error disconnecting polled interrupt mode\n")));
            }
            SOC_CONTROL(unit)->soc_flags &= ~SOC_F_POLLED;
        } else {
            if (soc_cm_interrupt_disconnect(unit) < 0)
            {
                LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "could not disconnect interrupt line\n")));
            }
        }
    }

    if (dnxc_intr_handler_deinit(unit) < 0) {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error at interrupt db deinitialization");
    }
           
exit:               
    SHR_FUNC_EXIT;
}

int soc_ramon_interrupts_init(int unit)
{
    soc_cmic_intr_handler_t handle;
    int ii;
    soc_reg_t reg[4] = {ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                        ICFG_CHIP_LP_INTR_ENABLE_REG1r,
                        ICFG_CHIP_LP_INTR_ENABLE_REG2r,
                        ICFG_CHIP_LP_INTR_ENABLE_REG3r};

    SHR_FUNC_INIT_VARS(unit);

    /* 
     * mask all interrupts in cmic (even in WB mode).
      */
    soc_ramon_interrupt_all_enable_set(unit, 0);
    if (!SAL_BOOT_NO_INTERRUPTS)
    {
        /* 
         * Init interrupt DB
         */
        if (dnxc_intr_handler_init(unit) < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "error initializing polled interrupt mode");
        }
    }
    handle.num = CHIP_INTR_LOW_PRIORITY;
    handle.intr_fn = soc_dnxc_intr_handler;
    handle.intr_data = NULL;
    soc_cmic_intr_register(unit, &handle, 1);
    /* 
     * enalbe interrupt
     */
    for (ii = 0; ii < 4; ii++) 
    {
        (void)soc_cmic_or_iproc_setreg(unit, reg[ii], 0xFFFFFFFF);
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE

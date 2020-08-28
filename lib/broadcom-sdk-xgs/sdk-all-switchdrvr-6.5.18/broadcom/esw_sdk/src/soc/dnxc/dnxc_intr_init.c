
/*
 * $Id: soc_dnxc_intr_handler.c, v1 16/06/2014 09:55:39 azarrin $
 *
 * $Copyright: (c) 2019 Broadcom.
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
#include <soc/intr_cmicx.h>
#include <soc/dnxc/intr.h>

#include <soc/dnxc/dnxc_intr_handler.h>
#include <soc/dnxc/error.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <shared/shrextend/shrextend_debug.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <soc/dnx/field/tcam_access/tcam_access.h>
#include <soc/dnx/intr/dnx_intr.h>
#endif
#include <soc/dnxc/dnxc_ha.h>

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_intr.h>
#include <soc/dnxf/cmn/dnxf_intr.h>
#endif
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

/*************
 * FUNCTIONS *
 *************/

int
soc_dnxc_set_mem_mask(
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    int all_one)
{
    int rc;
    int inst = 0;
    int blk;
    soc_reg_above_64_val_t regVal;
    soc_reg_above_64_val_t fieldVal;
    int array_index_min = 0;
    int array_index_max = 1;
    int array_index;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * unmask SER monitor registers
     */
    if (all_one)
    {
        SOC_REG_ABOVE_64_ALLONES(fieldVal);
        SOC_REG_ABOVE_64_ALLONES(regVal);
    }
    else
    {
        SOC_REG_ABOVE_64_CLEAR(fieldVal);
        SOC_REG_ABOVE_64_CLEAR(regVal);
    }

    SOC_BLOCK_ITER_ALL(unit, blk, SOC_REG_FIRST_BLK_TYPE(SOC_REG_INFO(unit, reg).block))
    {
        if (SOC_INFO(unit).block_valid[blk])
        {
            if (SOC_REG_IS_ARRAY(unit, reg))
            {
                array_index_max = SOC_REG_NUMELS(unit, reg) + SOC_REG_FIRST_ARRAY_INDEX(unit, reg);
                array_index_min = SOC_REG_FIRST_ARRAY_INDEX(unit, reg);
            }

            for (array_index = array_index_min; array_index < array_index_max; array_index++)
            {
                if (field != INVALIDf)
                {
                    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, inst, array_index, regVal));
                    soc_reg_above_64_field_set(unit, reg, regVal, field, fieldVal);
                }
                rc = soc_reg_above_64_set(unit, reg, inst, array_index, regVal);
                SHR_IF_ERR_EXIT(rc);
            }
        }
        inst++;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
* Function:
*      soc_dnxc_ser_init
* Purpose:
*      List all mem_mask reigster, unmask all memory for SER
* Parameters:
*      unit                    - (IN)  Device number
*
* Returns:
*      _SHR_E_XXX
*/
int
soc_dnxc_ser_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(soc_dnx_ser_init(unit));
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        SHR_IF_ERR_EXIT(soc_dnxf_ser_init(unit));
    }
#endif
exit:
    SHR_FUNC_EXIT;
}

/*
* Function:
*      soc_dnxc_interrupts_disable
* Purpose:
*      Disable all device interrupts
* Parameters:
*      unit                    - (IN)  Device number
*
* Returns:
*      _SHR_E_XXX
*/
int
soc_dnxc_interrupts_disable(
    int unit)
{
    int rc = _SHR_E_INTERNAL;
    int copy_no;
    soc_reg_t reg;
    int blk;
    soc_reg_above_64_val_t data;
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    soc_block_types_t block;
    soc_interrupt_db_t *interrupts;
    int i;
    int nof_interrupts = 0;
#endif

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        nof_interrupts = dnx_data_intr.general.nof_interrupts_get(unit);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        nof_interrupts = dnxf_data_intr.general.nof_interrupts_get(unit);
    }
#endif

    if (!SAL_BOOT_NO_INTERRUPTS)
    {

        /*
         * disable all block interrupts 
         */
        SOC_REG_ABOVE_64_CLEAR(data);
        for (blk = 0; ((SOC_BLOCK_TYPE(unit, blk) >= 0) && (SOC_BLOCK_TYPE(unit, blk) != SOC_BLK_EMPTY)); blk++)
        {
            /** skip disabled block */
            if (!SOC_INFO(unit).block_valid[blk])
            {
                continue;
            }
            reg = SOC_CONTROL(unit)->interrupts_info->interrupt_tree_info[SOC_BLOCK_INFO(unit, blk).cmic].int_mask_reg;
            if (SOC_REG_IS_VALID(unit, reg))
            {
                copy_no = SOC_BLOCK_NUMBER(unit, blk);
                rc = soc_reg_above_64_set(unit, reg, copy_no, 0, data);
                SHR_IF_ERR_EXIT(rc);
            }
        }
        /*
         * disable all interrupt vectors 
         */
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
        interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
        for (i = 0; i < nof_interrupts; i++)
        {
            reg = interrupts[i].reg;
            /*
             * Unsupported interrupts 
             */
            if (!SOC_REG_IS_VALID(unit, reg))
            {
                continue;
            }
            block = SOC_REG_INFO(unit, reg).block;
            SOC_REG_ABOVE_64_CLEAR(data);
            SOC_BLOCKS_ITER(unit, blk, block)
            {
                /** skip disabled block */
                if (!SOC_INFO(unit).block_valid[blk])
                {
                    continue;
                }
                copy_no = SOC_BLOCK_NUMBER(unit, blk);
                if (interrupts[i].vector_info)
                {
                    rc = soc_reg_above_64_set(unit, interrupts[i].vector_info->int_mask_reg, copy_no, 0, data);
                    SHR_IF_ERR_EXIT(rc);
                }
            }
        }
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

/*
* Function:
*      soc_dnxc_interrupts_deinit
* Purpose:
*      Deinit interrupts
* Parameters:
*      unit                    - (IN)  Device number
*
* Returns:
*      _SHR_E_XXX
*/
int
soc_dnxc_interrupts_deinit(
    int unit)
{
    int ii;
    soc_reg_t reg[4] = { ICFG_CHIP_LP_INTR_ENABLE_REG0r,
        ICFG_CHIP_LP_INTR_ENABLE_REG1r,
        ICFG_CHIP_LP_INTR_ENABLE_REG2r,
        ICFG_CHIP_LP_INTR_ENABLE_REG3r
    };
#ifdef BCM_DNX_SUPPORT
    soc_reg_above_64_val_t eci_irq_enable;
#endif

    SHR_FUNC_INIT_VARS(unit);

    /** Allow writing to registers and changing SW state for the interrupt deinit time */

    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));
    /*
     * disable interrupt
     */
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SOC_REG_ABOVE_64_CLEAR(eci_irq_enable);
        (void) soc_reg_above_64_set(unit, ECI_ECI_INTERRUPTS_MASKr, REG_PORT_ANY, 0, eci_irq_enable);
    }
#endif

    for (ii = 0; ii < 4; ii++)
    {
        (void) soc_cmic_or_iproc_setreg(unit, reg[ii], 0);
    }

    /*
     * mask all interrupts in cmic (even in WB mode). This Masking update WB DB, althoght we dont use WB DB. 
     */
    soc_cmic_intr_disable(unit, CHIP_INTR_LOW_PRIORITY);

    /** Return to warmboot normal mode */
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    if (dnxc_intr_handler_deinit(unit) < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error at interrupt db deinitialization");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
* Function:
*      soc_dnxc_interrupts_init
* Purpose:
*      Init interrupts
* Parameters:
*      unit                    - (IN)  Device number
*
* Returns:
*      _SHR_E_XXX
*/
int
soc_dnxc_interrupts_init(
    int unit)
{
    soc_cmic_intr_handler_t handle;
    int ii;
    soc_reg_t reg[4] = { ICFG_CHIP_LP_INTR_ENABLE_REG0r,
        ICFG_CHIP_LP_INTR_ENABLE_REG1r,
        ICFG_CHIP_LP_INTR_ENABLE_REG2r,
        ICFG_CHIP_LP_INTR_ENABLE_REG3r
    };
#ifdef BCM_DNX_SUPPORT
    soc_reg_above_64_val_t eci_irq_enable;
#endif

    SHR_FUNC_INIT_VARS(unit);

    /*
     * mask all interrupts in cmic (even in WB mode).
     */
    soc_cmic_intr_disable(unit, CHIP_INTR_LOW_PRIORITY);
    /*
     * Init interrupt DB
     */
    if (dnxc_intr_handler_init(unit) < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error initializing polled interrupt mode");
    }
    handle.num = CHIP_INTR_LOW_PRIORITY;
    handle.intr_fn = soc_dnxc_intr_handler;
    handle.intr_data = NULL;
    soc_cmic_intr_register(unit, &handle, 1);
    /*
     * enalbe interrupt
     */
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SOC_REG_ABOVE_64_ALLONES(eci_irq_enable);
        (void) soc_reg_above_64_set(unit, ECI_ECI_INTERRUPTS_MASKr, REG_PORT_ANY, 0, eci_irq_enable);
    }
#endif
    for (ii = 0; ii < 4; ii++)
    {
        (void) soc_cmic_or_iproc_setreg(unit, reg[ii], 0xFFFFFFFF);
    }

    if (!SOC_WARM_BOOT(unit))
    {
        /*
         *  Disable all interrupts vector
         */
        soc_dnxc_interrupts_disable(unit);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
* Function:
*      soc_dnxc_interrupt_all_enable_set
* Purpose:
*      Enable/Disable interrupts at top level
* Parameters:
*      unit                    - (IN)  Device number
*      enable                - (IN)  enable/disable
*
* Returns:
*      _SHR_E_XXX
*/
int
soc_dnxc_interrupt_all_enable_set(
    int unit,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Enable/Disable low priority interrupt line 
     */
    if (enable)
    {
        soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
    }
    else
    {
        soc_cmic_intr_disable(unit, CHIP_INTR_LOW_PRIORITY);
    }
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
        if (enable)
        {
            irqMask |= mask;
        }
        else
        {
            irqMask &= ~mask;
        }
        soc_pci_write(unit, addr, irqMask);
        sal_spl(s);
    }
#endif
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE

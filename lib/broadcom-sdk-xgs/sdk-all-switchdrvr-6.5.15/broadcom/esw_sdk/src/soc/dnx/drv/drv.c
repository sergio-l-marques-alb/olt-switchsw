/**
 * \file src/soc/dnx/drv/drv.c
 * Slim SoC module to allow bcm actions.
 * 
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 */

/*
 * $Copyright: (c) 2018 Broadcom.
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

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/access.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/sand/sand_mem.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/sand/sand_mbist.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_fifodma.h>
#include <soc/dnxc/drv.h>
#include <soc/mcm/driver.h>
#include <soc/linkctrl.h>
#include <bcm_int/dnx/init/init.h>
#include "include/soc/dnx/adapter/adapter_reg_access.h"
#include <soc/dnxc/legacy/dnxc_cmic.h>
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/iproc_m0ssq.h>
#include <soc/dnxc/legacy/dnxc_mem.h>
#include <soc/ipoll.h>
#include <soc/intr_cmicx.h>
#include <soc/dcb.h>
#include <soc/dnxc/dnxc_ha.h>

extern const dnx_init_step_t dnx_init_deinit_seq[];
/*
 * }
 */

shr_error_e
soc_dnx_chip_driver_find(
    int unit,
    uint16 pci_dev_id,
    uint8 pci_rev_id,
    soc_driver_t ** found_driver)
{
    uint16 driver_dev_id;
    uint8 driver_rev_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get handler to locate the device driver, 
     * the result will usually be the same as the input, 
     * however it will be modified in cases of running SKUs 
     */
    SHR_IF_ERR_EXIT(soc_cm_get_id_driver(pci_dev_id, pci_rev_id, &driver_dev_id, &driver_rev_id));

    if (SOC_IS_JERICHO_2_TYPE(driver_dev_id))
    {
        if (driver_rev_id == JERICHO_2_A0_REV_ID)
        {
            *found_driver = &soc_driver_bcm88690_a0;
        }
        else if (driver_rev_id == JERICHO_2_B0_REV_ID)
        {
            *found_driver = &soc_driver_bcm88690_b0;
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                     "a suitable driver was not found for specified revision_id %u\n%s%s",
                                     driver_rev_id, EMPTY, EMPTY);
        }
    }
    else if (SOC_IS_J2C_TYPE(driver_dev_id))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                 "a suitable driver was not found for specified revision_id %u\n%s%s",
                                 driver_rev_id, EMPTY, EMPTY);
    }
    else
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "a suitable driver was not found for specified "
                                 "device_id %u and revision_id %u\n%s", driver_dev_id, driver_rev_id, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_chip_type_set(
    int unit,
    uint16 dev_id)
{
    soc_info_t *si;

    SHR_FUNC_INIT_VARS(unit);

    si = &SOC_INFO(unit);

    /*
     * Used to implement the SOC_IS_*(unit) macros
     */
    if (SOC_IS_JERICHO_2_TYPE(dev_id))
    {
        si->chip_type = SOC_INFO_CHIP_TYPE_JERICHO_2;
        SOC_CHIP_STRING(unit) = "jericho_2";
    }
    else if (SOC_IS_J2C_TYPE(dev_id))
    {
        si->chip_type = SOC_INFO_CHIP_TYPE_J2C;
        SOC_CHIP_STRING(unit) = "jericho_2c";
    }
    else
    {
        si->chip_type = 0;
        SOC_CHIP_STRING(unit) = "???";
        LOG_VERBOSE_EX(BSL_LOG_MODULE, "soc_dnx_chip_type_set: driver device %04x unexpected\n%s%s%s",
                       dev_id, EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                 "soc_dnx_chip_type_set: cannot find a match for driver device\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_restore(
    int unit)
{
    int rv;
    uint16 device_id;
    uint8 revision_id;
    soc_control_t *soc;
    soc_info_t *si;
    SHR_FUNC_INIT_VARS(unit);
    soc = SOC_CONTROL(unit);

    sal_memset(soc, 0, sizeof(soc_control_t));

    /*
     * the MCM generates a driver from each supported register file 
     * this driver is linked here to the running device. SKUs will 
     * recieve the super set driver. 
     */
    rv = soc_cm_get_id(unit, &device_id, &revision_id);
    SHR_IF_ERR_EXIT(rv);
    rv = soc_dnx_chip_driver_find(unit, device_id, revision_id, &(soc->chip_driver));
    SHR_IF_ERR_EXIT_WITH_LOG(rv, "Couldn't find driver for unit %d (device 0x%04x, revision 0x%02x)\n",
                             unit, device_id, revision_id);

    rv = soc_dnx_chip_type_set(unit, device_id);
    SHR_IF_ERR_EXIT(rv);

    /** restore driver type and group */
    si = &SOC_INFO(unit);
    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];

    soc->soc_flags |= SOC_F_ATTACHED;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_attach(
    int unit)
{
    soc_control_t *soc;
    soc_persist_t *sop;
    int mem;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Allocate soc_control.
     */
    soc = SOC_CONTROL(unit);
    if (soc == NULL)
    {
        soc = sal_alloc(sizeof(soc_control_t), "soc_control");
        if (soc == NULL)
        {
            SHR_IF_ERR_EXIT(_SHR_E_MEMORY);
        }
        SOC_CONTROL(unit) = soc;
    }

    SHR_IF_ERR_EXIT(soc_dnx_restore(unit));

    /*
     * Abort and disable previous DMA operations 
     */
    SHR_IF_ERR_EXIT(soc_cmicx_dma_abort(unit, 0));

    /*
     * Init Feature list 
     */
    soc_feature_init(unit);

    /*
     * Initialize SCHAN 
     */
    SHR_IF_ERR_EXIT_WITH_LOG(soc_cmic_intr_init(unit), "failed to Initialize CMIC interrupt framework\n%s%s%s", EMPTY,
                             EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(soc_cmic_intr_all_disable(unit));

    soc_dnx_info_config(unit);

    SOC_PERSIST(unit) = sal_alloc(sizeof(soc_persist_t), "soc_persist");
    if (SOC_PERSIST(unit) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate soc_persist");
    }
    sal_memset(SOC_PERSIST(unit), 0, sizeof(soc_persist_t));
    sop = SOC_PERSIST(unit);
    sop->version = 1;

    /*
     * Initialize memory index_maxes. Chip specific overrides follow.
     */
    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (SOC_MEM_IS_VALID(unit, mem))
        {
            sop->memState[mem].index_max = SOC_MEM_INFO(unit, mem).index_max;
            /*
             * should only create mutexes for valid memories.
             */
            if ((soc->memState[mem].lock = sal_mutex_create(SOC_MEM_NAME(unit, mem))) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate memState lock");
            }

            /*
             * Set cache copy pointers to NULL
             */
            sal_memset(soc->memState[mem].cache, 0, sizeof(soc->memState[mem].cache));
        }
        else
        {
            sop->memState[mem].index_max = -1;
        }
    }

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) && !SAL_BOOT_PLISIM)
    {
        soc_iproc_m0ssq_reset(unit, 1);
    }
#endif

exit:
    if (SHR_FUNC_ERR())
    {
        LOG_ERROR_EX(BSL_LOG_MODULE, "soc_dnx_attach: unit %d failed\n%s%s%s", unit, EMPTY, EMPTY, EMPTY);
        soc_dnx_detach(unit);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_detach(
    int unit)
{
    soc_control_t *soc;

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_NUM_VALID(unit))
    {
        SHR_IF_ERR_EXIT(_SHR_E_UNIT);
    }

    if (SOC_PERSIST(unit) != NULL)
    {
        sal_free(SOC_PERSIST(unit));
    }
    SOC_PERSIST(unit) = NULL;

    soc = SOC_CONTROL(unit);
    if (soc == NULL)
    {
        SHR_EXIT();
    }

    sal_free(soc);
    SOC_CONTROL(unit) = NULL;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_ledup_init(
    int unit)
{
    uint32 reg_val;
    int freq;
    uint32 clk_half_period, refresh_period;
    SHR_FUNC_INIT_VARS(unit);

    freq = dnx_data_device.general.core_clock_khz_get(unit);
    /*
     * For LED refresh period = 33 ms (about 30Hz) 
     * refresh period = (required refresh period in sec)*(switch clock frequency in Hz) 
     */
    refresh_period = (freq * 33);
    reg_val = 0;
    soc_reg_field_set(unit, U0_LED_REFRESH_CTRLr, &reg_val, REFRESH_CYCLE_PERIODf, refresh_period);

    SHR_IF_ERR_EXIT(WRITE_U0_LED_REFRESH_CTRLr(unit, reg_val));
    /*
     * For LED clock period 
     * LEDCLK_HALF_PERIOD = [(required LED clock period in sec)/2]*(M0SS clock frequency in Hz)
     * Where M0SS freqency is 1000MHz and Typical LED clock period is 50ns(20MHz) = 50*10^-9 
     * value = Clk freq * (period/2) = 25
     */
    freq = 1000 * 1000000;
    clk_half_period = (freq + 39 * 1000000) / (20000000 * 2);
    reg_val = 0;
    soc_reg_field_set(unit, U0_LED_CLK_DIV_CTRLr, &reg_val, LEDCLK_HALF_PERIODf, clk_half_period);
    SHR_IF_ERR_EXIT(WRITE_U0_LED_CLK_DIV_CTRLr(unit, reg_val));
    /*
     * When PM shift out the led data, it shift out lane 7 first,followed by lane 6, 5, 4, 3, 2, 1, and 0.  The
     * LAST_PORT value to be the port number of sub-port 0. (88 for JR2) 
     */
    SHR_IF_ERR_EXIT(READ_U0_LED_ACCU_CTRLr(unit, &reg_val));
    soc_reg_field_set(unit, U0_LED_ACCU_CTRLr, &reg_val, LAST_PORTf, 88);
    SHR_IF_ERR_EXIT(WRITE_U0_LED_ACCU_CTRLr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_iproc_config(
    int unit)
{
    uint32 data;
    uint32 reg_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_ICFG_CHIP_ID_REGr(unit, &data));
    SHR_IF_ERR_EXIT(READ_PAXB_0_FUNC0_IMAP1_7r(unit, &reg_val));
    /*
     * This field is holding The Mapping address of SRAM into iProc. 
     */
    soc_reg_field_set(unit, PAXB_0_FUNC0_IMAP1_7r, &reg_val, ADDRf, 0xe80);
    /*
     * This field is making the entry valid 
     */
    soc_reg_field_set(unit, PAXB_0_FUNC0_IMAP1_7r, &reg_val, VALIDf, 0x1);
    SHR_IF_ERR_EXIT(WRITE_PAXB_0_FUNC0_IMAP1_7r(unit, 0xe8000001));

    SHR_IF_ERR_EXIT(soc_dnx_ledup_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_ring_config(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x34422227));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x55566333));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x55555555));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x55555555));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x55555555));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x55555555));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x55555555));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x00000000));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x22222000));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x00000002));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x00000004));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x00000000));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x00000000));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x00000000));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x00333333));
    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x00000655));

    SHR_IF_ERR_EXIT(soc_dnxc_cmic_sbus_timeout_set(unit, dnx_data_device.general.core_clock_khz_get(unit) /* KHz */ ,
                                                   SOC_CONTROL(unit)->schanTimeout));
    /*
     * Clear SCHAN_ERR 
     */
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH0_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH1_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH2_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH3_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH4_ERRr(unit, 0));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_schan_config_and_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Function for reading the configuration SOC properties 
     */
    soc_sand_access_conf_get(unit);
    /*
     * Initilizing the SCHAN 
     */
    SHR_IF_ERR_EXIT(soc_schan_init(unit));
exit:
    SHR_FUNC_EXIT;
}

/*
 * Perform a soft reset of the device.
 * The terminology used is according to the naming of the used ECI registers.
 * Using ASIC terminology, this is hard reset, the same operation performed by iproc's hard reset.
 * This operation sets registers to their default values.
 * All blocks will be placed in the soft reset and s-bus reset state and taken out of it.
 * Other needed actions are performed.
 */

shr_error_e
soc_dnx_soft_reset(
    int unit)
{
    uint32 reg_val;
    soc_reg_above_64_val_t reg_above_64_val;
    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
    /*
     * Soft Reset IN
     */
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
    /*
     * Sbus Reset IN
     */
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));
    /*
     * Misc Reset IN
     */
    reg_val = 7;
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
    sal_usleep(5000);
    /*
     * Misc Reset OUT
     */
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, DDR_DEC_RSTNf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, DDR_ENC_RSTNf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, 0x18));
    sal_usleep(40000);
    /*
     * SBUS Reset OUT
     */
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));
    sal_usleep(40000);
    /*
     * Soft Reset OUT
     */
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
    sal_usleep(40000);

    /*
     * Init needed for EPNI to CMIC CPU port connection
     */
    SHR_IF_ERR_EXIT(READ_ECI_PIR_CONFIGURATIONSr(unit, &reg_val));
    soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, PIR_RSTNf, 0);
    SHR_IF_ERR_EXIT(WRITE_ECI_PIR_CONFIGURATIONSr(unit, reg_val));
    soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, PIR_RSTNf, 1);
    sal_usleep(200);
    SHR_IF_ERR_EXIT(WRITE_ECI_PIR_CONFIGURATIONSr(unit, reg_val));
    soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_0_RXI_RESET_Nf, 1);
    soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_0_ENABLEf, 1);
    soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_1_RXI_RESET_Nf, 1);
    soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_1_ENABLEf, 1);
    soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, TXI_CREDITS_INIT_VALUEf, 0);
    soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, TXI_CREDITS_INITf, 0);
    sal_usleep(200);
    SHR_IF_ERR_EXIT(WRITE_ECI_PIR_CONFIGURATIONSr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_pll_configuration(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_sbus_polled_interrupts(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}
/*
 * Disable memory masking and add bubbles for memory access.
 * Needs to be run after WB, because of a internal registers updates.
 */
shr_error_e
soc_dnx_unmask_mem_writes(
    int unit)
{
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 i;

    CONST STATIC soc_reg_t indirect_wr_mask_reg[] = {
        DDHA_INDIRECT_WR_MASKr,
        DDHB_INDIRECT_WR_MASKr,
        DHC_INDIRECT_WR_MASKr,
        ERPP_INDIRECT_WR_MASKr,
        IPPA_INDIRECT_WR_MASKr,
        IPPB_INDIRECT_WR_MASKr,
        IPPC_INDIRECT_WR_MASKr,
        IPPD_INDIRECT_WR_MASKr,
        IPPE_INDIRECT_WR_MASKr,
        IPPF_INDIRECT_WR_MASKr,
        KAPS_INDIRECT_WR_MASKr,
        MDB_INDIRECT_WR_MASKr,
        SCH_INDIRECT_WR_MASKr,
        TCAM_INDIRECT_WR_MASKr
    };

    SHR_FUNC_INIT_VARS(unit);
    SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);

    /*
     * Temporarily allow HW writes for current thread even if it's generally disabled.
     */
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_SCHAN));
    /*
     * Disable memory masking 
     */
    for (i = 0; i < sizeof(indirect_wr_mask_reg) / sizeof(indirect_wr_mask_reg[0]); i++)
    {
        if (SOC_REG_IS_VALID(unit, indirect_wr_mask_reg[i]))
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, indirect_wr_mask_reg[i], REG_PORT_ANY, 0, reg_above_64_val));
        }
    }

    /*
     * Fill the Bubble registers in order to have access to all memories in their blocks
     */
    if (SOC_REG_IS_VALID(unit, CDUM_INDIRECT_FORCE_BUBBLEr))
    {
        SHR_IF_ERR_EXIT(WRITE_CDUM_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    }
    SHR_IF_ERR_EXIT(WRITE_CDU_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_CFC_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_CGM_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_CRPS_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_DDHA_INDIRECT_FORCE_BUBBLEr(unit, REG_PORT_ANY, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_DDHB_INDIRECT_FORCE_BUBBLEr(unit, REG_PORT_ANY, 0x020019));
    if (SOC_REG_IS_VALID(unit, DHC_INDIRECT_FORCE_BUBBLEr))
    {
        SHR_IF_ERR_EXIT(WRITE_DHC_INDIRECT_FORCE_BUBBLEr(unit, REG_PORT_ANY, 0x020019));
    }
    SHR_IF_ERR_EXIT(WRITE_DQM_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_ECGM_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_ECI_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_EDB_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_EPNI_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_EPS_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_ERPP_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_ETPPA_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_ETPPB_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_ETPPC_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_EVNT_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_FCR_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_FCT_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    if (SOC_REG_IS_VALID(unit, FDR_INDIRECT_FORCE_BUBBLEr))
    {
        SHR_IF_ERR_EXIT(WRITE_FDR_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    }
    SHR_IF_ERR_EXIT(WRITE_FDT_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_FQP_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_FSRD_INDIRECT_FORCE_BUBBLEr(unit, REG_PORT_ANY, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_HBMC_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_ILE_INDIRECT_FORCE_BUBBLEr(unit, REG_PORT_ANY, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_IPPA_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_IPPB_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_IPPC_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_IPPD_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_IPPE_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_IPPF_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_IPS_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_IPT_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_IQM_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_IRE_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_ITPPD_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_ITPP_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_KAPS_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_MACT_INDIRECT_FORCE_BUBBLEr(unit, REG_PORT_ANY, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_MCP_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_MDB_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_MRPS_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_MTM_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_OAMP_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_OCB_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_OLP_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_PEM_INDIRECT_FORCE_BUBBLEr(unit, REG_PORT_ANY, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_PQP_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_RQP_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_RTP_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_SCH_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_SPB_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_SQM_INDIRECT_FORCE_BUBBLEr(unit, 0x020019));
    SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_FORCE_BUBBLEr(unit, SOC_CORE_ALL, 0x020019));

    /*
     * revert dnxc_allow_hw_write_enable.
     */
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_SCHAN));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Perform a soft init of the device.
 * The terminology used is according to the naming of the used ECI registers.
 * Using ASIC terminology, this is a soft reset.
 * This operation does not set registers to their default values.
 * The operation resets the state of the blocks, and performs block specific operations.
 * All blocks will be placed in soft init state and taken out of it.
 * Other needed actions are performed.
 * At the end of the function, very basic device operational testing is performed.
 */

shr_error_e
soc_dnx_soft_init(
    int unit)
{
    uint32 reg_val, field_val;
    uint16 dev_id;
    uint8 rev_id;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_above_64_val_t field_above_64_val = { 0 };
    uint32 mem_data;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Pre Reset SCH */
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    field_above_64_val[0] = 0xffffffff;
    soc_reg_above_64_field_set(unit, SCH_DVS_NIF_CONFIGr, reg_above_64_val, NIF_FORCE_PAUSE_Nf, field_above_64_val);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, 1, reg_above_64_val));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    field_above_64_val[0] = 1;
    soc_reg_above_64_field_set(unit, SCH_DVS_CONFIGr, reg_above_64_val, FORCE_PAUSEf, field_above_64_val);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));

    /** Enter soft init */
    SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
    sal_usleep(2000);

    {
        /*
         * Init FQP TXI
         */
        dbal_tables_e dbal_table_id;
        uint32 entry_handle_id;
        bcm_core_t core;
        int egq_if, nof_egq_if;

        dbal_table_id = DBAL_TABLE_EGQ_INTERFACE_REG_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        nof_egq_if = dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit);
        /*
         * key construction. Constant part.
         */
        core = DBAL_CORE_ALL;
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        for (egq_if = 0; egq_if < nof_egq_if; egq_if++)
        {
            /*
             * key construction. Variable part
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egq_if);
            /*
             * Set INIT_FQP_TXIf to 'all 1'
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INIT_FQP_TXI_BIT, INST_SINGLE, (uint32) 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            /*
             * Set INIT_FQP_TXIf to 'all 0'
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INIT_FQP_TXI_BIT, INST_SINGLE, (uint32) 0);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
    /** Exit soft init */
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
    sal_usleep(5000);
    /** soft init scheduler - initialize dynamic memories */
    mem_data = 0;
    SHR_IF_ERR_EXIT(soc_mem_write(unit, SCH_SCHEDULER_INITm, MEM_BLOCK_ALL, 0, &mem_data));
    sal_usleep(1000);

    /** Post Reset SCH */
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    soc_reg_above_64_field_set(unit, SCH_DVS_NIF_CONFIGr, reg_above_64_val, NIF_FORCE_PAUSE_Nf, field_above_64_val);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, 1, reg_above_64_val));
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    soc_reg_above_64_field_set(unit, SCH_DVS_CONFIGr, reg_above_64_val, FORCE_PAUSEf, field_above_64_val);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));

    /** test s-bus access by reading some registers */
    soc_cm_get_id(unit, &dev_id, &rev_id);
    SHR_IF_ERR_EXIT(READ_ECI_VERSION_REGISTERr(unit, reg_above_64_val));
    field_val = soc_reg_above_64_field32_get(unit, ECI_VERSION_REGISTERr, reg_above_64_val, CHIP_TYPEf);
    if ((field_val & 0xffff) != dev_id)
    {
        LOG_ERROR_EX(BSL_LOG_MODULE, "Chip version is wrong: unit %d expected 0x%x and found 0x%x\n%s", unit,
                     dev_id, field_val, EMPTY);
    }
    SHR_IF_ERR_EXIT(soc_dnxc_verify_device_init(unit));
    SHR_IF_ERR_EXIT(READ_SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 0, &reg_val));
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, IRE_STATIC_CONFIGURATIONr, 0, 0, &reg_val));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_perform_bist_tests(
    int unit)
{
    int any_bist_performed = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sand_init_cpu2tap(unit));   /* always init CPU2TAP, perform tests if needed */

    if (!SOC_WARM_BOOT(unit))
    {
        if (dnx_data_device.general.bist_enable_get(unit) && !SAL_BOOT_PLISIM)
        {
            any_bist_performed = 1;
            SHR_IF_ERR_EXIT(soc_bist_all_jr2(unit, dnx_data_device.general.bist_enable_get(unit)));
        }
        
        if (any_bist_performed)
        {
            int step_found, i;
            utilex_seq_step_t *utilex_list;
            int deinit_steps[] = { DNX_INIT_STEP_DMA, DNX_INIT_STEP_SBUS, DNX_INIT_STEP_INTERRUPT, DNX_INIT_STEP_PLL };
            int init_steps[] =
                { DNX_INIT_STEP_HARD_RESET, DNX_INIT_STEP_SBUS, DNX_INIT_STEP_INTERRUPT, DNX_INIT_STEP_SOFT_RESET,
                DNX_INIT_STEP_DMA, DNX_INIT_STEP_PLL,
                DNX_INIT_STEP_SBUS_INTERRUPTS
            };
            SHR_IF_ERR_EXIT(dnx_init_step_list_convert(unit, dnx_init_deinit_seq, &utilex_list));

            /*
             * De-init of the steps
             */
            for (i = 0; i < 4; i++)
            {
                SHR_IF_ERR_EXIT(utilex_seq_run_step_by_id(unit, utilex_list, deinit_steps[i], 0, &step_found));
                if (step_found != 1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid step \n");
                }
            }

            /*
             * Init of the needed steps
             */
            for (i = 0; i < 7; i++)
            {
                SHR_IF_ERR_EXIT(utilex_seq_run_step_by_id(unit, utilex_list, init_steps[i], 1, &step_found));
                if (step_found != 1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid step \n");
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_endian_config(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SOC_CMCS_NUM(unit) = 2;
    SHR_IF_ERR_EXIT(soc_sbusdma_reg_init(unit));
    soc_endian_config(unit);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_polling_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_hard_reset(
    int unit)
{
    int disable_hard_reset = 0x0;
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);

    disable_hard_reset = dnx_data_device.general.feature_get(unit, dnx_data_device_general_hard_reset_disable);

    if (disable_hard_reset == 0)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_cmicx_device_hard_reset(unit, 4));

        if (SOC_IS_JERICHO_2_ONLY(unit))
        {       /* Limit to one outstanding AXI write for the Eventor */
            SHR_IF_ERR_EXIT(READ_AXIIC_EXT_S0_FN_MOD_BM_ISSr(unit, &reg_val));
            soc_reg_field_set(unit, AXIIC_EXT_S0_FN_MOD_BM_ISSr, &reg_val, WRITE_ISS_OVERRIDEf, 1);
            SHR_IF_ERR_EXIT(WRITE_AXIIC_EXT_S0_FN_MOD_BM_ISSr(unit, reg_val));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_dma_init(
    int unit)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize CMIC info
     */
    SOC_CMCS_NUM(unit) = 2;
    SOC_PCI_CMCS_NUM(unit) = soc_property_uc_get(unit, 0, spn_PCI_CMCS_NUM, 1);
    SOC_PCI_CMC(unit) = soc_property_uc_get(unit, 0, spn_PCI_CMC, 0);
    NUM_CPU_COSQ(unit) = 48;

    /*
     * Packet DMA descriptors Initilazation
     */
    soc_dcb_unit_init(unit);

    /*
     * reset is TRUE
     */
    SHR_IF_ERR_EXIT(soc_dma_attach(unit, TRUE));

#ifdef BCM_SBUSDMA_SUPPORT
    SOC_CONTROL(unit)->max_sbusdma_channels = SOC_DNXC_MAX_SBUSDMA_CHANNELS;
    SOC_CONTROL(unit)->tdma_ch = SOC_DNXC_TDMA_CHANNEL;
    SOC_CONTROL(unit)->tslam_ch = SOC_DNXC_TSLAM_CHANNEL;
    SOC_CONTROL(unit)->desc_ch = SOC_DNXC_DESC_CHANNEL;
    /*
     * maximum possible memory entry size used for clearing memory, should be a multiple of 32bit words
     */
    SOC_MEM_CLEAR_CHUNK_SIZE_SET(unit, soc_property_get(unit, spn_MEM_CLEAR_CHUNK_SIZE, SOC_DNXC_MEM_CLEAR_CHUNK_SIZE));
#endif

    /*
     * Abort and disable previous DMA operations 
     */

    if (soc_feature(unit, soc_feature_sbusdma))
    {
        /*
         * Setup DMA structures when a device is attached 
         */
        SHR_IF_ERR_EXIT(soc_sbusdma_lock_init(unit));   /* Initialize table/slam/sbus DMA locks */
        if (soc_mem_dmaable(unit, CGM_DESTINATION_TABLEm, SOC_MEM_BLOCK_ANY(unit, CGM_DESTINATION_TABLEm)) || soc_mem_slamable(unit, CGM_DESTINATION_TABLEm, SOC_MEM_BLOCK_ANY(unit, CGM_DESTINATION_TABLEm)))  /* check 
                                                                                                                                                                                                                 * if 
                                                                                                                                                                                                                 * DMA 
                                                                                                                                                                                                                 * is 
                                                                                                                                                                                                                 * enabled 
                                                                                                                                                                                                                 */
        {
            /*
             * check if DMA is enabled 
             */
            SHR_IF_ERR_EXIT(soc_sbusdma_init(unit, soc_property_get(unit, spn_DMA_DESC_TIMEOUT_USEC, 0),
                                             soc_property_get(unit, spn_DMA_DESC_INTR_ENABLE, 0)));

            SHR_IF_ERR_EXIT(sand_init_fill_table(unit));
        }
    }
    /*
     * Initialize FIFO DMA
     */
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_init(unit));

    /*
     * Init the uC area 
     */
    SHR_IF_ERR_EXIT(soc_cmicx_uc_msg_init(unit));

    if (!SOC_WARM_BOOT(unit))
    {
        /*
         * Zero EP_TO_CPU header size
         */
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ECI_RESERVED_22r, REG_PORT_ANY, 0, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_RESERVED_22r, reg_above_64_val, FIELD_30_33f, 0);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_RESERVED_22r, REG_PORT_ANY, 0, reg_above_64_val));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_dma_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * De-Initialize SBUS DMA
     */
    if (soc_feature(unit, soc_feature_sbusdma))
    {
        if (soc_mem_dmaable(unit, CGM_DESTINATION_TABLEm, SOC_MEM_BLOCK_ANY(unit, CGM_DESTINATION_TABLEm))
            || soc_mem_slamable(unit, CGM_DESTINATION_TABLEm, SOC_MEM_BLOCK_ANY(unit, CGM_DESTINATION_TABLEm)))
        {
            SHR_IF_ERR_EXIT(sand_deinit_fill_table(unit));
        }
        (void) soc_sbusdma_lock_deinit(unit);
    }

    /*
     * De-Initialize FIFO DMA
     */
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_deinit(unit));

    SHR_IF_ERR_EXIT(soc_dma_detach(unit));

    SHR_IF_ERR_EXIT(soc_sbusdma_desc_detach(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_mutexes_init(
    int unit)
{
    int cmc;
    soc_control_t *soc;
    SHR_FUNC_INIT_VARS(unit);

    soc = SOC_CONTROL(unit);
    SOC_PCI_CMCS_NUM(unit) = soc_property_uc_get(unit, 0, spn_PCI_CMCS_NUM, 1);

    /*
     * Create mutexes.
     */
    if ((soc->schan_wb_mutex = sal_mutex_create("SchanWB")) == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate SchanWB\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    if ((soc->socControlMutex = sal_mutex_create("SOC_CONTROL")) == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate soc_control lock\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    if ((soc->counterMutex = sal_mutex_create("Counter")) == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate counter Lock\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    if ((soc->miimMutex = sal_mutex_create("MIIM")) == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate MIIM lock\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    if ((soc->miimIntr = sal_sem_create("MIIM interrupt", sal_sem_BINARY, 0)) == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate MIIM interrup Sem\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    if ((soc->schanMutex = sal_mutex_create("SCHAN")) == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate Schan Lock\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_access_mem_mutex_init(unit), "Failed to allocate mutex Lock\n%s%s%s", EMPTY, EMPTY, EMPTY);    /* init 
                                                                                                                                 * memory 
                                                                                                                                 * mutexes 
                                                                                                                                 */

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++)
    {
        if ((soc->schanIntr[cmc] = sal_sem_create("SCHAN interrupt", sal_sem_BINARY, 0)) == NULL)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate Schan interrupt Sem\n%s%s%s", EMPTY, EMPTY,
                                     EMPTY);
        }
    }

#ifdef BCM_CMICM_SUPPORT
    soc->fschanMutex = NULL;
    if ((soc->fschanMutex = sal_mutex_create("FSCHAN")) == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate fSchan Lock\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
#endif /* BCM_CMICM_SUPPORT */

    /*
     * Create per Memory Mutex
     */
    SHR_IF_ERR_EXIT(dnx_access_mem_mutex_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_mutexes_deinit(
    int unit)
{
    soc_control_t *soc;
    int cmc;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_IF_ERR_EXIT(SOC_E_UNIT);
    }

    soc = SOC_CONTROL(unit);
    if (soc == NULL)
    {
        SHR_EXIT();
    }

    /*
     * Destroy per Memory Mutex
     */
    SHR_IF_ERR_EXIT(dnx_access_mem_mutex_deinit(unit));

    /*
     * Destroy Sem/Mutex 
     */
#ifdef BCM_CMICM_SUPPORT
    if (soc->fschanMutex != NULL)
    {
        sal_mutex_destroy(soc->fschanMutex);
        soc->fschanMutex = NULL;
    }
#endif /* BCM_CMICM_SUPPORT */

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++)
    {
        if (soc->schanIntr[cmc])
        {
            sal_sem_destroy(soc->schanIntr[cmc]);
            soc->schanIntr[cmc] = NULL;
        }
    }
    if (soc->schanMutex != NULL)
    {
        sal_mutex_destroy(soc->schanMutex);
        soc->schanMutex = NULL;
    }
    if (soc->miimIntr != NULL)
    {
        sal_sem_destroy(soc->miimIntr);
        soc->miimIntr = NULL;
    }

    if (soc->miimMutex != NULL)
    {
        sal_mutex_destroy(soc->miimMutex);
        soc->miimMutex = NULL;
    }

    if (soc->counterMutex != NULL)
    {
        sal_mutex_destroy(soc->counterMutex);
        soc->counterMutex = NULL;
    }

    if (soc->socControlMutex != NULL)
    {
        sal_mutex_destroy(soc->socControlMutex);
        soc->socControlMutex = NULL;
    }

    if (soc->schan_wb_mutex != NULL)
    {
        sal_mutex_destroy(soc->schan_wb_mutex);
        soc->schan_wb_mutex = NULL;
    }
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_access_mem_mutex_deinit(unit), "Failed to allocate mutex Lock\n%s%s%s", EMPTY, EMPTY, EMPTY);  /* init 
                                                                                                                                 * memory 
                                                                                                                                 * mutexes 
                                                                                                                                 */
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_mark_not_inited(
    int unit)
{
    soc_control_t *soc;
    SHR_FUNC_INIT_VARS(unit);

    soc = SOC_CONTROL(unit);
    soc->soc_flags &= ~SOC_F_INITED;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnx_info_config(
    int unit)
{
    soc_info_t *si;
    int mem, blk;

    SHR_FUNC_INIT_VARS(unit);

    si = &SOC_INFO(unit);
    si->num_ucs = 2;

    SHR_IF_ERR_EXIT(soc_sand_info_config_blocks(unit, 0));      /* configure blocks data structure */

    /*
     * Calculate the mem_block_any array for this configuration
     * The "any" block is just the first one enabled
     */
    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        si->mem_block_any[mem] = -1;
        if (SOC_MEM_IS_VALID(unit, mem))
        {
            SOC_MEM_BLOCK_ITER(unit, mem, blk)
            {
                si->mem_block_any[mem] = blk;
                break;
            }
        }
    }

    /*
     * use of gports is true for all SAND devices 
     */
    SOC_USE_GPORT_SET(unit, TRUE);

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnx_info_config_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Restore SoC Control to be identical to the SoC control Initlized by soc_dnx_attach()
     */
    SHR_IF_ERR_CONT(soc_dnx_restore(unit));

    /*
     * declare that driver is done detaching
     */
    SOC_DETACH(unit, 0);

    SHR_FUNC_EXIT;
}

int
soc_dnx_rcpu_port_init(
    int unit,
    int port_i)
{
    uint32 rval = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (port_i < 32)
    {
        SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_0r(unit, &rval));
        rval |= 0x1 << port_i;
        SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_0r(unit, rval));
    }
    else if (port_i < 64)
    {
        SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_1r(unit, &rval));
        rval |= 0x1 << (port_i - 32);
        SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_1r(unit, rval));
    }
    else if (port_i < 96)
    {
        SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_2r(unit, &rval));
        rval |= 0x1 << (port_i - 64);
        SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_2r(unit, rval));
    }
    else if (port_i < 128)
    {
        SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_3r(unit, &rval));
        rval |= 0x1 << (port_i - 96);
        SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_3r(unit, rval));
    }
    else if (port_i < 160)
    {
        SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_4r(unit, &rval));
        rval |= 0x1 << (port_i - 128);
        SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_4r(unit, rval));
    }
    else if (port_i < 192)
    {
        SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_5r(unit, &rval));
        rval |= 0x1 << (port_i - 160);
        SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_5r(unit, rval));
    }
    else if (port_i < 224)
    {
        SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_6r(unit, &rval));
        rval |= 0x1 << (port_i - 192);
        SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_6r(unit, rval));
    }
    else if (port_i < 256)
    {
        SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_7r(unit, &rval));
        rval |= 0x1 << (port_i - 224);
        SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_7r(unit, rval));
    }
    else
    {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Error: RCPU port range is 0 - 255\n")));
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_rcpu_init(
    int unit)
{
    int port_i = 0;
    const dnx_data_access_rcpu_rx_t *rcpu_rx;

    SHR_FUNC_INIT_VARS(unit);

    rcpu_rx = dnx_data_access.rcpu.rx_get(unit);

    SOC_PBMP_ITER(rcpu_rx->pbmp, port_i)
    {
        SHR_IF_ERR_EXIT(soc_dnx_rcpu_port_init(unit, port_i));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * Returns TRUE if the memory is dynamic
 */
int
dnx_tbl_is_dynamic(
    int unit,
    soc_mem_t mem)
{
    SOC_MEM_ALIAS_TO_ORIG(unit, mem);

    return soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem) || soc_mem_is_signal(unit, mem);
}

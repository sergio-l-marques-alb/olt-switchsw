/**
 * \file src/soc/dnx/drv/drv.c
 * Slim SoC module to allow bcm actions.
 *
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 */

/*
 * $Copyright: (c) 2020 Broadcom.
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
#include <soc/led.h>
#include <soc/mem.h>
#include <soc/sand/sand_mem.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/sand/sand_mbist.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/drv.h>
#include <soc/dnx/access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_fifodma.h>
#include <soc/dnxc/drv.h>
#include <soc/mcm/driver.h>
#include <soc/linkctrl.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/dram/buffers/buffers_quarantine_mechanism.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include "include/soc/dnx/adapter/adapter_reg_access.h"
#include <soc/dnxc/dnxc_cmic.h>
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/iproc_m0ssq.h>
#include <soc/dnxc/dnxc_mem.h>
#include <soc/ipoll.h>
#include <soc/intr_cmicx.h>
#include <soc/dcb.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <bcm_int/dnx/dram/dram.h>
#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/cmicx_link.h>
#define CMICX_HW_LINKSCAN 1
#endif
extern const dnx_init_step_t dnx_init_deinit_seq[];

dnx_startup_test_function_f dnx_startup_test_functions[SOC_MAX_NUM_DEVICES] = { 0 };

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
    uint16 driver_rev_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get handler to locate the device driver,
     * the result will usually be the same as the input,
     * however it will be modified in cases of running SKUs
     */
    SHR_IF_ERR_EXIT(soc_cm_get_id_driver(pci_dev_id, pci_rev_id, &driver_dev_id, &driver_rev_id));
    {
        if (SOC_IS_JERICHO2_TYPE(driver_dev_id))
        {
            if (driver_rev_id == JERICHO2_A0_REV_ID)
            {
                *found_driver = &soc_driver_bcm88690_a0;
            }
            else if ((driver_rev_id == JERICHO2_B0_REV_ID) || (driver_rev_id == JERICHO2_B1_REV_ID))
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
#if !defined(NO_BCM_88800_A0)
/* { */
        else if (SOC_IS_J2C_TYPE(driver_dev_id))
        {
            if ((driver_rev_id == J2C_A0_REV_ID) || (driver_rev_id == J2C_A1_REV_ID))
            {
                *found_driver = &soc_driver_bcm88800_a0;
            }
            else
            {
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                         "a suitable driver was not found for specified revision_id %u\n%s%s",
                                         driver_rev_id, EMPTY, EMPTY);
            }
        }
/* } */
#endif
        else if (SOC_IS_J2P_TYPE(driver_dev_id))
        {
            if (driver_rev_id == J2P_A0_REV_ID)
            {
                *found_driver = &soc_driver_bcm88850_a0;
            }
            else
            {
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                         "a suitable driver was not found for specified revision_id %u\n%s%s",
                                         driver_rev_id, EMPTY, EMPTY);
            }
        }
        else if (SOC_IS_Q2A_TYPE(driver_dev_id))
        {
            if (driver_rev_id == Q2A_A0_REV_ID)
            {
                *found_driver = &soc_driver_bcm88480_a0;
            }
            else if (driver_rev_id >= Q2A_B0_REV_ID)
            {
                *found_driver = &soc_driver_bcm88480_b0;
            }
            else
            {
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                         "a suitable driver was not found for specified revision_id %u\n%s%s",
                                         driver_rev_id, EMPTY, EMPTY);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "a suitable driver was not found for specified "
                                     "device_id %u and revision_id %u\n%s", driver_dev_id, driver_rev_id, EMPTY);
        }
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
    {
        /*
         * Used to implement the SOC_IS_*(unit) macros
         */
        if (SOC_IS_JERICHO2_TYPE(dev_id))
        {
            si->chip_type = SOC_INFO_CHIP_TYPE_JERICHO2;
            SOC_CHIP_STRING(unit) = "jericho2";
        }
        else if (SOC_IS_J2C_TYPE(dev_id))
        {
            si->chip_type = SOC_INFO_CHIP_TYPE_J2C;
            SOC_CHIP_STRING(unit) = "jericho2c";
        }
        else if (SOC_IS_J2P_TYPE(dev_id))
        {
            si->chip_type = SOC_INFO_CHIP_TYPE_J2P;
            SOC_CHIP_STRING(unit) = "jericho2p";
        }
        else if (SOC_IS_Q2A_TYPE(dev_id))
        {
            si->chip_type = SOC_INFO_CHIP_TYPE_Q2A;
            SOC_CHIP_STRING(unit) = "q2a";
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
    /*
     * initialize the device_name in soc control structure
     */
    if ((CMDEV(unit).dev.info->rev_id & SOC_CM_DEVICE_ANY_LAST_DEVID_DIGIT) != 0)
    {
        uint32 rev_id = CMDEV(unit).dev.info->rev_id & SOC_CM_REVID_MASK;
        uint8 last_revid_digit = (rev_id - 1) & 0xf;
        sal_snprintf(SOC_CONTROL(unit)->device_name, sizeof(SOC_CONTROL(unit)->device_name) - 1, "BCM8%.4X_%c%c",
                     (unsigned) CMDEV(unit).dev.dev_id, 'A' + (rev_id >> 4),
                     (last_revid_digit < 10 ? '0' + last_revid_digit : ('A' - 10) + last_revid_digit));
    }
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
#ifdef BCM_CMICX_SUPPORT
    static int default_fw_loaded[SOC_MAX_NUM_DEVICES] = { 0 };
#endif

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
     * Init Feature list
     */
    soc_feature_init(unit);

    /** Handle iproc hot swap events,abort and disable previous DMA operations */
    SHR_IF_ERR_EXIT(soc_cmicx_handle_hotswap_and_dma_abort(unit));

    /*
     * Initial LED driver.
     */
    soc_led_driver_init(unit);

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
        soc_cmicx_linkscan_pause(unit);
        soc_cmicx_led_enable(unit, 0);
        soc_iproc_m0ssq_reset(unit);

        if (default_fw_loaded[unit] == 0)
#if defined(SOC_PCI_DEBUG) && defined(INCLUDE_CPU_I2C)
            if (!SAL_BOOT_I2C_ACCESS)   /* do not load default linkscan FW when using I2C as this is slow */
#endif

            {
                SOC_IF_ERROR_RETURN(soc_cmicx_led_linkscan_default_fw_load(unit));
                default_fw_loaded[unit] = 1;
            }
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
    int spl;
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
    /** sync to bcmPOLL thread */
    spl = sal_splhi();
    sal_free(soc);
    SOC_CONTROL(unit) = NULL;
    sal_spl(spl);

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
    uint32 last_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    freq = dnx_data_device.general.core_clock_khz_get(unit);
    last_port = dnx_data_nif.global.last_port_led_scan_get(
    unit);
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
    soc_reg_field_set(unit, U0_LED_ACCU_CTRLr, &reg_val, LAST_PORTf, last_port);
    SHR_IF_ERR_EXIT(WRITE_U0_LED_ACCU_CTRLr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_iproc_config(
    int unit)
{
    uint32 reg_val;
    SHR_FUNC_INIT_VARS(unit);

    /** If needed, perform a system reset of the device, and prevent startup if it is not performed */
    SHR_IF_ERR_EXIT(soc_dnxc_perform_system_reset_if_needed(unit));

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
    SHR_IF_ERR_EXIT(dnxc_block_instance_validity_override(unit));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_ring_config(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_JERICHO2_ONLY(unit))
    {
        /*
         * SBUS ring map:
         * Ring2: BDM0(21), CDMAC0(1072), CDMAC1(1072), CDMAC10(1152), CDMAC11(1152), CDMAC2(1088), CDMAC3(1088),
         *        CDMAC4(1104), CDMAC5(1104), CDMAC6(1120), CDMAC7(1120), CDMAC8(1136), CDMAC9(1136), CDPORT0(1072),
         *        CDPORT1(1088), CDPORT2(1104), CDPORT3(1120), CDPORT4(1136), CDPORT5(1152),
         *        CDU0(53), CDU1(55), CDU2(57), CDU3(59), CDU4(61), CDUM0(51), CGM0(20), CRPS0(17),
         *        DDP0(65), DQM0(69), ECGM0(26), EDB0(31), EPNI0(24), EPRE0(25), EPS0(30), ERPP0(44),
         *        ETPPA0(45), ETPPB0(46), ETPPC0(47), FDR0(22), FQP0(29), ILE0(48), IPPA0(36), IPPB0(37),
         *        IPPC0(38), IPPD0(39), IPPE0(40), IPPF0(41), IPS0(18), IPT0(19), IQM0(68), IRE0(71), ITPP0(43),
         *        ITPPD0(42), MCP0(66), MRPS0(64), NMG0(49), PDM0(67), PEM0(32), PEM1(33), PEM2(34), PEM3(35),
         *        PQP0(28), RQP0(27), SCH0(63), SIF0(62), SPB0(16), SQM0(70)
         * Ring3: BDM1(140), CDMAC12(1792), CDMAC13(1792), CDMAC14(1808), CDMAC15(1808), CDMAC16(1824), CDMAC17(1824),
         *        CDMAC18(1840), CDMAC19(1840), CDMAC20(1856), CDMAC21(1856), CDMAC22(1872), CDMAC23(1872),
         *        CDPORT10(1856), CDPORT11(1872), CDPORT6(1792), CDPORT7(1808), CDPORT8(1824), CDPORT9(1840),
         *        CDU5(121), CDU6(122), CDU7(123), CDU8(124), CDU9(125), CDUM1(120), CGM1(139), CRPS1(136),
         *        DDP1(128), DQM1(132), ECGM1(145), EDB1(150), EPNI1(143), EPRE1(144), EPS1(149), ERPP1(163),
         *        ETPPA1(164), ETPPB1(165), ETPPC1(166), FDR1(141), FQP1(148), ILE1(118), IPPA1(155), IPPB1(156),
         *        IPPC1(157), IPPD1(158), IPPE1(159), IPPF1(160), IPS1(137), IPT1(138), IQM1(131), IRE1(134), ITPP1(162),
         *        ITPPD1(161), MCP1(129), MRPS1(127), NMG1(119), PDM1(130), PEM4(151), PEM5(152), PEM6(153), PEM7(154),
         *        PQP1(147), RQP1(146), SCH1(167), SIF1(126), SPB1(135), SQM1(133)
         * Ring4: DDHA0(82), DDHA1(83), DDHB0(84), DDHB1(85), DDHB2(86), DDHB3(87),
         *        DHC0(88), DHC1(89), DHC2(90), DHC3(91), DHC4(92), DHC5(93), DHC6(94), DHC7(95),
         *        EVNT(101), KAPS0(96), KAPS1(97), MACT(100), MDB(81), MDB_ARM(1280), OAMP(102), OLP(103),
         *        TCAM0(98), TCAM1(99)
         * Ring5: BRDC_FMAC(1920), BRDC_FSRD(1936), FCR(886), FCT(888), FDA(887), FDT(882), FDTL(883),
         *        FMAC0(208), FMAC1(224), FMAC10(448), FMAC11(464), FMAC12(496), FMAC13(512), FMAC14(544),
         *        FMAC15(560), FMAC16(592), FMAC17(608), FMAC18(640), FMAC19(656), FMAC2(256), FMAC20(688),
         *        FMAC21(704), FMAC22(736), FMAC23(752), FMAC24(784), FMAC25(800), FMAC26(832), FMAC27(848),
         *        FMAC3(272), FMAC4(304), FMAC5(320), FMAC6(352), FMAC7(368), FMAC8(400), FMAC9(416),
         *        FSRD0(240), FSRD1(288), FSRD10(720), FSRD11(768), FSRD12(816), FSRD13(864), FSRD2(336),
         *        FSRD3(384), FSRD4(432), FSRD5(480), FSRD6(528), FSRD7(576), FSRD8(624), FSRD9(672),
         *        MESH_TOPOLOGY(884), OCB0(880), OCB1(881), RTP(885)
         * Ring6: BRDC_HBC(1952), CFC0(199), CFC1(200), HBC0(180), HBC1(181), HBC10(190), HBC11(191), HBC12(192),
         *        HBC13(193), HBC14(194), HBC15(195), HBC2(182), HBC3(183), HBC4(184), HBC5(185), HBC6(186),
         *        HBC7(187), HBC8(188), HBC9(189), HBMC0(178), HBMC1(179), MTM(198), TDU0(176), TDU1(177)
         * Ring7: ECI(0)
         */
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x34422227));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x55566333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x00000066));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x22222000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x00000002));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x00000004));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x00333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x00000655));
    }
    else if (SOC_IS_J2C(unit))
    {
        /*
         * SBUS ring map:
         * Ring2: BDM(82), BRDC_FMAC(1920), BRDC_FSRD(1936), BRDC_HBC(1952), CDMAC0(1072), CDMAC1(1072),
         *        CDMAC2(1088), CDMAC3(1088), CDPORT0(1072), CDPORT1(1088), CDU0(59), CDU1(60), CFC(16),
         *        CGM(27), CLMAC16(1392), CLMAC17(1408), CLMAC18(1424), CLMAC19(1440), CLMAC20(1456),
         *        CLMAC21(1472), CLMAC22(1488), CLMAC23(1504), CLPORT16(1392), CLPORT17(1408),
         *        CLPORT18(1424), CLPORT19(1440), CLPORT20(1456), CLPORT21(1472), CLPORT22(1488),
         *        CLPORT23(1504), CLU4(57), CLU5(53), CLUP4(56), CLUP5(52), CRPS(85), DQM(30),
         *        EPNI(66), EPRE(65), ESB(88), ETPPA(90), ETPPC(68), FCR(78), FCT(80), FDA(79),
         *        FDR(74), FDT(75), FDTL(76), FMAC0(32), FMAC1(33), FMAC10(47), FMAC11(48), FMAC2(35),
         *        FMAC3(36), FMAC4(38), FMAC5(39), FMAC6(41), FMAC7(42), FMAC8(44), FMAC9(45),
         *        FSRD0(34), FSRD1(37), FSRD2(40), FSRD3(43), FSRD4(46), FSRD5(49),
         *        HBC0(19), HBC1(20), HBC2(21), HBC3(22), HBC4(23), HBC5(24), HBC6(25), HBC7(26), HBMC(18),
         *        ILE4(55), ILE5(51), ILE6(62), ILU4(54), ILU5(50), ILU6(61), IPS(28), IPT(72), IQM(29),
         *        IRE(67), ITPP(73), MCP(86), MESH_TOPOLOGY(81), MRPS(83), NMG(63), OCB(58), PDM(84),
         *        RTP(77), SCH(89), SIF(64), SQM(31), TDU(17)
         * Ring3: CDMAC4(1104), CDMAC5(1104), CDMAC6(1120), CDMAC7(1120), CDPORT2(1104), CDPORT3(1120),
         *        CDU2(135), CDU3(134), CLMAC0(1136), CLMAC1(1152), CLMAC10(1296), CLMAC11(1312), CLMAC12(1328),
         *        CLMAC13(1344), CLMAC14(1360), CLMAC15(1376), CLMAC2(1168), CLMAC3(1184), CLMAC4(1200),
         *        CLMAC5(1216), CLMAC6(1232), CLMAC7(1248), CLMAC8(1264), CLMAC9(1280),
         *        CLPORT0(1136), CLPORT1(1152), CLPORT10(1296), CLPORT11(1312), CLPORT12(1328), CLPORT13(1344),
         *        CLPORT14(1360), CLPORT15(1376), CLPORT2(1168), CLPORT3(1184), CLPORT4(1200), CLPORT5(1216),
         *        CLPORT6(1232), CLPORT7(1248), CLPORT8(1264), CLPORT9(1280),
         *        CLU0(132), CLU1(128), CLU2(124), CLU3(120), CLUP0(131), CLUP1(127), CLUP2(123), CLUP3(119),
         *        DDP(156), ECGM(141), EDB(133), EPS(139), ERPP(144), ETPPB(145), FQP(138),
         *        ILE0(130), ILE1(126), ILE2(122), ILE3(118), ILE7(137),
         *        ILU0(129), ILU1(125), ILU2(121), ILU3(117), ILU7(136),
         *        IPPA(164), IPPB(165), IPPC(147), IPPD(148), IPPE(162), IPPF(163), ITPPD(158), MTM(161),
         *        PEM0(152), PEM1(153), PEM2(154), PEM3(155), PQP(143), RQP(142), SPB(157)
         * Ring4: DDHA(195), DDHB0(202), DDHB1(203), DDHB2(204), DDHB3(205), EVNT(209), KAPS(207),
         *        MACT(210), MDB(192), MDB_ARM(1792), OAMP(208), OLP(211), TCAM(206)
         * Ring7: ECI(0)
         */
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x30222227));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x00440333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x00000002));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x33322000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x33333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x23333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x02222222));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x00000004));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x00000222));
    }
    else if (SOC_IS_Q2A(unit))
    {
        /*
         * SBUS ring map:
         * Ring2: CDMAC0(1072), CDMAC1(1088), CDPORT0(1072), CDPORT1(1088), CDU0(508), CDU1(504),
         *        CLMAC0(1136), CLMAC1(1152), CLMAC2(1168), CLMAC3(1184), CLMAC4(1200), CLMAC5(1216),
         *        CLMAC6(1232), CLMAC7(1248), CLMAC8(1264),
         *        CLPORT0(1136), CLPORT1(1152), CLPORT2(1168), CLPORT3(1184), CLPORT4(1200), CLPORT5(1216),
         *        CLPORT6(1232), CLPORT7(1248), CLPORT8(1264),
         *        CLU0(544), CLU1(548), CLU2(552), CLUP0(556), CLUP1(560), CLUP2(564),
         *        FASIC(540), FEU(512), FLEXEWP(528), FPRD(516), FSAR(536), FSCL(532), ILE0(568), ILE1(572),
         *        ILU0(576), ILU1(580), NMG(520), SIF(524)
         * Ring3: CFC(100), DDP(176), ECGM(108), EDB(113), EPNI(160), EPRE(164), EPS(124), ESB(168),
         *        ETPPA(132), ETPPB(152), ETPPC(156), FQP(128), IPT(190), IRE(172), ITPP(191), ITPPD(184),
         *        MTM(104), OCB(188), PEM0(136), PEM1(140), PEM2(144), PEM3(148), PQP(120), RQP(116), SPB(180)
         * Ring4: BDM(248), CGM(220), CRPS(256), DCC0(216), DCC1(217), DCC2(218), DCC3(219), DPC0(212), DPC1(213),
         *        DQM(228), ERPP(284), IPPA(276), IPPB(272), IPPC(264), IPPD(289), IPPE(280), IPPF(204), IPS(224),
         *        IQM(236), MCP(260), MRPS(252), PDM(240), SCH(200), SQM(232), TDU(208)
         * Ring5: DDHA(320), DDHB0(305), DDHB1(308), DDHB2(312), DDHB3(316), EVNT(332), KAPS(344), MACT(336),
         *        MDB(324), MDB_ARM(1792), OAMP(328), OLP(340), TCAM(304)
         * Ring7: ECI(0)
         */
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x33000007));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x44443333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x00555444));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x20000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x00022222));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x20022000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x22222222));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x00000005));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x00000000));
    }
    else if (SOC_IS_J2P(unit))
    {
        /*
         * SBUS ring map:
         * Ring2: BDM0(206), BRDC_FMAC0(1872), BRDC_FSRD0(1904), BRDC_HBC0(1936), CDB0(250), CDB1(251),
         *        CDB2(252), CDB3(253), CDB4(254), CDB5(255), CDB6(256), CDB7(257), CDBM0(258), CDMAC0(1520),
         *        CDMAC1(1536), CDMAC2(1552), CDMAC3(1568), CDMAC4(1584), CDMAC5(1600), CDMAC6(1616),
         *        CDMAC7(1632), CDMAC8(1648), CDPM0(241), CDPM1(242), CDPM2(243), CDPM3(244), CDPM4(245),
         *        CDPM5(246), CDPM6(247), CDPM7(248), CDPM8(249), CDPORT0(1520), CDPORT1(1536), CDPORT2(1552),
         *        CDPORT3(1568), CDPORT4(1584), CDPORT5(1600), CDPORT6(1616), CDPORT7(1632), CDPORT8(1648),
         *        CGM0(192), CRPS0(197), DDP0(200), DQM0(194), ECGM0(210), EDB0(212), EPNI0(217), EPRE0(216),
         *        EPS0(214), ERPP0(220), ESB0(218), ETPPA0(221), ETPPB0(222), ETPPC0(223), FDR0(209), FDTL0(208),
         *        FMAC0(960), FMAC1(961), FMAC10(975), FMAC11(976), FMAC12(978), FMAC13(979), FMAC14(981),
         *        FMAC15(982), FMAC16(984), FMAC17(985), FMAC18(987), FMAC19(988), FMAC2(963), FMAC20(990),
         *        FMAC21(991), FMAC22(993), FMAC23(994), FMAC3(964), FMAC4(966), FMAC5(967), FMAC6(969),
         *        FMAC7(970), FMAC8(972), FMAC9(973), FQP0(215),
         *        FSRD0(962), FSRD1(965), FSRD10(992), FSRD11(995), FSRD2(968), FSRD3(971), FSRD4(974),
         *        FSRD5(977), FSRD6(980), FSRD7(983), FSRD8(986), FSRD9(989),
         *        HBC0(270), HBC1(271), HBC2(272), HBC3(273), HBC4(274), HBC5(275), HBC6(276), HBC7(277),
         *        HBMC0(268), HRC0(269), ILE0(234), ILE1(235), IPPA0(226), IPPB0(227), IPPC0(228), IPPD0(229),
         *        IPPE0(224), IPPF0(225), IPS0(193), IPT0(203), IQM0(196), IRE0(199), ITPP0(204), ITPPD0(202),
         *        MACSEC0(1364), MACSEC1(1380), MACSEC2(1396), MACSEC3(1408), MACSEC4(1488), MCP0(198),
         *        MRPS0(207), MSD0(236), MSD1(237), MSD2(238), MSD3(239), MSS0(240), NMG0(259),
         *        OCB0(262), OCB1(263), OCB2(264), OCB3(265), OCBM0(261), PDM0(205),
         *        PEM0(230), PEM1(231), PEM2(232), PEM3(233), PQP0(213), RQP0(211), SCH0(219), SIF0(260),
         *        SPB0(201), SQM0(195), TDU0(266), TDU1(267)
         * Ring3: BDM1(398), BRDC_FMAC1(1888), BRDC_FSRD1(1920), BRDC_HBC1(1952),
         *        CDB10(444), CDB11(445), CDB12(446), CDB13(447), CDB14(448), CDB15(449), CDB8(442), CDB9(443),
         *        CDBM1(450), CDMAC10(1680), CDMAC11(1696), CDMAC12(1712), CDMAC13(1728), CDMAC14(1744),
         *        CDMAC15(1760), CDMAC16(1776), CDMAC17(1792), CDMAC9(1664), CDPM10(434), CDPM11(435), CDPM12(436),
         *        CDPM13(437), CDPM14(438), CDPM15(439), CDPM16(440), CDPM17(441), CDPM9(433),
         *        CDPORT10(1680), CDPORT11(1696), CDPORT12(1712), CDPORT13(1728), CDPORT14(1744), CDPORT15(1760),
         *        CDPORT16(1776), CDPORT17(1792), CDPORT9(1664), CGM1(384), CRPS1(389), DDP1(392), DQM1(386),
         *        ECGM1(402), EDB1(404), EPNI1(409), EPRE1(408), EPS1(406), ERPP1(412), ESB1(410), ETPPA1(413),
         *        ETPPB1(414), ETPPC1(415), FDR1(401), FDTL1(400), FMAC24(1008), FMAC25(1009), FMAC26(1011),
         *        FMAC27(1012), FMAC28(1014), FMAC29(1015), FMAC30(1017), FMAC31(1018), FMAC32(1020), FMAC33(1021),
         *        FMAC34(1023), FMAC35(1024), FMAC36(1026), FMAC37(1027), FMAC38(1029), FMAC39(1030), FMAC40(1032),
         *        FMAC41(1033), FMAC42(1035), FMAC43(1036), FMAC44(1038), FMAC45(1039), FMAC46(1041), FMAC47(1042),
         *        FQP1(407), FSRD12(1010), FSRD13(1013), FSRD14(1016), FSRD15(1019), FSRD16(1022), FSRD17(1025),
         *        FSRD18(1028), FSRD19(1031), FSRD20(1034), FSRD21(1037), FSRD22(1040), FSRD23(1043), HBC10(464),
         *        HBC11(465), HBC12(466), HBC13(467), HBC14(468), HBC15(469), HBC8(462), HBC9(463), HBMC1(460), HRC1(461),
         *        ILE2(426), ILE3(427), IPPA1(418), IPPB1(419), IPPC1(420), IPPD1(421), IPPE1(416), IPPF1(417), IPS1(385),
         *        IPT1(395), IQM1(388), IRE1(391), ITPP1(396), ITPPD1(394),
         *        MACSEC5(1424), MACSEC6(1440), MACSEC7(1456), MACSEC8(1472), MACSEC9(1504),
         *        MCP1(390), MRPS1(399), MSD4(428), MSD5(429), MSD6(430), MSD7(431), MSS1(432), NMG1(451),
         *        OCB4(454), OCB5(455), OCB6(456), OCB7(457), OCBM1(453), PDM1(397), PEM4(422), PEM5(423), PEM6(424), PEM7(425),
         *        PQP1(405), RQP1(403), SCH1(411), SIF1(452), SPB1(393), SQM1(387), TDU2(458), TDU3(459)
         * Ring4: DDHA0(576), DDHA1(577), DDHA2(578), DDHA3(579), DDHB0(580), DDHB1(581), DDHB2(582), DDHB3(583),
         *        EVNT(590), KAPS0(586), KAPS1(587), MACT(591), MDB(588), MDB_ARM(2000), OAMP(589), OLP(592),
         *        TCAM0(584), TCAM1(585)
         * Ring5: CFC0(769), CFC1(770), FCR(772), FCT(773), FDA(776), FDT(774), MESH_TOPOLOGY(775), MTM(768), RTP(771)
         * Ring7: ECI(0)
         */
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x00000017));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x22220000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x00000022));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x00333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x00440000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x00000005));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x32220000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x00000033));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x22200000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x23233332));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x22222222));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x33333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x23200003));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x00400323));
    }
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
    int disable_hard_reset = 0x0;
    soc_reg_above_64_val_t reg_above_64_val;
    SHR_FUNC_INIT_VARS(unit);

    disable_hard_reset = dnx_data_device.general.feature_get(unit, dnx_data_device_general_hard_reset_disable);
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
    reg_val = 0;
    soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, MBU_RESETf, 1);
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, TIME_SYNC_RESETf, 1);
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_RESETf, 1);
    soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IPROC_TS_RESETf, 1);
    soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_ENABLEf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));

    /*
     * Additional adjustments for the HARD reset
     */
    if ((disable_hard_reset != 0) && (SOC_IS_J2C(unit) || SOC_IS_Q2A(unit)))
    {
        /*
         * reset EP interface TXI credits at ECI side 
         */
        SHR_IF_ERR_EXIT(WRITE_ECI_PIR_CONFIGURATIONSr(unit, 0));
        SHR_IF_ERR_EXIT(WRITE_ECI_PIR_CONFIGURATIONSr(unit, 7));
        SHR_IF_ERR_EXIT(READ_ECI_PIR_CONFIGURATIONSr(unit, &reg_val));
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, TXI_CREDITS_INITf, 1);
        SHR_IF_ERR_EXIT(WRITE_ECI_PIR_CONFIGURATIONSr(unit, reg_val));
        SHR_IF_ERR_EXIT(READ_ECI_PIR_CONFIGURATIONSr(unit, &reg_val));
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, TXI_CREDITS_INITf, 0);
        SHR_IF_ERR_EXIT(WRITE_ECI_PIR_CONFIGURATIONSr(unit, reg_val));

        /*
         * reset EP interface RXI credits at iProc side 
         */
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_EPINTF_RELEASE_ALL_CREDITSr(unit, 1));

        /*
         * reset IP interface TXI credits at iProc side 
         */
        SHR_IF_ERR_EXIT(READ_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, &reg_val));
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr, &reg_val, WR_IP_INTF_CREDITSf, 0);
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr, &reg_val, IP_INTF_CREDITSf, 0);
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, reg_val));
        SHR_IF_ERR_EXIT(READ_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, &reg_val));
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr, &reg_val, WR_IP_INTF_CREDITSf, 1);
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr, &reg_val, IP_INTF_CREDITSf, 0);
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, reg_val));

        /*
         * reset IP interface RXI credits at ECI side 
         */
        SHR_IF_ERR_EXIT(READ_ECI_ECIC_MISC_RESETr(unit, &reg_val));
        soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_ENABLEf, 1);
        SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
        SHR_IF_ERR_EXIT(READ_ECI_ECIC_MISC_RESETr(unit, &reg_val));
        soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_ENABLEf, 0);
        SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
    }

    sal_usleep(5000);
    /*
     * Misc Reset OUT
     */
    if (SOC_REG_IS_VALID(unit, ECI_TOP_LEVEL_SAMPLING_CFGr))
    {
        SHR_IF_ERR_EXIT(WRITE_ECI_TOP_LEVEL_SAMPLING_CFGr(unit, 1));
    }
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, DDR_DEC_RSTNf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, DDR_ENC_RSTNf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
    if (SOC_REG_IS_VALID(unit, ECI_TOP_LEVEL_SAMPLING_CFGr) &&
        (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "disable_gated_top_level_sampling", 1) == 0))
    {
        SHR_IF_ERR_EXIT(WRITE_ECI_TOP_LEVEL_SAMPLING_CFGr(unit, 0));
    }
    reg_val = 0;
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, DDR_DEC_RSTNf, 1);
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, DDR_ENC_RSTNf, 1);
    soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_ENABLEf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
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
    if (SOC_IS_J2C(unit) || SOC_IS_Q2A(unit))
    {
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_RXI_RESET_Nf, 1);
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_ENABLEf, 1);
    }
    else
    {
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_0_RXI_RESET_Nf, 1);
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_0_ENABLEf, 1);
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_1_RXI_RESET_Nf, 1);
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_1_ENABLEf, 1);
    }
    soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, TXI_CREDITS_INIT_VALUEf, 0);
    soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, TXI_CREDITS_INITf, 0);
    soc_reg_set_field_if_exists(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_1_RXI_ENABLEf, 1);
    soc_reg_set_field_if_exists(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_0_RXI_ENABLEf, 1);
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

    CONST STATIC soc_reg_t indirect_force_bubble_reg[] = {
        CDUM_INDIRECT_FORCE_BUBBLEr,
        CDU_INDIRECT_FORCE_BUBBLEr,
        CFC_INDIRECT_FORCE_BUBBLEr,
        CGM_INDIRECT_FORCE_BUBBLEr,
        CLU_INDIRECT_FORCE_BUBBLEr,
#ifdef BLOCK_NOT_INITIALIZED    /* DCC cannot be initialized at this stage */
        DCC_INDIRECT_FORCE_BUBBLEr,
#endif
        CRPS_INDIRECT_FORCE_BUBBLEr,
        DDHA_INDIRECT_FORCE_BUBBLEr,
        DDHB_INDIRECT_FORCE_BUBBLEr,
        DHC_INDIRECT_FORCE_BUBBLEr,
        DPC_INDIRECT_FORCE_BUBBLEr,
        DQM_INDIRECT_FORCE_BUBBLEr,
        ECGM_INDIRECT_FORCE_BUBBLEr,
        ECI_INDIRECT_FORCE_BUBBLEr,
        EDB_INDIRECT_FORCE_BUBBLEr,
        EPNI_INDIRECT_FORCE_BUBBLEr,
        EPS_INDIRECT_FORCE_BUBBLEr,
        ERPP_INDIRECT_FORCE_BUBBLEr,
        ETPPA_INDIRECT_FORCE_BUBBLEr,
        ETPPB_INDIRECT_FORCE_BUBBLEr,
        ETPPC_INDIRECT_FORCE_BUBBLEr,
        EVNT_INDIRECT_FORCE_BUBBLEr,
        FCR_INDIRECT_FORCE_BUBBLEr,
        FCT_INDIRECT_FORCE_BUBBLEr,
        FDR_INDIRECT_FORCE_BUBBLEr,
        FDT_INDIRECT_FORCE_BUBBLEr,
        FSRD_INDIRECT_FORCE_BUBBLEr,
        FQP_INDIRECT_FORCE_BUBBLEr,
        HBMC_INDIRECT_FORCE_BUBBLEr,
        ILE_INDIRECT_FORCE_BUBBLEr,
        ILU_INDIRECT_FORCE_BUBBLEr,
        IPPA_INDIRECT_FORCE_BUBBLEr,
        IPPB_INDIRECT_FORCE_BUBBLEr,
        IPPC_INDIRECT_FORCE_BUBBLEr,
        IPPD_INDIRECT_FORCE_BUBBLEr,
        IPPE_INDIRECT_FORCE_BUBBLEr,
        IPPF_INDIRECT_FORCE_BUBBLEr,
        IPS_INDIRECT_FORCE_BUBBLEr,
        IPT_INDIRECT_FORCE_BUBBLEr,
        IQM_INDIRECT_FORCE_BUBBLEr,
        IRE_INDIRECT_FORCE_BUBBLEr,
        ITPPD_INDIRECT_FORCE_BUBBLEr,
        ITPP_INDIRECT_FORCE_BUBBLEr,
        KAPS_INDIRECT_FORCE_BUBBLEr,
        MACT_INDIRECT_FORCE_BUBBLEr,
        MCP_INDIRECT_FORCE_BUBBLEr,
        MDB_INDIRECT_FORCE_BUBBLEr,
        MRPS_INDIRECT_FORCE_BUBBLEr,
        MTM_INDIRECT_FORCE_BUBBLEr,
        OAMP_INDIRECT_FORCE_BUBBLEr,
        OCB_INDIRECT_FORCE_BUBBLEr,
        OLP_INDIRECT_FORCE_BUBBLEr,
        PEM_INDIRECT_FORCE_BUBBLEr,
        PQP_INDIRECT_FORCE_BUBBLEr,
        RQP_INDIRECT_FORCE_BUBBLEr,
        RTP_INDIRECT_FORCE_BUBBLEr,
        SCH_INDIRECT_FORCE_BUBBLEr,
        SPB_INDIRECT_FORCE_BUBBLEr,
        SQM_INDIRECT_FORCE_BUBBLEr,
        TCAM_INDIRECT_FORCE_BUBBLEr,
        /** FlexE related block */
        ESB_INDIRECT_FORCE_BUBBLEr,
#ifdef BLOCK_NOT_INITIALIZED    /* FlexE blocks cannot be initialized at this stage */
        FASIC_INDIRECT_FORCE_BUBBLEr,
        FLEXEWP_INDIRECT_FORCE_BUBBLEr,
        FSAR_INDIRECT_FORCE_BUBBLEr,
        FSCL_INDIRECT_FORCE_BUBBLEr,
#endif
        FEU_INDIRECT_FORCE_BUBBLEr
    };

    SHR_FUNC_INIT_VARS(unit);
    SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);

    /*
     * Temporarily allow HW writes for current thread even if it's generally disabled.
     */
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));
    /*
     * Disable memory masking
     */
    for (i = 0; i < sizeof(indirect_wr_mask_reg) / sizeof(indirect_wr_mask_reg[0]); i++)
    {
        if (SOC_REG_IS_VALID(unit, indirect_wr_mask_reg[i]))
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_set_all_instances(unit, indirect_wr_mask_reg[i], 0, reg_above_64_val));
        }
    }
    /*
     * Fill the Bubble registers in order to have access to all memories in their blocks
     */
    for (i = 0; i < sizeof(indirect_force_bubble_reg) / sizeof(indirect_force_bubble_reg[0]); i++)
    {
        if (SOC_REG_IS_VALID(unit, indirect_force_bubble_reg[i]))
        {
            SHR_IF_ERR_EXIT(soc_reg32_set_all_instances(unit, indirect_force_bubble_reg[i], 0, 0x020019));
        }
    }

    /*
     * revert dnxc_allow_hw_write_enable.
     */
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Perform soft init of SCH block
 * by writing to SCH_SCHEDULER_INIT memory
 */
shr_error_e
soc_dnx_soft_init_sch(
    int unit)
{

    uint32 error_check_en;
    uint32 entry[SOC_MAX_MEM_WORDS] = { 0 };
    uint32 mem_data;
    uint32 all_ones[SOC_MAX_MEM_WORDS];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(entry, 0x0, sizeof(entry));
    sal_memset(all_ones, 0xff, sizeof(all_ones));

    /** disable access error handling when accessing SCH_SCHEDULUER_INIT table */
    error_check_en = SOC_FEATURE_GET(unit, soc_feature_schan_err_check);
    SOC_FEATURE_CLEAR(unit, soc_feature_schan_err_check);
    if (SOC_IS_JERICHO2_ONLY(unit))
    {
        mem_data = 0;
    }
    else
    {
        mem_data = 1;
    }
    SHR_IF_ERR_EXIT(soc_mem_write(unit, SCH_SCHEDULER_INITm, MEM_BLOCK_ALL, 0, &mem_data));

    if (soc_sand_is_emulation_system(unit))
    {
        sal_sleep(10);
    }
    else
    {
        sal_usleep(1000);
    }

    /*
     * Following the write to SCH_SCHEDULER_INIT, token count of all flows is set to 2.
     * Need to set them to maximum
     */
    if (!(SAL_BOOT_PLISIM || soc_sand_is_emulation_system(unit)))
    {
        /** set to all ones */
        soc_mem_field_width_fit_set(unit, SCH_TOKEN_MEMORY_CONTROLLER_TMCm, entry, TOKEN_COUNTf, all_ones);
        soc_mem_field_width_fit_set(unit, SCH_TOKEN_MEMORY_CONTROLLER_TMCm, entry, SLOW_STATUSf, all_ones);
        soc_mem_field_width_fit_set(unit, SCH_TOKEN_MEMORY_CONTROLLER_TMCm, entry, FULLf, all_ones);

        SHR_IF_ERR_EXIT(sand_fill_table_with_entry_skip_emul_err(unit, SCH_TOKEN_MEMORY_CONTROLLER_TMCm, MEM_BLOCK_ALL,
                                                                 entry));
    }
    /** re-enable access error handling after accessing SCH_SCHEDULUER_INIT table */
    if (error_check_en)
    {
        SOC_FEATURE_SET(unit, soc_feature_schan_err_check);
    }

    /** clear number of tokens on reserved SE following soft reset */
    SHR_IF_ERR_EXIT(dnx_sch_reserve_se_update(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Perform a soft init of the device.
 * If without_fabric is not 0 perform the soft init without the Fabric blocks.
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
    int unit,
    int without_fabric,
    int without_ile)
{
    uint32 reg_val, field_val;
    uint16 dev_id;
    uint8 rev_id;

    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_above_64_val_t field_above_64_val = { 0 };
    soc_reg_above_64_val_t epni_field_above_64_val = { 0 };
    int array_index, array_index_min = 0, array_index_max = 0;
    soc_reg_above_64_val_t zero_field_val = { 0 };
    dnx_hbmc_mrs_values_t mrs_values;
    uint32 reset_cdu_eth[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };
    bcm_pbmp_t cdu_eth_pbmp[DNX_DATA_MAX_NIF_ILKN_PMS_NOF];
    int saved_rx_enable[DNX_DATA_MAX_NIF_ILKN_PMS_NOF][8];
    uint64 saved_mac_ctrl[DNX_DATA_MAX_NIF_ILKN_PMS_NOF][8];
    uint32 rx_fifo_reset_store[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };
    uint32 tx_port_reset_store[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };
    uint32 fc_reset_store[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };
    uint32 aligner_reset_store[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };
    uint32 instru_reset_store[DNX_DATA_MAX_NIF_ETH_CDU_NOF] = { 0 };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Due to HW overlook soft init also clears the MRs in the controller side, so after soft init need to write them back
     */
    SHR_IF_ERR_EXIT(dnx_hbmc_soft_init_mrs_save(unit, &mrs_values));

    /** Pre Reset SCH */

    array_index_min = SOC_REG_FIRST_ARRAY_INDEX(unit, SCH_DVS_NIF_CONFIGr);
    array_index_max = SOC_REG_NUMELS(unit, SCH_DVS_NIF_CONFIGr) + array_index_min;

    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    field_above_64_val[0] = 0xffffffff;
    soc_reg_above_64_field_set(unit, SCH_DVS_NIF_CONFIGr, reg_above_64_val, NIF_FORCE_PAUSE_Nf, field_above_64_val);
    for (array_index = array_index_min; array_index < array_index_max; array_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, array_index, reg_above_64_val));
    }
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    field_above_64_val[0] = 1;
    soc_reg_above_64_field_set(unit, SCH_DVS_CONFIGr, reg_above_64_val, FORCE_PAUSEf, field_above_64_val);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));

    /*
     * The problem is that the TMC is sending credits BEFORE both sides of the "accumulate_and_sync" are out of reset (as their reset is sampled more).
     * One option is to define that every time we reset the CDU (or the entire chip) - we must change the TMC reset register to be back in reset, and release it AFTER the CDU is out of reset.
     * This is required to use for A0.
     */
    if (SOC_IS_Q2A_A0(unit))
    {
        int block_instance;
        int number_blk_instances = 2;
        for (block_instance = 0; block_instance < number_blk_instances; block_instance++)
        {
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_TX_PORTS_SRSTNr, block_instance, 0, 0x0));
        }
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FEU_TX_PORTS_SRSTNr, 0, 0, reg_above_64_val));
    }
    /** Enter soft init */
    SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
    if (SOC_IS_Q2A(unit) || SOC_IS_J2C(unit))
    {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_SCHf, 0);
    }
    if (SOC_IS_Q2A(unit) || SOC_IS_J2C(unit))
    {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_MDBf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHAf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_KAPSf, 0);
        if (SOC_IS_Q2A(unit))
        {
            /** FlexE related blocks should not be soft-reseted, it will be done in imb_post_init */
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FLEXEWPf, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSCLf, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSARf, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FASICf, 0);
        }
    }
    else if (SOC_IS_JERICHO2_ONLY(unit))
    {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_MDBf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHA_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHA_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_SCH_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_SCH_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_KAPS_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_KAPS_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_4f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_5f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_6f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_7f, 0);
    }

    /*
     * Do not reset the FABRIC blocks
     */
    if ((without_fabric != 0) && (SOC_IS_JERICHO2_ONLY(unit) || SOC_IS_J2C(unit) || SOC_IS_J2P(unit)))
    {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FCRf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDAf, 0);
        if (SOC_IS_JERICHO2_ONLY(unit) || SOC_IS_J2P(unit))
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDR_0f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDR_1f, 0);
        }
        else
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDRf, 0);
        }
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDTf, 0);
        if (SOC_IS_J2P(unit))
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDTL_0f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDTL_1f, 0);
        }
        else
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDTLf, 0);
        }
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FCTf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_MESH_TOPOLOGYf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_RTPf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_4f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_5f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_6f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_7f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_8f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_9f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_10f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_11f, 0);
        if (SOC_IS_JERICHO2_ONLY(unit) || SOC_IS_J2P(unit))
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_12f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_13f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_14f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_15f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_16f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_17f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_18f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_19f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_20f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_21f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_22f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_23f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_24f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_25f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_26f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_27f, 0);
            if (SOC_IS_J2P(unit))
            {
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_28f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_29f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_30f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_31f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_32f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_33f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_34f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_35f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_36f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_37f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_38f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_39f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_40f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_41f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_42f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_43f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_44f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_45f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_46f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_47f,
                                             0);
            }
        }
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_4f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_5f, 0);
        if (SOC_IS_JERICHO2_ONLY(unit) || SOC_IS_J2P(unit))
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_6f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_7f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_8f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_9f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_10f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_11f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_12f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_13f, 0);
            if (SOC_IS_J2P(unit))
            {
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_14f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_15f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_16f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_17f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_18f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_19f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_20f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_21f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_22f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_23f,
                                             0);
            }
        }
    }
    /*
     * Do not reset the ILE blocks
     */
    if (without_ile != 0)
    {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_1f, 0);
        if (SOC_IS_J2C(unit) || SOC_IS_J2P(unit))
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_2f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_3f, 0);

            if (SOC_IS_J2C(unit))
            {
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_4f, 0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_5f, 0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_6f, 0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_7f, 0);
            }
        }
        /*
         * For ilkn fec in cdu, when resetting doesn't include ile, we can't reset cdu.
         * If this cdu also have eth port, we must reset the cdu eth module related.
         */
        if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_fec_units_in_cdu))
        {
            bcm_pbmp_t elk_ports_bitmap;
            bcm_port_t port = 0, active_port;
            dnx_algo_port_info_s active_port_info;
            int pm_idx, phy_id, is_active = 0;
            int nof_pm_cdu;
            bcm_pbmp_t nif_phys;
            bcm_pbmp_t pm_pmbp, pm_pmbp_temp;
            soc_field_t cdu_reset_field[] = { BLOCKS_SOFT_INIT_CDU_0f, BLOCKS_SOFT_INIT_CDU_1f, BLOCKS_SOFT_INIT_CDU_2f,
                BLOCKS_SOFT_INIT_CDU_3f
            };
            /*
             * Calculate which cdu for J2C no need to be reset
             */
            nof_pm_cdu = dnx_data_nif.eth.cdu_nof_get(unit);
            for (pm_idx = 0; pm_idx < nof_pm_cdu; pm_idx++)
            {
                BCM_PBMP_CLEAR(cdu_eth_pbmp[pm_idx]);
            }
            BCM_PBMP_CLEAR(elk_ports_bitmap);
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                            (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK, 0, &elk_ports_bitmap));
            BCM_PBMP_ITER(elk_ports_bitmap, port)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &nif_phys));
                for (pm_idx = 0; pm_idx < nof_pm_cdu; pm_idx++)
                {
                    /*
                     * If this cdu has elk on it, no need to reset this cdu.
                     */
                    pm_pmbp = dnx_data_nif.eth.pm_properties_get(unit, pm_idx)->phys;
                    BCM_PBMP_ASSIGN(pm_pmbp_temp, pm_pmbp);
                    BCM_PBMP_AND(pm_pmbp_temp, nif_phys);
                    if (BCM_PBMP_NOT_NULL(pm_pmbp_temp))
                    {
                        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                     cdu_reset_field[pm_idx], 0);
                        /*
                         * next check whether this cdu has eth on it, 
                         * if yes, find these out, and need to reset the cdu eth
                         */
                        BCM_PBMP_REMOVE(pm_pmbp, pm_pmbp_temp);
                        BCM_PBMP_ITER(pm_pmbp, phy_id)
                        {
                            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_active_get(unit, phy_id, &is_active));
                            if (is_active)
                            {
                                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get
                                                (unit, phy_id, FALSE, 0, &active_port));
                                SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, active_port, &active_port_info));
                                if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, active_port_info, TRUE, TRUE))
                                {
                                    reset_cdu_eth[pm_idx] = 1;
                                    BCM_PBMP_PORT_ADD(cdu_eth_pbmp[pm_idx], active_port);
                                }
                            }
                        }
                    }
                }
            }   /* end of elk port iter */
        }
    }

    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
    sal_usleep(2000);

    if (without_ile != 0 && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_fec_units_in_cdu))
    {
        int cdu_idx, array_idx;
        bcm_port_t active_eth_port;
        int nof_pm_cdu = dnx_data_nif.eth.cdu_nof_get(unit);
        /*
         * reset cdu eth
         */
        sal_memset(saved_rx_enable, 0x0, sizeof(saved_rx_enable));
        sal_memset(saved_mac_ctrl, 0x0, sizeof(saved_mac_ctrl));

        for (cdu_idx = 0; cdu_idx < nof_pm_cdu; cdu_idx++)
        {
            if (reset_cdu_eth[cdu_idx] == 0)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(soc_reg32_get(unit, CDU_RX_FIFOS_SRSTNr, cdu_idx, 0, &rx_fifo_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_RX_FIFOS_SRSTNr, cdu_idx, 0, 0x0));

            SHR_IF_ERR_EXIT(soc_reg32_get(unit, CDU_TX_PORTS_SRSTNr, cdu_idx, 0, &tx_port_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_TX_PORTS_SRSTNr, cdu_idx, 0, 0x0));

            SHR_IF_ERR_EXIT(soc_reg32_get(unit, CDU_FC_RESETr, cdu_idx, 0, &fc_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_FC_RESETr, cdu_idx, 0, 0x1));

            SHR_IF_ERR_EXIT(soc_reg32_get(unit, CDU_ALIGNER_MEMS_RESETr, cdu_idx, 0, &aligner_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_ALIGNER_MEMS_RESETr, cdu_idx, 0, 0x0));

            SHR_IF_ERR_EXIT(soc_reg32_get
                            (unit, CDU_INSTRUMENTATION_RESET_LOWr, cdu_idx, 0, &instru_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_INSTRUMENTATION_RESET_LOWr, cdu_idx, 0, 0x0));
            /*
             * Put Mac soft reset
             */
            array_idx = 0;
            BCM_PBMP_ITER(cdu_eth_pbmp[cdu_idx], active_eth_port)
            {
                SHR_IF_ERR_EXIT(portmod_port_soft_reset(unit, active_eth_port, portmodMacSoftResetModeIn,
                                                        &saved_rx_enable[cdu_idx][array_idx],
                                                        &saved_mac_ctrl[cdu_idx][array_idx]));
                array_idx++;
            }
        }
    }

    if (SOC_REG_IS_VALID(unit, EPNI_INIT_EPNI_NIF_TXIr))
    {
        /*
         * Init EPNI NIF TXI
         * Nif-txi should be initialized before NIF sends new credits.
         */
        int field_size = 0;
        field_size = soc_reg_field_length(unit, EPNI_INIT_EPNI_NIF_TXIr, INIT_EPNI_NIF_TXIf);

        SOC_REG_ABOVE_64_CREATE_MASK(epni_field_above_64_val, field_size, 0);

        SHR_IF_ERR_EXIT(READ_ESB_FPC_0_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_0_CONFIGr, &reg_val, FPC_0_INITf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_0_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_1_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_1_CONFIGr, &reg_val, FPC_1_INITf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_1_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_2_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_2_CONFIGr, &reg_val, FPC_2_INITf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_2_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_3_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_3_CONFIGr, &reg_val, FPC_3_INITf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_3_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, EPNI_INIT_EPNI_NIF_TXIr, REG_PORT_ANY, 0, reg_above_64_val));
        soc_reg_above_64_field_set(unit, EPNI_INIT_EPNI_NIF_TXIr, reg_above_64_val, INIT_EPNI_NIF_TXIf,
                                   epni_field_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPNI_INIT_EPNI_NIF_TXIr, REG_PORT_ANY, 0, reg_above_64_val));

        sal_usleep(1000);

        SHR_IF_ERR_EXIT(READ_ESB_FPC_0_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_0_CONFIGr, &reg_val, FPC_0_INITf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_0_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_1_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_1_CONFIGr, &reg_val, FPC_1_INITf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_1_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_2_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_2_CONFIGr, &reg_val, FPC_2_INITf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_2_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_3_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_3_CONFIGr, &reg_val, FPC_3_INITf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_3_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SOC_REG_ABOVE_64_CLEAR(epni_field_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, EPNI_INIT_EPNI_NIF_TXIr, REG_PORT_ANY, 0, reg_above_64_val));
        soc_reg_above_64_field_set(unit, EPNI_INIT_EPNI_NIF_TXIr, reg_above_64_val, INIT_EPNI_NIF_TXIf,
                                   epni_field_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPNI_INIT_EPNI_NIF_TXIr, REG_PORT_ANY, 0, reg_above_64_val));
        sal_usleep(5000);
    }

    if (SOC_IS_JERICHO2_ONLY(unit))
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
    if (SOC_IS_Q2A(unit) || SOC_IS_J2C(unit))
    {
        SOC_REG_ABOVE_64_CREATE_MASK(zero_field_val, 0, 0);
        /*
         * EPNI , FQP , ESB out of reset
         */
        SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FQPf,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_EPNIf,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ESBf,
                                   zero_field_val);
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
    }
    sal_usleep(1000);
    if (SOC_REG_IS_VALID(unit, ESB_ESB_INITr))
    { /** Init ESB if the device has it */
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_ESB_INITr, REG_PORT_ANY, 0, 0));
        sal_usleep(5000);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_ESB_INITr, REG_PORT_ANY, 0, 1));
        sal_usleep(5000);
    }
    if (SOC_IS_Q2A(unit) || SOC_IS_J2C(unit))
    {
        /*
         * NMG out of reset: must be before NIF block
         */
        SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_NMGf,
                                   zero_field_val);
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        sal_usleep(1000);
    }
/* *INDENT-OFF* */
    /* If do soft reset without ILE block, should do below sqeucne.
     * In default, we has put ILU to soft-reset, hence here, we only
     * do step 2 and 3.
     * 1.Insert ILU to soft-reset (with the rest of the device).
     * 2.Set ILE_ILKN_TX_PORT_FORCE_DATA_PCK_FLUSH=1,for all relevant ports of the device.
     * 3.Set ILE_ILKN_TX_PORT_FORCE_DATA_PCK_FLUSH=0,for all relevant ports of the device.
     * 4.Get ILU out-of soft-reset (with the rest of the device).
     */
/* *INDENT-ON* */
    if (without_ile != 0)
    {
        int block, array_iter;
        soc_reg_t reg;
        uint32 nof_block = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);

        reg = ILE_ILKN_TX_PORT_FORCE_DATA_PCK_FLUSHr;
        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
        for (block = 0; block < nof_block; block++)
        {
            for (array_iter = 0; array_iter < SOC_REG_NUMELS(unit, reg); array_iter++)
            {
                SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, block, array_iter, reg_above_64_val));
            }
        }
        /*
         * no need to wait
         */
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        for (block = 0; block < nof_block; block++)
        {
            for (array_iter = 0; array_iter < SOC_REG_NUMELS(unit, reg); array_iter++)
            {
                SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, block, array_iter, reg_above_64_val));
            }
        }
    }

    if (SOC_IS_Q2A(unit))
    {
        /*
         * CDU, CLU, ILE, ILU out of reset
         */
        SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_0f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_1f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_0f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_1f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_2f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_0f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_1f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_0f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_1f,
                                   zero_field_val);
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        sal_usleep(1000);
    }
    else if (SOC_IS_J2C(unit))
    {
        /*
         * CDU, CLU, ILE, ILU out of reset
         */
        SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_0f, zero_field_val); /** CDU0 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_1f, zero_field_val); /** CDU1 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_2f, zero_field_val); /** CDU2 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_3f, zero_field_val); /** CDU3 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_0f, zero_field_val); /** CLU0 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_1f, zero_field_val); /** CLU1 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_2f, zero_field_val); /** CLU2 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_3f, zero_field_val); /** CLU3 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_4f, zero_field_val); /** CLU4 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_5f, zero_field_val); /** CLU5 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_0f, zero_field_val); /** ILE0 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_1f, zero_field_val); /** ILE1 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_2f, zero_field_val); /** ILE2 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_3f, zero_field_val); /** ILE3 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_4f, zero_field_val); /** ILE4 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_5f, zero_field_val); /** ILE5 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_6f, zero_field_val); /** ILE6 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_7f, zero_field_val); /** ILE7 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_0f, zero_field_val); /** ILU0 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_1f, zero_field_val); /** ILU1 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_2f, zero_field_val); /** ILU2 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_3f, zero_field_val); /** ILU3 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_4f, zero_field_val); /** ILU4 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_5f, zero_field_val); /** ILU5 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_6f, zero_field_val); /** ILU6 */
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_7f, zero_field_val); /** ILU7 */
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        sal_usleep(1000);
    }

    /** Exit soft init */
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
    sal_usleep(5000);

    if (without_ile != 0 && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_fec_units_in_cdu))
    {
        int cdu_idx, array_idx;
        bcm_port_t active_eth_port;
        int nof_pm_cdu = dnx_data_nif.eth.cdu_nof_get(unit);
        /*
         * restore out of reset cdu eth
         */
        for (cdu_idx = 0; cdu_idx < nof_pm_cdu; cdu_idx++)
        {
            if (reset_cdu_eth[cdu_idx] == 0)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_RX_FIFOS_SRSTNr, cdu_idx, 0, rx_fifo_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_TX_PORTS_SRSTNr, cdu_idx, 0, tx_port_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_FC_RESETr, cdu_idx, 0, fc_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_ALIGNER_MEMS_RESETr, cdu_idx, 0, aligner_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set
                            (unit, CDU_INSTRUMENTATION_RESET_LOWr, cdu_idx, 0, instru_reset_store[cdu_idx]));
            /*
             * Put Mac out of soft reset 
             */
            array_idx = 0;
            BCM_PBMP_ITER(cdu_eth_pbmp[cdu_idx], active_eth_port)
            {
                SHR_IF_ERR_EXIT(portmod_port_soft_reset(unit, active_eth_port, portmodMacSoftResetModeOut,
                                                        &saved_rx_enable[cdu_idx][array_idx],
                                                        &saved_mac_ctrl[cdu_idx][array_idx]));
                array_idx++;
            }
        }
    }

    if (SOC_IS_Q2A_A0(unit))
    {
        /*
         * The problem is that the TMC is sending credits BEFORE both sides of the "accumulate_and_sync" are out of reset (as their reset is sampled more).
         * One option is to define that every time we reset the CDU (or the entire chip) - we must change the TMC reset register to be back in reset, and release it AFTER the CDU is out of reset.
         * This is required to use for A0.
         */
        int block_instance;
        int number_blk_instances = 2;
        for (block_instance = 0; block_instance < number_blk_instances; block_instance++)
        {
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_TX_PORTS_SRSTNr, block_instance, 0, 0xff));
        }

        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FEU_TX_PORTS_SRSTNr, 0, 0, reg_above_64_val));
    }
    if (SOC_REG_IS_VALID(unit, ESB_ESB_CALENDER_SETTINGr))
    {
        /*
         * Without it esb scheduler will think calendar is empty. We must toggle this bit after reset.
         */
        sal_usleep(1000);
        SHR_IF_ERR_EXIT(READ_ESB_ESB_CALENDER_SETTINGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_ESB_CALENDER_SETTINGr, &reg_val, ESB_CALENDAR_SWITCH_ENf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_ESB_CALENDER_SETTINGr, REG_PORT_ANY, 0, reg_val));
        sal_usleep(1000);
        SHR_IF_ERR_EXIT(READ_ESB_ESB_CALENDER_SETTINGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_ESB_CALENDER_SETTINGr, &reg_val, ESB_CALENDAR_SWITCH_ENf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_ESB_CALENDER_SETTINGr, REG_PORT_ANY, 0, reg_val));
        sal_usleep(1000);
    }
    /** soft init scheduler - initialize dynamic memories */
    {
        SHR_IF_ERR_EXIT(soc_dnx_soft_init_sch(unit));
    }

    if (!SAL_BOOT_PLISIM)
    { /** wait until EGQ blocks init are done: */
        uint32 prp_init, eqm_init, user_counter_init, aligner_init, fpc_init_stat = 0, init_not_done = 1, i;

        for (i = 2; i && init_not_done;)
        {
            SHR_IF_ERR_EXIT(READ_RQP_RQP_BLOCK_INIT_STATUSr(unit, 0, &prp_init));
            SHR_IF_ERR_EXIT(READ_PQP_EGQ_BLOCK_INIT_STATUSr(unit, 0, &eqm_init));       /* checks more inits */
            SHR_IF_ERR_EXIT(READ_FQP_FQP_BLOCK_INIT_STATUSr(unit, 0, &user_counter_init));
            SHR_IF_ERR_EXIT(READ_EPNI_EPNI_INIT_STATUSr(unit, 0, &aligner_init));
            if (SOC_REG_IS_VALID(unit, ESB_FPC_0_STATr))
            {
                SHR_IF_ERR_EXIT(READ_ESB_FPC_0_STATr(unit, SOC_CORE_ALL, &init_not_done));
                fpc_init_stat = soc_reg_field_get(unit, ESB_FPC_0_STATr, init_not_done, FPC_0_INIT_STATf);
                SHR_IF_ERR_EXIT(READ_ESB_FPC_1_STATr(unit, SOC_CORE_ALL, &init_not_done));
                fpc_init_stat |= soc_reg_field_get(unit, ESB_FPC_1_STATr, init_not_done, FPC_1_INIT_STATf) << 1;
                SHR_IF_ERR_EXIT(READ_ESB_FPC_2_STATr(unit, SOC_CORE_ALL, &init_not_done));
                fpc_init_stat |= soc_reg_field_get(unit, ESB_FPC_2_STATr, init_not_done, FPC_2_INIT_STATf) << 2;
                SHR_IF_ERR_EXIT(READ_ESB_FPC_3_STATr(unit, SOC_CORE_ALL, &init_not_done));
                fpc_init_stat |= soc_reg_field_get(unit, ESB_FPC_3_STATr, init_not_done, FPC_3_INIT_STATf) << 3;
            }
            /** mask out control fields, leave only status fields */
            eqm_init = eqm_init & 0x7;

            init_not_done = prp_init | eqm_init | user_counter_init | aligner_init | fpc_init_stat;
            if (init_not_done != 0 && --i != 0)
            {
                sal_usleep(5000);
                /** We expect this wait will never be needed */
                LOG_ERROR(BSL_LS_SOC_INIT, ("Warning: waiting for EGQ blocks to be ready\n"));
            }

        }
        if (init_not_done != 0)
        { /** We do currently not stop the init on this failure */
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U
                       (unit,
                        "EGQ blocks did not become ready  prp_init=%u eqm_init=%u user_counter_init=%u aligner_init=%u fpc_init_stat=0x%x\n"),
                       prp_init, eqm_init, user_counter_init, aligner_init, fpc_init_stat));
        }
    }

    /** Post Reset SCH */
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    soc_reg_above_64_field_set(unit, SCH_DVS_NIF_CONFIGr, reg_above_64_val, NIF_FORCE_PAUSE_Nf, field_above_64_val);
    for (array_index = array_index_min; array_index < array_index_max; array_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, array_index, reg_above_64_val));
    }
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    soc_reg_above_64_field_set(unit, SCH_DVS_CONFIGr, reg_above_64_val, FORCE_PAUSEf, field_above_64_val);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));

    /** test s-bus access by reading some registers */
    soc_cm_get_id(unit, &dev_id, &rev_id);
    SHR_IF_ERR_EXIT(READ_ECI_VERSION_REGISTERr(unit, reg_above_64_val));
    field_val = soc_reg_above_64_field32_get(unit, ECI_VERSION_REGISTERr, reg_above_64_val, CHIP_TYPEf);
#ifdef JER2B0_BRINGUP_DONE
    if ((field_val & 0xffff) != dev_id)
#else
    if (((field_val ^ dev_id) & 0xfff0) != 0)
#endif
    {
        LOG_ERROR_EX(BSL_LOG_MODULE, "Chip version is wrong: unit %d expected 0x%x and found 0x%x\n%s", unit,
                     dev_id, field_val, EMPTY);
    }
    SHR_IF_ERR_EXIT(soc_dnxc_verify_device_init(unit));
    SHR_IF_ERR_EXIT(READ_SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 0, &reg_val));
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, IRE_STATIC_CONFIGURATIONr, 0, 0, &reg_val));

    /*
     * Rebuild dram deleted buffers list that was reset during soft init
     */
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_restore_deleted_buffers_after_soft_init(unit));

    /*
     * Restore the previously saved MRs
     */
    SHR_IF_ERR_EXIT(dnx_hbmc_soft_init_mrs_restore(unit, &mrs_values));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* The function is performing the MBIST test sequence and also TR 10 if enabled */
shr_error_e
soc_dnx_perform_bist_tests(
    int unit)
{
    dnx_startup_test_function_f test_function = dnx_startup_test_functions[unit];
    utilex_seq_step_t *utilex_list = NULL;
    uint32 mbist_soc_prop_enable = dnx_data_device.general.bist_enable_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit))
    {

        /*
         * Get the "cpu2tap_enable" custom_feature soc proprty.
         */
        if (SOC_IS_Q2A(unit)
            && (dnx_drv_soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "cpu2tap_enable", 1) != 0))
        {
            SHR_IF_ERR_EXIT(soc_q2a_cpu2tap_init_mems(unit));
        }
        /*
         * If the value for the global variable is enabled perform TR 10 testing
         */
        if (test_function != NULL)
        {
            test_function(unit);
        }
        /** Perform if MBIST is enabled by TR5 or by soc property, and not in simulation */
        if (((any_bist_performed[unit / 32] & (((uint32) 1) << (unit % 32))) ||
             mbist_soc_prop_enable) && !SAL_BOOT_PLISIM)
        {
            int step_found, i, mbist_arg = (mbist_soc_prop_enable != 1);

            int deinit_steps[] = { DNX_INIT_STEP_DMA, DNX_INIT_STEP_SBUS, DNX_INIT_STEP_INTERRUPT, DNX_INIT_STEP_PLL };
            int init_steps[] =
                { DNX_INIT_STEP_IPROC, DNX_INIT_STEP_HARD_RESET, DNX_INIT_STEP_SBUS, DNX_INIT_STEP_INTERRUPT,
                DNX_INIT_STEP_SOFT_RESET,
                DNX_INIT_STEP_DMA, DNX_INIT_STEP_PLL,
                DNX_INIT_STEP_SBUS_INTERRUPTS
            };
            do
            {
                if (SOC_IS_J2P(unit))
                {
                    SHR_IF_ERR_EXIT(soc_bist_all_j2p(unit, mbist_arg));
                }
                else if (SOC_IS_J2C(unit))
                {
                    SHR_IF_ERR_EXIT(soc_bist_all_j2c(unit, mbist_arg));
                }
                else if (SOC_IS_Q2A(unit))
                {
                    SHR_IF_ERR_EXIT(soc_bist_all_q2a(unit, mbist_arg));
                }
                else
                {
                    SHR_IF_ERR_EXIT(soc_bist_all_jr2(unit, mbist_arg));
                }
            }
            while (mbist_soc_prop_enable == 9999);   /** This values is for looping forever in mbist */

            /*
             * Mark that the MBIST is successfull 
             */
            test_was_run_and_suceeded[unit / 32] |= ((uint32) 1) << (unit % 32);

            SHR_IF_ERR_EXIT(dnx_init_step_list_convert(unit, dnx_init_deinit_seq, &utilex_list));

            /*
             * De-init of the steps
             */
            for (i = 0; i < sizeof(deinit_steps) / sizeof(deinit_steps[0]); i++)
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
            for (i = 0; i < sizeof(init_steps) / sizeof(init_steps[0]); i++)
            {
                SHR_IF_ERR_EXIT(utilex_seq_run_step_by_id(unit, utilex_list, init_steps[i], 1, &step_found));
                if (step_found != 1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid step \n");
                }
            }

        }

        /** Set HBMs' WRST_N signal to 0 as start of HBM reset sequence */
        if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
        {
            int hbm_id = dnx_data_dram.general_info.max_nof_drams_get(unit);
            for (--hbm_id; hbm_id >= 0; --hbm_id)
            {
                if ((dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap & (1 << hbm_id)))
                {
                    SHR_IF_ERR_EXIT(soc_set_hbm_wrst_n_to_0(unit, hbm_id));
                }
            }
        }

    }
exit:
    if (utilex_list != NULL)
    {
        dnx_init_step_list_destory(unit, utilex_list);
    }
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

        if (SOC_IS_JERICHO2_ONLY(unit))
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
    int cmc_index = 0;
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize CMIC info
     */

    /*
     * Number of CMCs, 2 for CMICx
     */
    SOC_CMCS_NUM(unit) = dnx_data_dev_init.cmc.cmc_num_get(unit);
    /*
     * Number of classes-of-service for RX DMA
     */
    NUM_CPU_COSQ(unit) = dnx_data_dev_init.cmc.num_cpu_cosq_get(unit);
    /*
     * CMC in CMICx used by the PCI Host, CMC 0 by default
     */
    SOC_PCI_CMC(unit) = dnx_data_dev_init.cmc.cmc_pci_get(unit);
    /*
     * Number of CMCs used by the PCI Host, 1 by default
     */
    SOC_PCI_CMCS_NUM(unit) = dnx_data_dev_init.cmc.pci_cmc_num_get(unit);
    /*
     * CMC in CMICx used by the micro controller 0, CMC 1 by default
     */
    SOC_ARM_CMC(unit, 0) = dnx_data_dev_init.cmc.cmc_uc0_get(unit);
    /*
     * CMC in CMICx used by the micro controller 1, CMC 1 by default
     */
    SOC_ARM_CMC(unit, 1) = dnx_data_dev_init.cmc.cmc_uc1_get(unit);
    /*
     * Number of RX DMA COS for CMC 0, 64 by default
     */
    NUM_CPU_ARM_COSQ(unit, 0) = dnx_data_dev_init.cmc.num_queues_pci_get(unit);
    /*
     * Number of RX DMA COS for CMC 1, 0 by default. Given both UC0 and UC1 on CMC 1.
     */
    NUM_CPU_ARM_COSQ(unit, 1) =
        dnx_data_dev_init.cmc.num_queues_uc0_get(unit) + dnx_data_dev_init.cmc.num_queues_uc1_get(unit);
    if ((NUM_CPU_ARM_COSQ(unit, 0) + NUM_CPU_ARM_COSQ(unit, 1)) > NUM_CPU_COSQ(unit))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_PARAM,
                                 "Number of cosq reserved for CMC 0/1 exceed. num_queues_pci %u num_queues_uc0 %d\n%s",
                                 NUM_CPU_ARM_COSQ(unit, 0), NUM_CPU_ARM_COSQ(unit, 1), EMPTY);
    }

    /*
     * Clear cosq bitmaps per cmc
     */
    for (cmc_index = 0; cmc_index < SOC_CMCS_NUM(unit); cmc_index++)
    {
        SHR_BITCLR_RANGE(CPU_ARM_QUEUE_BITMAP(unit, cmc_index), 0, NUM_CPU_COSQ(unit));
    }

    /*
     * Set cosq bitmaps per cmc
     */
    SHR_BITSET_RANGE(CPU_ARM_QUEUE_BITMAP(unit, 0), 0, NUM_CPU_ARM_COSQ(unit, 0));
    SHR_BITSET_RANGE(CPU_ARM_QUEUE_BITMAP(unit, 1), NUM_CPU_ARM_COSQ(unit, 0), NUM_CPU_ARM_COSQ(unit, 1));

    /*
     * Packet DMA descriptors Initilazation
     */
    soc_dcb_unit_init(unit);

    if (SOC_WARM_BOOT(unit))
    {
        /*
         * When reset is false, the release of credits won't be done.
         * It should be done only once after both CMIC and EP blocks are out of reset.
         */
        SHR_IF_ERR_EXIT(soc_dma_attach(unit, FALSE));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_dma_attach(unit, TRUE));
    }

#ifdef BCM_SBUSDMA_SUPPORT
    SOC_CONTROL(unit)->max_sbusdma_channels = SOC_DNXC_MAX_SBUSDMA_CHANNELS;
    SOC_CONTROL(unit)->tdma_ch = SOC_DNXC_TDMA_CHANNEL;
    SOC_CONTROL(unit)->tslam_ch = SOC_DNXC_TSLAM_CHANNEL;
    SOC_CONTROL(unit)->desc_ch = SOC_DNXC_DESC_CHANNEL;
    /*
     * maximum possible memory entry size used for clearing memory, should be a multiple of 32bit words
     */
    SOC_MEM_CLEAR_CHUNK_SIZE_SET(unit, dnx_data_intr.general.mem_clear_chunk_size_get(unit));
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
                                                                                                                                                                                                                 * if DMA
                                                                                                                                                                                                                 * is
                                                                                                                                                                                                                 * enabled 
                                                                                                                                                                                                                 */
        {
            /*
             * check if DMA is enabled
             */
            SHR_IF_ERR_EXIT(soc_sbusdma_init(unit, dnx_data_intr.general.sbus_dma_interval_get(unit),
                                             dnx_data_intr.general.sbus_dma_intr_enable_get(unit)));
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

    /** Init indirect memory access */
    SHR_IF_ERR_EXIT(soc_sand_indirect_mem_access_init(unit));

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

    /** De-init indirect memory access */
    SHR_IF_ERR_EXIT(soc_sand_indirect_mem_access_deinit(unit));

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
    SOC_PCI_CMCS_NUM(unit) = dnx_data_dev_init.cmc.pci_cmc_num_get(unit);

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
    si->num_time_interface = 1;

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

/**
 * \brief - init dnx features
 */
static shr_error_e
soc_dnx_feature_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_feature_init(unit);

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
     * Restore features to be identical to the features Initlized by soc_dnx_attach()
     */
    SHR_IF_ERR_CONT(soc_dnx_feature_init(unit));

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
    {
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

shr_error_e
soc_dnx_block_enable_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /**
     disable flexe related blocks, because the PLL input from serdes
     once PLL enabled, re-enable those blocks.
     */
    if (SOC_IS_Q2A(unit))
    {
        /** skip simulation */
#ifndef PLISIM
        if (!(SAL_BOOT_PLISIM))
        {
            SOC_INFO(unit).block_valid[SOC_INFO(unit).fasic_block] = 0;
            SOC_INFO(unit).block_valid[SOC_INFO(unit).flexewp_block] = 0;
            SOC_INFO(unit).block_valid[SOC_INFO(unit).fsar_block] = 0;
            SOC_INFO(unit).block_valid[SOC_INFO(unit).fscl_block] = 0;
        }
#endif
    }
    /**
     configure dram blocks
     */
    SHR_IF_ERR_EXIT(dnx_dram_blocks_enable_set(unit));

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

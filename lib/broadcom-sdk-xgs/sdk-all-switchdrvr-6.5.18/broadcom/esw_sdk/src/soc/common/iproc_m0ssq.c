/*
 * $Id: iproc_m0ssq.c$
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#if defined(BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
#include <soc/mcm/cmicx.h>
#include <soc/mcm/intr_iproc.h>
#include <soc/mcm/memregs.h>
#include <soc/intr.h>
#include <soc/cm.h>
#include <soc/cmic.h>
#include <soc/drv.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_er_threading.h>
#endif
#ifdef BCM_CMICX_SUPPORT
#include <soc/iproc.h>
#include <soc/cmicx.h>
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/iproc_fwconfig.h>
#include <shared/cmicfw/cmicx_led.h>
#include <shared/cmicfw/cmicx_link.h>

/* iProc 15 or below. */
#define SW_PROG_INTR_IRQ                73

/* iProc 16 or above. */
#define P16_SW_PROG_INTR_IRQ            69
#define P16_SELF_RESETf                 1

static int iproc_m0ssq_init_done[SOC_MAX_NUM_DEVICES] = {0};

static const soc_reg_t control_reg[MAX_UCORES] = {
    U0_M0SS_CONTROLr,
    U1_M0SS_CONTROLr,
    U2_M0SS_CONTROLr,
    U3_M0SS_CONTROLr
};

static const soc_field_t control_field[MAX_UCORES] = {
    CORTEXM0_U0f,
    CORTEXM0_U1f,
    CORTEXM0_U2f,
    CORTEXM0_U3f
};

static const soc_reg_t irq_mask[MAX_UCORES] = {
    U0_M0SS_INTR_MASK_95_64r,
    U1_M0SS_INTR_MASK_95_64r,
    U2_M0SS_INTR_MASK_95_64r,
    U3_M0SS_INTR_MASK_95_64r,
};

static const soc_reg_t p16_irq_en[MAX_UCORES] = {
    U0_SW_PROG_INTR_ENABLEr,
    U1_SW_PROG_INTR_ENABLEr,
    U2_SW_PROG_INTR_ENABLEr,
    U3_SW_PROG_INTR_ENABLEr,
};

static const soc_reg_t p16_irq_set[MAX_UCORES] = {
    U0_SW_PROG_INTR_SETr,
    U1_SW_PROG_INTR_SETr,
    U2_SW_PROG_INTR_SETr,
    U3_SW_PROG_INTR_SETr,
};

static const soc_reg_t p16_irq_clr[MAX_UCORES] = {
    U0_SW_PROG_INTR_CLRr,
    U1_SW_PROG_INTR_CLRr,
    U2_SW_PROG_INTR_CLRr,
    U3_SW_PROG_INTR_CLRr,
};

uint32 soc_iproc_percore_membase_get(int unit, int ucnum)
{
    /* Check if corresponding uC exist. */
    if (!SOC_REG_IS_VALID(unit, control_reg[ucnum])) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "ucore %d doesn't exist.\n"), ucnum));
        assert(SOC_REG_IS_VALID(unit, control_reg[ucnum]));
        return 0xFFFFFFFF;
    };

    /* Get uC TCAM address by uC's control register. */
    return soc_reg_addr(unit, control_reg[ucnum], REG_PORT_ANY, 0) & 0xFFFF0000;
}

uint32 soc_iproc_percore_memsize_get(int unit, int ucnum)
{
    /* Check if corresponding uC exist. */
    if (!SOC_REG_IS_VALID(unit, control_reg[ucnum])) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "ucore %d doesn't exist.\n"), ucnum));
        assert(SOC_REG_IS_VALID(unit, control_reg[ucnum]));
        return 0xFFFFFFFF;
    };

    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        return IPROC_M0SSQ_TCAM_SIZE_HX5;
    } else {
        return IPROC_M0SSQ_TCAM_SIZE;
    }
}

uint32 soc_iproc_sram_membase_get(int unit)
{
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        return IPROC_M0SSQ_SRAM_BASE_HX5;
    } else {
        return IPROC_M0SSQ_SRAM_BASE;
    }
}

/*
 * Function:
 *     soc_iproc_m0_self_reset_set
 * Purpose:
 *     Send a interrupt to M0 FW. Let M0 FW reset M0 itself.
 * Parameters:
 *     unit number
 *     ucore number
 *     reset Soft reset value.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_iproc_m0_self_reset_set(int unit, int ucore, int reset)
{
    uint32 val = 0;

    /* Device with iProc16 or above. */
    if (SOC_IS_HELIX5(unit)) {

        if (reset) {

            /* Enable interrupt mask. */
            soc_iproc_getreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), &val);
            val |= (1 << (P16_SW_PROG_INTR_IRQ - 64));
            soc_iproc_setreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), val);

            /* Enable software interrupt to M0. */
            soc_iproc_getreg(unit, soc_reg_addr(unit, p16_irq_en[ucore], REG_PORT_ANY, 0), &val);
            val |= P16_SELF_RESETf;
            soc_iproc_setreg(unit, soc_reg_addr(unit, p16_irq_en[ucore], REG_PORT_ANY, 0), val);

            /* Set software interrupt to M0. */
            val = P16_SELF_RESETf;
            soc_iproc_setreg(unit, soc_reg_addr(unit, p16_irq_set[ucore], REG_PORT_ANY, 0), val);

        } else {

            /* Disable interrupt mask. */
            soc_iproc_getreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), &val);
            val &= ~(1 << (P16_SW_PROG_INTR_IRQ - 64));
            soc_iproc_setreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), val);

            /* Clear software interrupt to M0. */
            val = P16_SELF_RESETf;
            soc_iproc_setreg(unit, soc_reg_addr(unit, p16_irq_clr[ucore], REG_PORT_ANY, 0), val);

            /* Disable interrupt.  */
            soc_iproc_getreg(unit, soc_reg_addr(unit, p16_irq_en[ucore], REG_PORT_ANY, 0), &val);
            val &= ~(P16_SELF_RESETf);
            soc_iproc_setreg(unit, soc_reg_addr(unit, p16_irq_en[ucore], REG_PORT_ANY, 0), val);
        }

    } else {

        if (reset) {

          /* Enable interrupt mask. */
          soc_iproc_getreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), &val);
          val |= (1 << (SW_PROG_INTR_IRQ - 64));
          soc_iproc_setreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), val);

	  /* Set software interrupt to M0. */
          READ_ICFG_PCIE_SW_PROG_INTRr(unit, &val);
          soc_reg_field_set(unit, ICFG_PCIE_SW_PROG_INTRr, &val, control_field[ucore], 1);
          WRITE_ICFG_PCIE_SW_PROG_INTRr(unit, val);
        } else {

          /* Disable interrupt mask. */
          soc_iproc_getreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), &val);
          val &= ~(1 << (SW_PROG_INTR_IRQ - 64));
          soc_iproc_setreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), val);

          /* Clear software interrupt to M0. */
          READ_ICFG_PCIE_SW_PROG_INTRr(unit, &val);
          soc_reg_field_set(unit, ICFG_PCIE_SW_PROG_INTRr, &val, control_field[ucore], 0);
          WRITE_ICFG_PCIE_SW_PROG_INTRr(unit, val);
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_iproc_m0ssq_reset_ucore
 * Purpose:
 *     Reset Cortex-M0 in Iproc subsystem quad
 * Parameters:
 *     unit number
 *     ucore number
 *     reset Soft reset value.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_iproc_m0ssq_reset_ucore(int unit, int ucore, int reset)
{
    uint32 val = 0;
    int cur_reset = 0;
    soc_timeout_t to;

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit,"ucore 0x%x reset 0x%x\n"), ucore, reset));

    /* Parameter check. */
    if (ucore >= MAX_UCORES || !SOC_REG_IS_VALID(unit, control_reg[ucore])) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "ucore %d doesn't exist.\n"), ucore));
        return SOC_E_PARAM;
    }

    /* Covert reset value as boolean. */
    reset = (reset != 0);

    /* Get reset bit. */
    soc_iproc_getreg(unit, soc_reg_addr(unit, control_reg[ucore], REG_PORT_ANY, 0), &val);

    /* If FW is running, notify FW to do self-reset interrupt. */
    cur_reset = soc_reg_field_get(unit, control_reg[ucore], val, SOFT_RESETf);

    LOG_DEBUG(BSL_LS_SOC_M0, (BSL_META_U(unit,"ucore 0x%x cur_reset 0x%x\n"), ucore, cur_reset));
    if (reset) {

        if (cur_reset == 0) {

            LOG_DEBUG(BSL_LS_SOC_M0,
                     (BSL_META_U(unit,"ucore 0x%x self reset.\n"), ucore));

            /* Set M0 to reset itself */
            soc_iproc_m0_self_reset_set(unit, ucore, 1);

            /* Check reset completion, timeout = 200ms. */
            soc_timeout_init(&to, 200000, 100);
            do {
                 soc_iproc_getreg(unit, soc_reg_addr(unit, control_reg[ucore], REG_PORT_ANY, 0), &val);
                 cur_reset = soc_reg_field_get(unit, control_reg[ucore], val, SOFT_RESETf);
            } while ((cur_reset != 1) && !soc_timeout_check(&to));

            if (cur_reset != 1) {
                LOG_ERROR(BSL_LS_SOC_M0,
                         (BSL_META_U(unit,"ucore 0x%x self reset fail.\n"), ucore));
            }
        }

        /* Clear M0 self reset. */
        soc_iproc_m0_self_reset_set(unit, ucore, 0);
    }

    /* Set reset bit. */
    soc_iproc_getreg(unit, soc_reg_addr(unit, control_reg[ucore], REG_PORT_ANY, 0), &val);
    soc_reg_field_set(unit, control_reg[ucore], &val, SOFT_RESETf, reset);
    soc_iproc_setreg(unit, soc_reg_addr(unit, control_reg[ucore], REG_PORT_ANY, 0), val);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_m0ssq_reset_ucore_get
 * Purpose:
 *      Get reset of Cortex-M0 in Iproc subsystem quad
 * Parameters:
 *      unit number
 *      ucore number
 *      reset Soft reset value.
 * Returns:
 *      SOC_E_xxx
 */
int
soc_iproc_m0ssq_reset_ucore_get(int unit, int ucore, int *reset)
{
    uint32 val = 0;

    /* Parameter check. */
    if (reset == NULL) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    if (ucore >= MAX_UCORES || !SOC_REG_IS_VALID(unit, control_reg[ucore])) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "ucore %d doesn't exist.\n"), ucore));
        return SOC_E_PARAM;
    }

    /* Get reset bit. */
    soc_iproc_getreg(unit, soc_reg_addr(unit, control_reg[ucore], REG_PORT_ANY, 0), &val);
    *reset = soc_reg_field_get(unit, control_reg[ucore], val, SOFT_RESETf);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_m0ssq_reset
 * Purpose:
 *      Reset Iproc Cortex-M0 subsystem quad
 * Parameters:
 *      unit number
 *      reset Soft reset value.
 * Returns:
 *      None
 */
void soc_iproc_m0ssq_reset(int unit, int reset)
{
    int ucnum;

    for(ucnum = 0; ucnum < MAX_UCORES; ucnum++) {
#ifdef BCM_CMICX_SUPPORT
        if (sal_boot_flags_get() & BOOT_F_WARM_BOOT) {
            if (soc_feature(unit, soc_feature_cmicx) && !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
                uint32 addr, eapps;
                /* Check if eApps by verifying "0xbadc0de1" @ 0xc0 */
                addr = soc_iproc_percore_membase_get(unit, ucnum);
                eapps = soc_cm_iproc_read(unit, (addr + 0xC0));
                if (eapps == 0xbadc0de1) {
                    continue;
                }
            }
        }
#endif

        /* uc2 is being used for PCIe Firmware loader,
           exclude it from reset */
        if (ucnum != 2) {
            (void)soc_iproc_m0ssq_reset_ucore(unit, ucnum, reset);
        }
    }
}

/*
 * Function:
 *     iproc_m0ssq_uc_fw_dump
 * Purpose:
 *     Dump fimrware on console.
 * Parameters:
 *     unit Unit number
 *     offset Starting pointer of firmware downloading.
 *     data Firmware array.
 *     len Length in bytes.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_iproc_m0ssq_uc_fw_dump(int unit, int uc, int offset, int len)
{
    uint32 addr, data32;
    uint8 *data8 = (uint8 *) &data32;
    int wlen = (len + 3) & 0xFFFFFFFC;
    int i;

    addr = soc_iproc_percore_membase_get(unit, uc);
    addr += offset;

    for (i = 0; i < wlen; i += 4) {

        soc_iproc_getreg(unit, addr + i, &data32);

        if ((i % 16) == 0) {
            LOG_CLI((BSL_META_U(unit, "%04X: "), addr + i));
        }

        LOG_CLI((BSL_META_U(unit, "%02X %02X %02X %02X "), data8[3], data8[2], data8[1], data8[0]));

        if ((i % 16) == 12) {
            LOG_CLI((BSL_META_U(unit, "\n")));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     iproc_m0ssq_uc_fw_load
 * Purpose:
 *     Purpose
 * Parameters:
 *     unit Unit number
 *     offset Starting pointer of firmware downloading.
 *     data Firmware array.
 *     len Length in bytes.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_iproc_m0ssq_uc_fw_load(int unit, int uc, int offset, const uint8 *data, int len)
{
    uint32 addr, data32;
    int i, eapps;
    int wlen = (len + 3) & 0xFFFFFFFC;

    /* Check if eApps by verifying "0xbadc0de1" @ 0xc0 */
    eapps = (data[0xc3] == 0xba) && (data[0xc2] == 0xdc) &&
            (data[0xc1] == 0x0d) && (data[0xc0] == 0xe1);

    if (eapps) {
        (void)soc_iproc_m0ssq_reset_ucore(unit, uc, 1);
    }

    /* Pass device id to M0 firmware. */
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        soc_m0ssq_tcam_write32(unit, uc, DEVID_ID_OFFSET, CMDEV(unit).dev.dev_id);
    }

    addr = soc_iproc_percore_membase_get(unit, uc);
    addr += offset;

    for (i = 0; i < wlen; i += 4) {
        data32 = (data[i + 3] << 24) | (data[i + 2] << 16) | (data[i + 1] << 8) | data[i];
        soc_iproc_setreg(unit, addr + i, data32);
    }

    if (eapps) {
        addr = soc_iproc_percore_membase_get(unit, uc);
        soc_iproc_setreg(unit, addr + 0xc4, uc);
        (void)soc_iproc_m0ssq_reset_ucore(unit, uc, 0);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_quad_event_thread
 * Purpose:
 *      Thread to handle iproc Cortex-M0 subsystem events
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
void soc_iproc_quad_event_thread(void *unit_vp)
{
    int unit = M0SSQ_DECODE_UNIT(unit_vp);
    int ucnum = M0SSQ_DECODE_UCORE(unit_vp);
    soc_control_t       *soc = SOC_CONTROL(unit);
    soc_iproc_m0ssq_control_t *iproc_m0ssq = &soc->iproc_m0ssq_ctrl[ucnum];

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit))
    {
        DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_ADD(unit, DNX_ERR_RECOVERY_INTERNAL_THREAD_IPROC_EVENT);
    }
#endif

    while (iproc_m0ssq->thread_interval != 0) {
        LOG_DEBUG(BSL_LS_SOC_M0, (BSL_META_U(unit,"soc_iproc_quad_event_thread: sleep %d\n"),
                                                iproc_m0ssq->thread_interval));

        (void) sal_sem_take(iproc_m0ssq->event_sema, sal_sem_FOREVER);
        soc_cmic_intr_enable(unit, SW_PROG_INTR);

        /* Interrupt Handler */
        soc_iproc_msgintr_handler(unit, NULL);
    }

    sal_sem_destroy(iproc_m0ssq->event_sema);
    iproc_m0ssq->thread_interval = 0;
    iproc_m0ssq->thread_pid = NULL;

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit))
    {
        DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_REMOVE(unit, DNX_ERR_RECOVERY_INTERNAL_THREAD_IPROC_EVENT);
    }
#endif

    sal_thread_exit(0);
}

/*
 * Function:
 *      soc_iproc_m0ssq_init
 * Purpose:
 *      Initialize iproc Cortex-M0 subsystem
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
int soc_iproc_m0ssq_init(int unit)
{
   int rv = SOC_E_NONE;
   soc_control_t       *soc = SOC_CONTROL(unit);
   soc_iproc_m0ssq_control_t *iproc_m0ssq = NULL;
   uint32 interval = IPROC_M0SSQ_THREAD_INTERVAL;
   uint32 max_ucores, ucnum;

   if (iproc_m0ssq_init_done[unit]) {
       return rv;
   }

   /* Init max number of ucores enabled */
   _soc_iproc_fw_config(unit);

   /* Init linkscan firmware config. */

   /* Get max ucores enabled */
   max_ucores = _soc_iproc_num_ucore_get(unit);

   for (ucnum = 0; ucnum < max_ucores; ucnum++) {
       iproc_m0ssq = &soc->iproc_m0ssq_ctrl[ucnum];

       iproc_m0ssq->event_sema = sal_sem_create("m0ssq_intr", sal_sem_BINARY, 0);
       if (iproc_m0ssq->event_sema == NULL) {
           LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc M0SSQ event sem create failed\n")));
           return SOC_E_MEMORY;
       }

       sal_snprintf(iproc_m0ssq->thread_name, sizeof(iproc_m0ssq->thread_name),
                "IPROC_M0SSQ_EVENT.%d", unit);
       iproc_m0ssq->thread_interval = interval;
       iproc_m0ssq->thread_pid = sal_thread_create(iproc_m0ssq->thread_name,
                                                  SAL_THREAD_STKSZ,
                                                  soc_property_get(unit,
                                                                   spn_LINKSCAN_THREAD_PRI,
                                                                   IPROC_M0SSQ_THREAD_PRI),
                                                  (void (*)(void*))soc_iproc_quad_event_thread,
                                                  M0SSQ_ENCODE_UNIT_UCORE(unit, ucnum));

       if (iproc_m0ssq->thread_pid == SAL_THREAD_ERROR) {
           iproc_m0ssq->thread_interval = 0;
           sal_sem_destroy(iproc_m0ssq->event_sema);
           LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc M0SSQ thread creation failed \n")));
           return SOC_E_MEMORY;
       }
   }

   soc_cmic_intr_enable(unit, SW_PROG_INTR);

   iproc_m0ssq_init_done[unit] = 1;

   return rv;
}

/*
 * Function:
 *      soc_iproc_m0ssq_exit
 * Purpose:
 *      Exit iproc Cortex-M0 subsystem
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
int soc_iproc_m0ssq_exit(int unit)
{
    int rv = SOC_E_NONE;
    soc_control_t       *soc = SOC_CONTROL(unit);
    soc_iproc_m0ssq_control_t *iproc_m0ssq;
    uint32 ucnum;

    if (!iproc_m0ssq_init_done[unit]) {
        return rv;
    }

    for (ucnum = 0; ucnum < MAX_UCORES; ucnum++) {
        iproc_m0ssq = &soc->iproc_m0ssq_ctrl[ucnum];

        iproc_m0ssq->thread_interval = 0;

        if (iproc_m0ssq->event_sema) {
            sal_sem_give(iproc_m0ssq->event_sema);
        }
    }

    soc_cmic_intr_disable(unit, SW_PROG_INTR);

    iproc_m0ssq_init_done[unit] = 0;

    return rv;
}

/*
 * Function:
 *      soc_iproc_m0_init
 * Purpose:
 *      Initialize iproc Cortex-M0 subsystem and mbox
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
int soc_iproc_m0_init(int unit)
{
    int rv = SOC_E_NONE;

    if (SOC_CONTROL(unit)->iproc_m0_init_done) {
        return rv;
    }

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "IPROC M0 init\n")));

    /* Initialize Iproc ARM Cortex-M0 subsystem quad */
    rv = soc_iproc_m0ssq_init(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc M0SSQ init failed\n")));
        return rv;
    }

    /* Initialize Iproc ARM Cortex-M0 subsystem quad(M0SSQ) mailbox */
    rv = soc_iproc_mbox_init(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc mbox init failed\n")));
        return rv;
    }

    /* Initialize LED mailbox */
    rv = soc_cmicx_led_init(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "LED mbox init failed\n")));
        return rv;
    }

    rv = soc_cmicx_link_fw_config_init(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "LED mbox init failed\n")));
        return rv;
    }

    /* bring M0 core out of reset */
    /* uC0 for Linkscan/LED */
    (void)soc_iproc_m0ssq_reset_ucore(unit, 0, 0);

    SOC_CONTROL(unit)->iproc_m0_init_done = 1;

    return rv;
}

/*
 * Function:
 *      soc_iproc_m0_exit
 * Purpose:
 *      Exit iproc Cortex-M0 subsystem and mbox
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
int soc_iproc_m0_exit(int unit)
{
    int rv = SOC_E_NONE;
    int ucnum;

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "IPROC M0 exit\n")));

    if (!SOC_CONTROL(unit)->iproc_m0_init_done) {
        return rv;
    }

    /* Call LED deinit */
    rv = soc_cmicx_led_deinit(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "cmicx led deinit failed\n")));
        return rv;
    }

    /* Call Linkscan deinit */
    rv = soc_cmicx_linkscan_hw_deinit(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "cmicx linkscan deinit failed\n")));
        return rv;
    }

    /* Cleanup Iproc ARM Cortex-M0 subsystem quad(M0SSQ) mailbox */
    rv = soc_iproc_mbox_exit(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc mbox exit failed\n")));
        return rv;
    }

    /* Cleanup Iproc ARM Cortex-M0 subsystem quad */
    rv = soc_iproc_m0ssq_exit(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc M0SSQ exit failed\n")));
        return rv;
    }

    for(ucnum = 0; ucnum < MAX_UCORES; ucnum++) {
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx) && !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
            uint32 addr, eapps;

            /* Check if eApps by verifying "0xbadc0de1" @ 0xc0 */
            addr = soc_iproc_percore_membase_get(unit, ucnum);
            eapps = soc_cm_iproc_read(unit, (addr + 0xC0));
            if (eapps == 0xbadc0de1) {
                continue;
            }
        }
#endif

        /* uc2 is being used for PCIe Firmware loader,
           exclude it from reset */
        if (ucnum != 2) {
            (void)soc_iproc_m0ssq_reset_ucore(unit, ucnum, M0SSQ_RESET_ENABLE);
        }
    }

    SOC_CONTROL(unit)->iproc_m0_init_done = 0;

    return rv;
}

/*
 * Function:
 *      soc_iproc_m0ssq_shmem_get
 * Purpose:
 *      Get shared memory object by name.
 * Parameters:
 *      unit Device unit number.
 *      name Name of memory object.
 *      shmem Shared memory object.
 * Returns:
 *      SOC_E_NONE if success, otherwise return SOC_E_XXX.
 */
int
soc_iproc_m0ssq_shmem_get(int unit, char *name, soc_iproc_m0ssq_shmem_t **pshmem)
{
    if (pshmem == NULL) {
        return SOC_E_PARAM;
    }

    if (sal_strncmp(name, "linkscan", 8) == 0) {

        /* Static allocation of linkscan shmem. */
        *pshmem = sal_alloc(sizeof(soc_iproc_m0ssq_shmem_t), "SocIprocM0ssqShmem");
        if (*pshmem == NULL) {
            return SOC_E_MEMORY;
        }

        (*pshmem)->size = CMICX_LS_SHMEM_SIZE;
        (*pshmem)->base = soc_iproc_percore_membase_get(unit, 0) +
                          soc_iproc_percore_memsize_get(unit, 0) -
                          CMICX_LS_SHMEM_OFFSET_DEFAULT;
        (*pshmem)->unit = unit;

        return SOC_E_NONE;
    } else if (strncmp(name, "led", 3) == 0) {

        /* Static allocation of led shmem. */
        *pshmem = sal_alloc(sizeof(soc_iproc_m0ssq_shmem_t), "SocIprocLedM0ssqShmem");
        if (*pshmem == NULL) {
            return SOC_E_MEMORY;
        }

        (*pshmem)->size = CMICX_LED_SHMEM_SIZE;
        (*pshmem)->base = soc_iproc_percore_membase_get(unit, 0) +
                          soc_iproc_percore_memsize_get(unit, 0) -
                          CMICX_LED_SHMEM_OFFSET_DEFAULT;
        (*pshmem)->unit = unit;

        return SOC_E_NONE;
    }


    return SOC_E_FAIL;
}

/*
 * Function:
 *      soc_iproc_m0ssq_shmem_write32
 * Purpose:
 *      Write 32bits data into shared memory.
 * Parameters:
 *      shmem Shared memory object.
 *      offset Offset within memory object.
 *      value 32bits data.
 * Returns:
 *      SOC_E_NONE if success, otherwise return SOC_E_XXX.
 */
int
soc_iproc_m0ssq_shmem_write32(soc_iproc_m0ssq_shmem_t *shmem,
                              uint32 offset, uint32 value)
{
    uint32 addr;

    if (shmem == NULL) {
        return SOC_E_UNAVAIL;
    }

    if (offset >= shmem->size) {
        return SOC_E_PARAM;
    }

    addr = shmem->base + offset;
    soc_iproc_setreg(shmem->unit, addr, value);

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_iproc_m0ssq_shmem_read32
 * Purpose:
 *      Read 32bits data from shared memory.
 * Parameters:
 *      shmem Shared memory object.
 *      offset Offset within memory object.
 *      value 32bits data.
 * Returns:
 *      SOC_E_NONE if success, otherwise return SOC_E_XXX.
 */
int
soc_iproc_m0ssq_shmem_read32(soc_iproc_m0ssq_shmem_t *shmem,
                             uint32 offset, uint32 *value)
{
    uint32 addr;

    if (shmem == NULL ||
        value == NULL)
    {
        return SOC_E_UNAVAIL;
    }

    if (offset >= shmem->size) {
        return SOC_E_PARAM;
    }

    addr = shmem->base + offset;

    soc_iproc_getreg(shmem->unit, addr, value);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_m0ssq_shmem_clear
 * Purpose:
 *      Clear shared memory object as zero.
 * Parameters:
 *      shmem Shared memory object.
 * Returns:
 *      SOC_E_NONE if success, otherwise return SOC_E_XXX.
 */
int
soc_iproc_m0ssq_shmem_clear(soc_iproc_m0ssq_shmem_t *pshmem)
{
    int rv;
    uint32 addr;
    for (addr = 0; addr < pshmem->size; addr += 4) {
        rv = soc_iproc_m0ssq_shmem_write32(pshmem, addr, 0);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
    }
    return SOC_E_NONE;
}

#endif /* BCM_CMICX_SUPPORT */
#endif /* BCM_ESW_SUPPORT */

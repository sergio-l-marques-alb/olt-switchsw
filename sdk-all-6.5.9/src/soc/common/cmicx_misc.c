/*
 * $Id: cmix_misc.c,v 1.00 Broadcom SDK $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * CMICx miscllaneous functions
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <sal/core/sync.h>
#include <soc/util.h>
#include <soc/cmic.h>

#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#include <soc/iproc.h>
#endif

/* PCIe capabilities */
#ifndef PCI_CAPABILITY_LIST
#define PCI_CAPABILITY_LIST     0x34
#endif
#ifndef PCI_CAP_ID_EXP
#define PCI_CAP_ID_EXP          0x10
#endif
#ifndef PCI_EXP_DEVCAP
#define PCI_EXP_DEVCAP          4
#endif
#ifndef PCI_EXP_DEVCTL
#define PCI_EXP_DEVCTL          8
#endif
#ifndef PCI_EXT_CAP_START
#define PCI_EXT_CAP_START       0x100
#endif
#ifndef PCI_EXT_CAP_ID
#define PCI_EXT_CAP_ID(_hdr)    (_hdr & 0x0000ffff)
#endif
#ifndef PCI_EXT_CAP_VER
#define PCI_EXT_CAP_VER(_hdr)   ((_hdr >> 16) & 0xf)
#endif
#ifndef PCI_EXT_CAP_NEXT
#define PCI_EXT_CAP_NEXT(_hdr)  ((_hdr >> 20) & 0xffc)
#endif
#ifndef PCI_EXT_CAP_ID_VNDR
#define PCI_EXT_CAP_ID_VNDR     0x0b
#endif


#ifdef BCM_CMICX_SUPPORT
/**************************************************
* @function soc_is_cmicx(unit)
* purpose API to test if soc has cmicx
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
**************************************************/
int soc_is_cmicx(int unit)
{
    unsigned int cap_base, rval;

    /* Look for PCIe vendor-specific extended capability (VSEC) */
    cap_base = PCI_EXT_CAP_START;
    while (cap_base) {
        rval = soc_pci_conf_read(unit, cap_base);
        if (rval == 0xffffffff) {
           /* Assume PCI HW read error */
           return 0;
        }

        if (PCI_EXT_CAP_ID(rval) == PCI_EXT_CAP_ID_VNDR) {
            break;
        }
        cap_base = PCI_EXT_CAP_NEXT(rval);
    }
    if (cap_base) {
        /*
         * VSEC layout:
         *
         * 0x00: PCI Express Extended Capability Header
         * 0x04: Vendor-Specific Header
         * 0x08: Vendor-Specific Register 1
         * 0x0c: Vendor-Specific Register 2
         *     ...
         * 0x24: Vendor-Specific Register 8
         */
         /* Read PCIe Vendor Specific Register 1 */
         /* VENODR REG FORMAT
          * [7:0] iProc Rev = 8'h0E (for P14)
          * [11:8] CMIC BAR = 4'h1 (BAR64-1)
          * [15:12] CMIC Version = 4'h4
          * [19:16] CMIC Rev = 4'h1
          * [22:20] SBUS Version = 4'h4
          */

        rval = soc_pci_conf_read(unit, cap_base + 8);

        if (((rval >> 12) & 0xf) == 0x4) {
            return 1;
        }
    }

    return 0;

}

/**************************************************
* @function soc_cmicx_pci_test
* purpose API to test PCI access to cmicx registers
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
**************************************************/
int soc_cmicx_pci_test(int unit)
{
    int i;
    uint32 tmp, reread;
    uint32 pat;

    SCHAN_LOCK(unit);

    /* Check for address uniqueness */

        for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
            pat = 0x55555555 ^ (i << 24 | i << 16 | i << 8 | i);
            soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(0, i), pat);
        }

        for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
            pat = 0x55555555 ^ (i << 24 | i << 16 | i << 8 | i);
            tmp = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(0, i));
            if (tmp != pat) {
                goto error;
            }
        }

    if (!SAL_BOOT_QUICKTURN) {  /* Takes too long */
        /* Rotate walking zero/one pattern through each register */

        pat = 0xff7f0080;       /* Simultaneous walking 0 and 1 */

        for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
            int j;

            for (j = 0; j < 32; j++) {
                    soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(0, i), pat);
                    tmp = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(0, i));
                if (tmp != pat) {
                    goto error;
                }
                pat = (pat << 1) | ((pat >> 31) & 1);	/* Rotate left */
            }
        }
    }

    /* Clear to zeroes when done */

    for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
            soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(0, i), 0);
    }
    SCHAN_UNLOCK(unit);
    return SOC_E_NONE;

 error:
    reread = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(0, i));

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "FATAL PCI error testing PCIM[0x%x]:\n"
                          "Wrote 0x%x, read 0x%x, re-read 0x%x\n"),
               i, pat, tmp, reread));

    SCHAN_UNLOCK(unit);
    return SOC_E_INTERNAL;
}

/**************************************************
* @function soc_cmicx_paxb_tx_arbiter_set
* purpose: API to set tx arbiter
*
* @param unit [in] unit
* @param unit [in] enable
* @param unit [in] priority
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
**************************************************/
int soc_cmicx_paxb_tx_arbiter_set(int unit, uint8 enable, uint8 priority)
{
    uint32 cmic_reg_val = 0;

    if (SOC_IS_TRIDENT3X(unit)) {
        soc_reg_field_set(unit, PAXB_0_PAXB_TX_ARBITER_PRIORITYr, &cmic_reg_val,
                          TX_REQ_SEQ_ENf, enable);
        soc_reg_field_set(unit, PAXB_0_PAXB_TX_ARBITER_PRIORITYr, &cmic_reg_val,
                          TX_ARB_PRIORITYf, priority);
        SOC_IF_ERROR_RETURN(WRITE_PAXB_0_PAXB_TX_ARBITER_PRIORITYr(unit, cmic_reg_val));
    }

    return SOC_E_NONE;
}

#endif /* BCM_CMICX_SUPPORT  */

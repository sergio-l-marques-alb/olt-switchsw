/*
 * $Id: $
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 */

#include <shbde_iproc.h>

/* PAXB register offsets within PCI BAR0 window */
#define BAR0_PAXB_ENDIANESS                     0x2030
#define BAR0_PAXB_PCIE_EP_AXI_CONFIG            0x2104
#define BAR0_PAXB_IMAP0_0                       0x2c00
#define BAR0_PAXB_IMAP0_1                       0x2c04
#define BAR0_PAXB_IMAP0_2                       0x2c08
#define BAR0_PAXB_IMAP0_7                       0x2c1c
#define BAR0_PAXB_OARR_FUNC0_MSI_PAGE           0x2d34
#define BAR0_PAXB_OARR_2                        0x2d60
#define BAR0_PAXB_OARR_2_UPPER                  0x2d64
#define BAR0_DMU_PCU_PCIE_SLAVE_RESET_MODE      0x7024

/* Force byte pointer for offset adjustments */
#define ROFFS(_ptr, _offset) ((unsigned char*)(_ptr) + (_offset))

static unsigned int
iproc32_read(shbde_hal_t *shbde, void *addr)
{
    if (!shbde || !shbde->io32_read) {
        return 0;
    }
    return shbde->io32_read(addr);
}

static void
iproc32_write(shbde_hal_t *shbde, void *addr, unsigned int data)
{
    if (!shbde || !shbde->io32_write) {
        return;
    }
    shbde->io32_write(addr, data);
}

/*
 * Function:
 *      shbde_iproc_config_init
 * Purpose:
 *      Initialize iProc configuration parameters
 * Parameters:
 *      icfg - pointer to empty iProc configuration structure
 * Returns:
 *      -1 if error, otherwise 0
 */
int
shbde_iproc_config_init(shbde_iproc_config_t *icfg,
                        unsigned int dev_id, unsigned int dev_rev)
{
    if (!icfg) {
        return -1;
    }

    /* Save device ID and revision */
    icfg->dev_id = dev_id;
    icfg->dev_rev = dev_rev;

    /* Check device families first */
    switch (icfg->dev_id & 0xfff0) {
    case 0x8400: /* Greyhound */
    case 0x8410: /* Greyhound */
    case 0x8420: /* Bloodhound */
    case 0x8450: /* Elkhound */
    case 0xb060: /* Greyhound */
        icfg->iproc_ver = 7;
        icfg->dma_hi_bits = 0x2;
        break;
    default:
        break;
    }

    /* Check for exceptions */
    switch (icfg->dev_id) {
    default:
        break;
    }

    return 0;
}

/*
 * Function:
 *      shbde_iproc_paxb_init
 * Purpose:
 *      Initialize iProc PCI-AXI bridge for CMIC access
 * Parameters:
 *      shbde - pointer to initialized hardware abstraction module
 *      iproc_regs - memory mapped iProc registers in PCI BAR
 *      icfg - iProc configuration parameters
 * Returns:
 *      -1 if error, otherwise 0
 */
int
shbde_iproc_paxb_init(shbde_hal_t *shbde, void *iproc_regs,
                      shbde_iproc_config_t *icfg)
{
    void *reg;
    unsigned int data;
    int pci_num;

    if (!iproc_regs || !icfg) {
        return -1;
    }

    /*
     * The following code attempts to auto-detect the correct
     * iProc PCI endianess configuration by reading a well-known
     * register (the endianess configuration register itself).
     * Note that the PCI endianess may be different for different
     * big endian host processors.
     */
    reg = ROFFS(iproc_regs, BAR0_PAXB_ENDIANESS);
    /* Select big endian */
    iproc32_write(shbde, reg, 0x01010101);
    /* Check if endianess register itself is correct endian */
    if (iproc32_read(shbde, reg) != 1) {
        /* If not, then assume little endian */
        iproc32_write(shbde, reg, 0x0);
    }

    /* Select which PCI core to use */
    pci_num = 0;
    reg = ROFFS(iproc_regs, BAR0_PAXB_IMAP0_2);
    data = iproc32_read(shbde, reg);
    if (data  & 0x1000) {
        /* PAXB_1 is mapped to sub-window 2 */
        pci_num = 1;
    }

    /* Default DMA mapping if uninitialized */
    if (icfg->dma_hi_bits == 0) {
        icfg->dma_hi_bits = 0x1;
        if (pci_num == 1) {
            icfg->dma_hi_bits = 0x2;
        }
    }

    /* Enable iProc DMA to external host memory */
    reg = ROFFS(iproc_regs, BAR0_PAXB_PCIE_EP_AXI_CONFIG);
    iproc32_write(shbde, reg, 0x0);
    reg = ROFFS(iproc_regs, BAR0_PAXB_OARR_2);
    iproc32_write(shbde, reg, 0x1);
    reg = ROFFS(iproc_regs, BAR0_PAXB_OARR_2_UPPER);
    iproc32_write(shbde, reg, icfg->dma_hi_bits);

    /* Configure MSI interrupt page */
    if (icfg->use_msi) {
        reg = ROFFS(iproc_regs, BAR0_PAXB_OARR_FUNC0_MSI_PAGE);
        data = iproc32_read(shbde, reg);
        iproc32_write(shbde, reg, data | 0x1);
    }

    return pci_num;
}

/*
 * Function:
 *      shbde_iproc_pci_read
 * Purpose:
 *      Read iProc register through PCI BAR 0
 * Parameters:
 *      shbde - pointer to initialized hardware abstraction module
 *      iproc_regs - memory mapped iProc registers in PCI BAR
 *      addr - iProc register address in AXI memory space
 * Returns:
 *      Register value
 */
unsigned int
shbde_iproc_pci_read(shbde_hal_t *shbde, void *iproc_regs,
                     unsigned int addr)
{
    unsigned int subwin_base;
    void *reg;

    if (!iproc_regs) {
        return -1;
    }

    /* Sub-window size is 0x1000 (4K) */
    subwin_base = (addr & ~0xfff) | 0x1;

    /* Update base address for sub-window 7 */
    reg = ROFFS(iproc_regs, BAR0_PAXB_IMAP0_7);
    iproc32_write(shbde, reg, subwin_base);

    /* Read register through sub-window 7 */
    reg = ROFFS(iproc_regs, 0x7000 + (addr & 0xfff));
    return iproc32_read(shbde, reg);
}

/*
 * Function:
 *      shbde_iproc_pci_write
 * Purpose:
 *      Write iProc register through PCI BAR 0
 * Parameters:
 *      shbde - pointer to initialized hardware abstraction module
 *      iproc_regs - memory mapped iProc registers in PCI BAR
 *      addr - iProc register address in AXI memory space
 *      data - data to write to iProc register
 * Returns:
 *      Register value
 */
void
shbde_iproc_pci_write(shbde_hal_t *shbde, void *iproc_regs,
                      unsigned int addr, unsigned int data)
{
    unsigned int subwin_base;
    void *reg;

    if (!iproc_regs) {
        return;
    }

    /* Sub-window size is 0x1000 (4K) */
    subwin_base = (addr & ~0xfff) | 0x1;

    /* Update base address for sub-window 7 */
    reg = ROFFS(iproc_regs, BAR0_PAXB_IMAP0_7);
    iproc32_write(shbde, reg, subwin_base);

    /* Write register through sub-window 7 */
    reg = ROFFS(iproc_regs, 0x7000 + (addr & 0xfff));
    iproc32_write(shbde, reg, data);
}

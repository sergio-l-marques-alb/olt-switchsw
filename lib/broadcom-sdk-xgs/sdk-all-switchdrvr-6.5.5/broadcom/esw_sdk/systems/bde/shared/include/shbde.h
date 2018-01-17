/*
 * $Id: $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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

#ifndef __SHBDE_H__
#define __SHBDE_H__

typedef void (*shbde_log_func_t)(int level, const char *str, int param);

#define SHBDE_ERR       0
#define SHBDE_WARN      1
#define SHBDE_DBG       2

/* Hardware abstraction functions */
typedef struct shbde_hal_s {

    /* Optional log output interface */
    shbde_log_func_t log_func;

    /* PCI configuration access */
    unsigned char (*pcic8_read)(void *pci_dev, unsigned int reg);
    void (*pcic8_write)(void *pci_dev, unsigned int reg, unsigned char data);
    unsigned short (*pcic16_read)(void *pci_dev, unsigned int reg);
    void (*pcic16_write)(void *pci_dev, unsigned int reg, unsigned short data);
    unsigned int (*pcic32_read)(void *pci_dev, unsigned int reg);
    void (*pcic32_write)(void *pci_dev, unsigned int reg, unsigned int data);

    /* iProc register access */
    unsigned int (*io32_read)(void *addr);
    void (*io32_write)(void *addr, unsigned int);

    /* usleep function (optional) */
    void (*usleep)(int usec);

    /* PCI parent device access */
    void *(*pci_parent_device_get)(void *pci_dev);

} shbde_hal_t;

/* iProc configuration (primarily used for PCI-AXI bridge) */
typedef struct shbde_iproc_config_s {
    unsigned int dev_id;
    unsigned int dev_rev;
    unsigned int use_msi;
    unsigned int iproc_ver;
    unsigned int dma_hi_bits;
    unsigned int mdio_base_addr;
    unsigned int pcie_phy_addr;
    unsigned int adjust_pcie_preemphasis;
} shbde_iproc_config_t;

#endif /* __SHBDE_H__ */

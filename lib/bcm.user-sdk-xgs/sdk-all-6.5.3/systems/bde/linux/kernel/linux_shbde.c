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

#include <gmodule.h>
#include <shbde.h>
#include <shbde_iproc.h>
#include "linux_shbde.h"

/* Hardware abstractions for shared BDE functions */

static unsigned short
linux_pcic16_read(void *pci_dev, unsigned int addr)
{
    u16 data = 0;

    pci_read_config_word((struct pci_dev *)pci_dev, addr, &data);

    return data;
}

static void
linux_pcic16_write(void *pci_dev, unsigned int addr, unsigned short data)
{
    pci_write_config_word((struct pci_dev *)pci_dev, addr, (u16)data);
}

static unsigned int
linux_pcic32_read(void *pci_dev, unsigned int addr)
{
    u32 data = 0;

    pci_read_config_dword((struct pci_dev *)pci_dev, addr, &data);

    return data;
}

static void
linux_pcic32_write(void *pci_dev, unsigned int addr, unsigned int data)
{
    pci_write_config_dword((struct pci_dev *)pci_dev, addr, (u32)data);
}

static unsigned int
linux_io32_read(void *addr)
{
    return *((volatile u32 *)addr);
}

static void
linux_io32_write(void *addr, unsigned int data)
{
    *((volatile u32 *)addr) = data;
}

static void
linux_usleep(int usec)
{
    udelay(usec);
}


/* To get the PCI parent device under linux, from only the device pointer */
static void *
linux_pci_parent_device_get(void *pci_dev)
{
    return (void *)(((struct pci_dev *)pci_dev)->bus->self);
}


/*
 * Function:
 *      linux_shbde_hal_init
 * Purpose:
 *      Initialize hardware abstraction module for Linux kernel.
 * Parameters:
 *      shbde - pointer to uninitialized hardware abstraction module
 *      log_func - optional log output function
 * Returns:
 *      Always 0
 */
int
linux_shbde_hal_init(shbde_hal_t *shbde, shbde_log_func_t log_func)
{
    memset(shbde, 0, sizeof(*shbde));

    shbde->log_func = log_func;

    shbde->pcic16_read = linux_pcic16_read;
    shbde->pcic16_write = linux_pcic16_write;
    shbde->pcic32_read = linux_pcic32_read;
    shbde->pcic32_write = linux_pcic32_write;

    shbde->io32_read = linux_io32_read;
    shbde->io32_write = linux_io32_write;

    shbde->usleep = linux_usleep;

    shbde->pci_parent_device_get = linux_pci_parent_device_get;

    return 0;
}

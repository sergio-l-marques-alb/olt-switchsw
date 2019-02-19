/*
 * $Id: pci.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
 * $Copyright: Copyright 2008 Broadcom Corporation.
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
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * PCI memory and configuration space routines.
 *
 * NOTE: the base address used in this file is opaque. This means that
 * the PCI device must first have its base-address programmed by
 * writing the start offset to PCI configuration offset 0x10.
 * Writes to the memory space afterwards requires the address to be
 * based on the base-address plus the offset to the desire register
 * to be accessed.
 */

#include <cyg/kernel/kapi.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/infra/diag.h>

#ifndef BCM_ICS 
#include <cyg/io/pci.h>
#endif

#include <sal/appl/pci.h>
#include <sal/core/alloc.h>


#include <soc/debug.h>
#include <soc/drv.h>
#include <sal/appl/io.h>

#ifdef BCM_ICS

#define SBIPSFLAG_ADDR     0xb8000100
#define DEF_SBIPSFLAG      0x03020100  /* 1=uart0, 2=uart1 and 3=cmic 4=gpio */
#define NOCMIC_SBIPSFLAG   0x033F0100  /* 1=uart0, 2=uart1 and 3=XXX 4=gpio */

int pci_config_putw(pci_dev_t *dev, uint32 addr, uint32 data)
{
    return 0xffffffff;
}
uint32 pci_config_getw(pci_dev_t *dev, uint32 addr)
{
    return 0xffffffff;
}
#else

#define MASK_PCI_INTA	*(volatile uint32 *)0xb8004024 = 0
#define UNMASK_PCI_INTA *(volatile uint32 *)0xb8004024 = 1

/*
 * Write a DWORD (32 bits) of data to PCI configuration space
 * at the specified offset.
 */

int pci_config_putw(pci_dev_t *dev, uint32 addr, uint32 data)
{
    cyg_pci_device_id devid;
	
    devid = (dev->busNo << 16) | (((dev->devNo <<3) | dev->funcNo) << 8);
    cyg_pci_write_config_uint32(devid, addr, data);
	
    return 0;    
}

/*
 * Read a DWORD (32 bits) of data from PCI configuration space
 * at the specified offset.
 */

uint32 pci_config_getw(pci_dev_t *dev, uint32 addr)
{
    cyg_pci_device_id devid;
    uint32 val;
	
    devid = (dev->busNo << 16) | (((dev->devNo <<3) | dev->funcNo) << 8);
    cyg_pci_read_config_uint32(devid, addr, &val);
    return val;
}
#endif

/*
 * Write a DWORD (32 bits) of data to PCI memory space
 * at the specified offset. Return last value at address.
 * NOTE: Deprecated routine
 */

int pci_memory_putw(pci_dev_t *dev, uint32 addr, uint32 data)
{
    *(uint32 *)INT_TO_PTR(addr) = data;
    return 0;
}

/*
 * Read a DWORD (32 bits) of data from PCI memory space.
 * NOTE: Deprecated routine
 */

uint32 pci_memory_getw(pci_dev_t *dev, uint32 addr)
{
    return *(uint32 *)INT_TO_PTR(addr);
}

/*
 * DMA memory routines.
 * The routines below operate on system memory which the CMIC onboard the
 * Orion will read and write.
 */

int pci_dma_putw(pci_dev_t *dev, uint32 addr, uint32 data)
{
    *(uint32 *)INT_TO_PTR(addr) = data;
    return 0;
}

int pci_dma_puth(pci_dev_t *dev, uint32 addr, uint16 data)
{
    *(uint16 *)INT_TO_PTR(addr) = data;
    return 0;
}

int pci_dma_putb(pci_dev_t *dev, uint32 addr, uint8 data)
{
    *(uint8 *)INT_TO_PTR(addr) = data;
    return 0;
}

uint32 pci_dma_getw(pci_dev_t *dev, uint32 addr)
{
    return *(uint32 *)INT_TO_PTR(addr);
}

uint16 pci_dma_geth(pci_dev_t *dev, uint32 addr)
{
    return *(uint16 *)INT_TO_PTR(addr);
}

uint8 pci_dma_getb(pci_dev_t *dev, uint32 addr)
{
    return *(uint8 *)INT_TO_PTR(addr);
}

/* Control Data */
typedef struct ecos_ctrl_s {
/* Interrupt Handling */
    void (*isr)(void *);
    void *isr_data;
    struct ecos_ctrl_s *next;
} ecos_ctrl_t;

static ecos_ctrl_t *soc_ctrl_head = NULL;

static void
pci_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    ecos_ctrl_t *m;	
    
    for(m = soc_ctrl_head; m; m = m->next) {
        if (m->isr) {
            m->isr(m->isr_data);
        } 
    }
#ifdef BCM_ICS
    cyg_interrupt_unmask(vector);
    *((volatile unsigned long*)(SBIPSFLAG_ADDR)) = DEF_SBIPSFLAG;
#else
    UNMASK_PCI_INTA; // enable mask
#endif
}

// This ISR is called when the ethernet interrupt occurs
static cyg_uint32
pci_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    //CYG_INTERRUPT_STATE old_ints;
    //HAL_DISABLE_INTERRUPTS(old_ints);
    
	   cyg_drv_interrupt_acknowledge(vector);
#ifdef BCM_ICS
    cyg_interrupt_mask(vector);
    /* disable CMIC interrupt */
    *((volatile unsigned long*)(SBIPSFLAG_ADDR)) = NOCMIC_SBIPSFLAG;
#else    
    MASK_PCI_INTA; // clear mask
#endif
    //HAL_RESTORE_INTERRUPTS(old_ints);
    return CYG_ISR_CALL_DSR;
}

/*
 * pci_int_connect
 *
 *   Adds an interrupt service routine to the ISR chain for a
 *   specified interrupt line.
 */

int pci_int_connect(int intLine,
		    pci_isr_t isr,
		    void *isr_data)
{
    static cyg_handle_t interrupt_handle;
    static cyg_interrupt interrupt_object;
    ecos_ctrl_t *soc_ctrl, *m;
    
    soc_ctrl = (ecos_ctrl_t *)sal_alloc(sizeof(ecos_ctrl_t), "SAL_PCI");
    	
    soc_ctrl->isr = isr;
    soc_ctrl->isr_data = isr_data;
    soc_ctrl->next = NULL;
    
    /* ONLY attach the first interrupt handle */
    if(soc_ctrl_head == NULL) {
        cyg_drv_interrupt_create(
        intLine,
        0,
        PTR_TO_INT(soc_ctrl),
        pci_isr,
        pci_dsr,
        &interrupt_handle,
        &interrupt_object);
    
        cyg_drv_interrupt_attach(interrupt_handle);
        soc_ctrl_head = soc_ctrl;
        cyg_interrupt_unmask(intLine);
    } else {
       for(m = soc_ctrl_head; m; m = m->next) {
       	  if(m->next == NULL) {
              m->next = soc_ctrl;
              break;
          }
       }
	   }
    return 0;
}

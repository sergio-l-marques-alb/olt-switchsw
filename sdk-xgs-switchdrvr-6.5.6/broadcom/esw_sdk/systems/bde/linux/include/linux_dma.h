/***********************************************************************
 *
 * $Id: linux_dma.h,v 1.24 Broadcom SDK $
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
 **********************************************************************/

#ifndef __LINUX_DMA_H__
#define __LINUX_DMA_H__

#include <sal/types.h>

#ifdef __KERNEL__

#ifdef SAL_BDE_XLP
#define KMALLOC(size, flags)    __kmalloc(size, flags)
#else
#define KMALLOC(size, flags)    kmalloc(size, flags)
#endif

#if defined(CONFIG_IDT_79EB334) || defined(CONFIG_BCM4702)
/* ioremap is broken in kernel */
#define IOREMAP(addr, size) ((void *)KSEG1ADDR(addr))
#else
#define IOREMAP(addr, size) ioremap_nocache(addr, size)
#endif

#if defined (__mips__)
#if defined(CONFIG_NONCOHERENT_IO) || defined(CONFIG_DMA_NONCOHERENT)
/* Use flush/invalidate for cached memory */
#define NONCOHERENT_DMA_MEMORY
/* Remap virtual DMA addresses to non-cached segment */
#define REMAP_DMA_NONCACHED
#endif /* CONFIG_NONCOHERENT_IO || CONFIG_DMA_NONCOHERENT */
#endif /* __mips__ */

#if defined(BCM958525) && (LINUX_VERSION_CODE <= KERNEL_VERSION(3,6,5))
#define REMAP_DMA_NONCACHED
#endif

#ifndef DMA_BIT_MASK
#define DMA_BIT_MASK(n) (((n) == 64) ? ~0ULL : ((1ULL<<(n))-1))
#endif

extern void _dma_init(int robo_switch);
extern int _dma_cleanup(void);
extern void _dma_pprint(void);
extern uint32_t *_salloc(int d, int size, const char *name);
extern void _sfree(int d, void *ptr);
extern int _sinval(int d, void *ptr, int length);
extern int _sflush(int d, void *ptr, int length);
extern sal_paddr_t _l2p(int d, void *vaddr);
extern uint32_t *_p2l(int d, sal_paddr_t paddr);
extern int _dma_pool_allocated(void);
extern int _dma_range_valid(unsigned long phys_addr, unsigned long size);

#endif /* __KERNEL__ */

#endif /* __LINUX_DMA_H__ */

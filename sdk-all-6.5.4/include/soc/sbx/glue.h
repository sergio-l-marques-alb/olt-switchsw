/******************************************************************************
** ========================================================================
** == glue.h -  Lower level OS specific glue layer for Sandburst devices ==
** ========================================================================
**
** WORKING REVISION: $Id: glue.h,v 1.6 Broadcom SDK $
**
** $Copyright: Copyright 2016 Broadcom Corporation.
** This program is the proprietary software of Broadcom Corporation
** and/or its licensors, and may only be used, duplicated, modified
** or distributed pursuant to the terms and conditions of a separate,
** written license agreement executed between you and Broadcom
** (an "Authorized License").  Except as set forth in an Authorized
** License, Broadcom grants no license (express or implied), right
** to use, or waiver of any kind with respect to the Software, and
** Broadcom expressly reserves all rights in and to the Software
** and all intellectual property rights therein.  IF YOU HAVE
** NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
** IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
** ALL USE OF THE SOFTWARE.  
**  
** Except as expressly set forth in the Authorized License,
**  
** 1.     This program, including its structure, sequence and organization,
** constitutes the valuable trade secrets of Broadcom, and you shall use
** all reasonable efforts to protect the confidentiality thereof,
** and to use this information only in connection with your use of
** Broadcom integrated circuit products.
**  
** 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
** PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
** REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
** OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
** DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
** NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
** ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
** CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
** OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
** 
** 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
** BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
** INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
** ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
** TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
** THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
** WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
** ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
**
** MODULE NAME:
**
**     glue.h
**
** ABSTRACT:
**
**     Provide access to Sandburst chips.
**     All access is through opaque types.
**
** LANGUAGE:
**
**     C
**
** AUTHORS:
**
**     Lennart Augustsson
**
** CREATION DATE:
**
**     22-July-2004
**
******************************************************************************/
#ifndef _THINGLUE_H_
#define _THINGLUE_H_

#include <soc/sbx/sbTypesGlue.h>
#include <soc/sbx/glue_dma.h>


/*
 * The following function pointer definition allows the user
 * to run code without actually writing to or reading from the
 * chip, thereby 'dry-running'
 */
typedef uint32 (*thin_ack_reg_func_ptr_t)(uint32 ulOffset);

extern sbhandle thin_open(const char *device, thin_bus_t bt, sb_chip_t type );
extern void thin_close(sbhandle hdl);

extern sbhandle thin_map_registers(sbhandle hdl);
extern sbhandle thin_offset_handle(sbhandle addr, uint32 offs);
extern void thin_offset_handle_remove(sbhandle hdl);
extern int thin_unmap_registers(sbhandle hdl);

extern sbreg thin_cswap32(sbhandle addr, sbreg data);

extern sbreg thin_read32(sbhandle addr, uint32 offs);
extern void thin_write32(sbhandle addr, uint32 offs, sbreg data);

void
thin_no_write(sbhandle addr, uint32 offs, sbreg data);

extern sbreg thin_read32_raw(sbhandle addr, uint32 offs);
extern void thin_write32_raw(sbhandle addr, uint32 offs, sbreg data);

extern sbStatus_t thin_malloc(void *clientData, sbFeMallocType_t type, uint32 size,
	  void **memoryP, sbDmaMemoryHandle_t *dmaHandleP);

extern sbStatus_t
thin_contig_malloc(void *clientData, sbFeMallocType_t type, uint32 size,
	  void **memoryP, sbDmaMemoryHandle_t *dmaHandleP);

extern sbStatus_t
gen_thin_malloc(void *clientData, sbFeMallocType_t type, uint32 size,
          void **memoryP, sbDmaMemoryHandle_t *dmaHandleP, uint8 contig);

extern sbStatus_t
thin_free(void *clientData, sbFeMallocType_t type, uint32 size,
	void *memory, sbDmaMemoryHandle_t dmaHandle);

extern void*
thin_host_malloc(uint32 size);

extern void
thin_host_free(void *vp);

extern void thin_delay(uint32 nanosecs);

extern void *thin_alloc_dma_memory(sbhandle hdl, uint32 size,
			    uint32 **pages, uint32 *npages);
extern void *thin_alloc_contig_dma_memory(sbhandle hdl, uint32 size,
				   uint32 *pages);

extern int thin_get_fd(sbhandle hdl);
extern int thin_get_pagesize(sbhandle hdl);

extern int thin_intr_handler(sbhandle hdl, void (*f)(void *), void *data);
extern int thin_block_intr(sbhandle hdl);
extern void thin_unblock_intr(sbhandle hdl, int token);

extern int thin_dma_read_sync(sbhandle hdl, uint32 offs, uint32 len);
extern int thin_dma_write_sync(sbhandle hdl, uint32 offs, uint32 len);

extern void thin_enable_signal(sbhandle);
extern void thin_disable_signal(sbhandle);

extern uint32 thin_get_cb_num(sbhandle hdl);

extern sbStatus_t thin_sem_open(uint32 key, int* semId);
extern sbStatus_t thin_sem_close(int semId);
extern sbStatus_t thin_sem_get(int semId);
extern sbStatus_t thin_sem_put(int semId);
extern sbStatus_t thin_sem_wait_get(int semId, int timeOut);

extern sbStatus_t thin_dryrun_set(sbhandle hdl, thin_ack_reg_func_ptr_t pfDryRun);
extern sbStatus_t thin_verbose_set(sbhandle hdl, int verblvl);


#endif /* _THINGLUE_H_ */

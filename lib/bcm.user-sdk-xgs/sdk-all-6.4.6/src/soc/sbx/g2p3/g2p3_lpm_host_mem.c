/* -*- mode:c; c-style:k&r; c-basic-offset:2; indent-tabs-mode: nil; -*- */
/* vi:set expandtab cindent shiftwidth=2 cinoptions=\:0l1(0t0g0: */
/*
 * $Id: g2p3_lpm_host_mem.c,v 1.12 Broadcom SDK $
 *
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 */

#include <shared/bsl.h>

#include "g2p3_lpm_host_mem.h"
#include "g2p3_lpm_host_mem_internal.h"
#include "sbWrappers.h"
#include "soc/cm.h"


#define LPM_HOST_MEM_LOCK(handle) sal_mutex_take((handle)->mutex, sal_mutex_FOREVER)
#define LPM_HOST_MEM_UNLOCK(handle) sal_mutex_give((handle)->mutex)

#ifndef LPM_HOST_MEM_DEBUG
#define LPM_HOST_MEM_DEBUG 0
#endif


/* XXX might not be appropriate for some platforms or data */
#define BYTE_ALIGNMENT (sizeof(uint32))

/* requires: size >= 0
 * modifies: nothing
 * effects:  none
 * returns:  size rounded up to alignment
 */
static
size_t
alignSize(size_t size)
{
    size += sizeof(lpmHostMemBlock_t);
    return (size % BYTE_ALIGNMENT ?
            ((size / BYTE_ALIGNMENT) + 1) * BYTE_ALIGNMENT : size);
}

/* see g2p3_lpmHostMem.h for contract */
sbStatus_t
g2p3_lpmHostMemInit(g2p3_lpmHostMemHandle_t *pHandle,
                        size_t nbytes,
                        sbMalloc_f_t hostAlloc, void *hostAllocToken,
                        sbFree_f_t hostFree, void *hostFreeToken)
{
    sbStatus_t status;
    void *vp;
    g2p3_lpmHostMemHandle_t handle;
    sbDmaMemoryHandle_t bogus;

    SB_ASSERT(pHandle);
    SB_ASSERT(hostAlloc);

    if (nbytes < sizeof(g2p3_lpmHostMem_t))
        return SB_LPM_OUT_OF_HOST_MEMORY;
    status = hostAlloc(hostAllocToken, SB_ALLOC_INTERNAL, nbytes,
                       &vp, NULL);
    if (status != SB_OK)
        return status;
    handle = (g2p3_lpmHostMemHandle_t) vp;
    sal_memset(handle, 0, nbytes);
    handle->allocatedBytes = nbytes;
    handle->start = vp;
    handle->extraFreeLists = NULL;
    handle->top = (uint8 *) vp + nbytes;
    handle->free =
        (uint8 *) vp + alignSize(sizeof(g2p3_lpmHostMem_t));
    COMPILER_64_ZERO(handle->newAllocs);
    COMPILER_64_ZERO(handle->reusedAllocs);
    COMPILER_64_ZERO(handle->deallocs);
    COMPILER_64_ZERO(handle->allocBytes);
    COMPILER_64_ZERO(handle->deallocBytes);
    handle->hostFree = hostFree;
    handle->hostFreeToken = hostFreeToken;
    *pHandle = handle;

    handle->mutex = sal_mutex_create("LPM HOST MUTEX");
    if (handle->mutex == NULL) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("HostMem Mutex: sal_mutex_create failed")));
        bogus.handle = NULL; /* not used on free code */
        hostFree(hostFreeToken, SB_ALLOC_INTERNAL, nbytes, handle, bogus);
        return SB_FAILED;
    }
    return SB_OK;
}

/* see g2p3_lpmHostMem.h for contract */
sbStatus_t
g2p3_lpmHostMemAlloc(g2p3_lpmHostMemHandle_t hostMem,
                         void **result, size_t nbytes, char*owner)
{
    lpmHostMemBlock_p_t *pBlocks;
    lpmHostMemBlock_p_t block;
    lpmHostMemFreeList_p_t fl;
    SB_ASSERT(hostMem);
    SB_ASSERT(hostMem->start);
    SB_ASSERT(hostMem->start <= hostMem->free);
    SB_ASSERT(hostMem->free <= hostMem->top);
    SB_ASSERT(nbytes > 0);
    SB_ASSERT(owner);

#if LPM_HOST_MEM_DEBUG
    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META("HostMem Alloc: 0x%x (%d) bytes "), nbytes, nbytes));
#endif
    *result = NULL; /* in case of error exit */
    nbytes = alignSize(nbytes);
    if (nbytes < sizeof(lpmHostMemBlock_t))
        nbytes = sizeof(lpmHostMemBlock_t);
    pBlocks = NULL;
    for (fl = hostMem->extraFreeLists;
         fl && fl->blockBytes <= nbytes;
         fl = fl->next)  {
        if (fl->blockBytes == nbytes) {
            pBlocks = &fl->freeBlocks;
        }
    }
    if (pBlocks && *pBlocks) {
#if LPM_HOST_MEM_DEBUG
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("(FL) ***\n")));
#endif
        /* take a block off the freelist */
        block = *pBlocks;
        *pBlocks = (*pBlocks)->next;
        COMPILER_64_ADD_32(hostMem->reusedAllocs,1);
    } else {
        /* allocate block, plus an extra freelist entry if required */
        size_t extra = pBlocks ? 0 : sizeof(lpmHostMemFreeList_t);
#if LPM_HOST_MEM_DEBUG
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("(extend top%s) ***\n"), extra ? " and alloc freelist" : ""));
#endif
        if (nbytes + extra > hostMem->top - hostMem->free)
            return SB_LPM_OUT_OF_HOST_MEMORY;
        if (extra > 0) {
            /* insert extra freelist field */
            lpmHostMemFreeList_p_t fl, *pFl;
            fl = (lpmHostMemFreeList_p_t) hostMem->free;
            fl->blockBytes = nbytes;
            fl->freeBlocks = NULL;
            for (pFl = &hostMem->extraFreeLists;
                 *pFl && (*pFl)->blockBytes < nbytes;
                 pFl = &(*pFl)->next);
            SB_ASSERT(*pFl == NULL || (*pFl)->blockBytes > nbytes);
            fl->next = *pFl;
            *pFl = fl;
        }
        block = (lpmHostMemBlock_p_t) (hostMem->free + extra);
        hostMem->free += nbytes + extra;
        COMPILER_64_ADD_32(hostMem->newAllocs,1);
    }
    COMPILER_64_ADD_32(hostMem->allocBytes, nbytes);
    block->user = owner;
    block->size = nbytes;
    block->prev = NULL;

    LPM_HOST_MEM_LOCK(hostMem);

    block->next = hostMem->usedBlocks;
    if (hostMem->usedBlocks) {
       hostMem->usedBlocks->prev = block;
    }
    hostMem->usedBlocks = block;

    LPM_HOST_MEM_UNLOCK(hostMem);

    *result = (void*)((char*)block+sizeof(*block));

    return SB_OK;
}

/* see g2p3_lpmHostMem.h for contract */
sbStatus_t
g2p3_lpmHostMemFree(g2p3_lpmHostMemHandle_t hostMem,
                        void *pointer, size_t nbytes)
{
    lpmHostMemBlock_p_t *pBlocks;
    lpmHostMemBlock_p_t block;
    lpmHostMemFreeList_p_t fl;

    SB_ASSERT(hostMem);
    SB_ASSERT(nbytes > 0);
    SB_ASSERT(pointer);
#if LPM_HOST_MEM_DEBUG
    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META("HostMem Free: 0x%x (%d) bytes ***\n"), nbytes, nbytes));
#endif
    block = (lpmHostMemBlock_p_t) ((char*)pointer - sizeof(lpmHostMemBlock_t));
    nbytes = alignSize(nbytes);
    /* Remove it from the used block chain */
    LPM_HOST_MEM_LOCK(hostMem);

    if (block->next) 
       block->next->prev = block->prev;
    if (block->prev) 
       block->prev->next = block->next;
    if (block == hostMem->usedBlocks)
       hostMem->usedBlocks = block->next;

    LPM_HOST_MEM_UNLOCK(hostMem);

    if (nbytes < sizeof(lpmHostMemBlock_t))
        nbytes = sizeof(lpmHostMemBlock_t);

    pBlocks = NULL;
    for (fl = hostMem->extraFreeLists;
             fl && fl->blockBytes <= nbytes;
             fl = fl->next)                  {
        if (fl->blockBytes == nbytes) {
            pBlocks = &fl->freeBlocks;
        }
    }
    SB_ASSERT(pBlocks);
    if (pBlocks) {
        /* insert into freelist */
        block->next = *pBlocks;
        *pBlocks = block;
    }
    COMPILER_64_ADD_32(hostMem->deallocs,1);
    COMPILER_64_ADD_32(hostMem->deallocBytes, nbytes);
    return SB_OK;
}

/* see g2p3_lpmHostMem.h for contract */
sbStatus_t
g2p3_lpmHostMemUninit(g2p3_lpmHostMemHandle_t *pHostMem)
{
    sbStatus_t status;
    g2p3_lpmHostMemHandle_t hostMem;
    sbDmaMemoryHandle_t bogus;
    uint64 uuTmp;

    SB_ASSERT(pHostMem);
    hostMem = *pHostMem;
    SB_ASSERT(hostMem);
    bogus.handle = NULL; /* not used on free code */
    

    /* no leaks */
    uuTmp = hostMem->newAllocs;
    COMPILER_64_ADD_64(uuTmp, hostMem->reusedAllocs);
    SB_ASSERT(COMPILER_64_EQ(hostMem->deallocs,uuTmp));
    SB_ASSERT(COMPILER_64_EQ(hostMem->deallocBytes, hostMem->allocBytes));
    
    status = SB_OK;
    if (hostMem->mutex) 
        sal_mutex_destroy(hostMem->mutex);

    if (hostMem->hostFree) {
        status = hostMem->hostFree(hostMem->hostFreeToken,
                                   SB_ALLOC_INTERNAL,
                                   hostMem->allocatedBytes, hostMem, bogus);
    }
    *pHostMem = NULL;

    return status;
}

void
g2p3_lpmHostMemUsage(g2p3_lpmHostMemHandle_t pHostMem)
{
    lpmHostMemBlock_t *pblock;
    char buffer[26];
    uint32 lpm_rbt_node_used=0, lpm_node_used=0, hpm_pyld_used=0, dpm_pyld_used=0;
    uint32 lpm_rbt_node=0, lpm_node=0, hpm_pyld=0, dpm_pyld=0, tot_used=0;;
   
    if (pHostMem) {
        LOG_CLI((BSL_META("\n --- LPM Host memory usage ---- ")));
        LOG_CLI((BSL_META("\n\n   Max available        : %u"), (uint32)pHostMem->allocatedBytes));
        LOG_CLI((BSL_META("\n   num new allocs       : 0x%x%08x"), COMPILER_64_HI(pHostMem->newAllocs), COMPILER_64_LO(pHostMem->newAllocs))); 
        LOG_CLI((BSL_META("\n   num reallocs         : 0x%x%08x"), COMPILER_64_HI(pHostMem->reusedAllocs), COMPILER_64_LO(pHostMem->reusedAllocs))); 
        LOG_CLI((BSL_META("\n   num freed            : 0x%x%08x"), COMPILER_64_HI(pHostMem->deallocs), COMPILER_64_LO(pHostMem->deallocs))); 
        LOG_CLI((BSL_META("\n   total alloc bytes    : 0x%x%08x"), COMPILER_64_HI(pHostMem->allocBytes), COMPILER_64_LO(pHostMem->allocBytes))); 
        LOG_CLI((BSL_META("\n   total deallocs bytes : 0x%x%08x\n"), COMPILER_64_HI(pHostMem->deallocBytes), COMPILER_64_LO(pHostMem->deallocBytes))); 
        LOG_CLI((BSL_META("\n   Block Level Statistics \n")));

       LPM_HOST_MEM_LOCK(pHostMem);
       for (pblock = pHostMem->usedBlocks;
               pblock; pblock = pblock->next) {


          if (sal_strcmp(pblock->user, "lpm node")==0) {
             lpm_node++;
             lpm_node_used+=pblock->size;
          } else if(sal_strcmp(pblock->user, "hpm pyld")==0) {
             hpm_pyld++;
             hpm_pyld_used +=pblock->size;
          } else if(sal_strcmp(pblock->user, "dpm pyld")==0) {
             dpm_pyld++;
             dpm_pyld_used +=pblock->size;
          } else if(sal_strcmp(pblock->user, "lpm rbt node")==0) {
             lpm_rbt_node++;
             lpm_rbt_node_used +=pblock->size;
          } else {
             LOG_CLI((BSL_META("\n   %-22s -- %12d "),pblock->user, pblock->size));
             tot_used += pblock->size;
          }
       }
       LPM_HOST_MEM_UNLOCK(pHostMem);

       sal_snprintf(buffer, 26,  "%12s (%d)", "lpm rbt node", lpm_rbt_node);
       LOG_CLI((BSL_META("\n   %-22s -- %12d "), buffer, lpm_rbt_node_used));
       sal_snprintf(buffer, 26,  "%12s (%d)", "lpm node", lpm_node);
       LOG_CLI((BSL_META("\n   %-22s -- %12d "), buffer, lpm_node_used));
       sal_snprintf(buffer, 26,  "%12s (%d)", "hpm pyld", hpm_pyld);
       LOG_CLI((BSL_META("\n   %-22s -- %12d "), buffer, hpm_pyld_used));
       sal_snprintf(buffer, 26,  "%12s (%d)", "dpm pyld", dpm_pyld);
       LOG_CLI((BSL_META("\n   %-22s -- %12d "), buffer, dpm_pyld_used));
       tot_used += lpm_rbt_node_used + lpm_node_used + hpm_pyld_used + dpm_pyld_used;
       LOG_CLI((BSL_META("\n   Total used: %d\n"), tot_used));
    }
}


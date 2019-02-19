/*
 * $Id: dcmn_mem.c,v 1.3 Broadcom SDK $
 *
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
 * SOC DCMN MEM
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MEM
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/drv.h>
#include <soc/memory.h>
#include <soc/mem.h>

/* Allocate memory of a given size, and store its location in the given pointer */
uint32 dcmn_alloc_mem(
    const int unit,
    void      **mem_ptr,       /* Will hold the pointer to the allocated memory, must be NULL */
    const unsigned size,       /* memory size in bytes to be allocated */
    const char     *alloc_name /* name of the memory allocation, used for debugging */
)
{
    SOCDNX_INIT_FUNC_DEFS;
    if (mem_ptr == NULL || alloc_name == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("null parameter" )));
    } else if (*mem_ptr != NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("memory pointer value is not NULL, when attempted to allocate %s"), alloc_name));
    }
    if ((*mem_ptr = sal_alloc(size, (char*)alloc_name)) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY,(_BSL_SOCDNX_MSG("Failed to allocate %u bytes for %s"), size, alloc_name));
    }
    sal_memset(*mem_ptr, 0, size); /* init the allocated memory to zero */
exit:
    SOCDNX_FUNC_RETURN;
}

/* deallocate memory of a given size, and store its location in the given pointer */
uint32 dcmn_free_mem(
    const int unit,
    void **mem_ptr /* holds the pointer to the allocated memory, will be set to NULL */
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(mem_ptr);
    if (mem_ptr == NULL || *mem_ptr == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("null parameter" )));
    }
    sal_free(*mem_ptr);
    *mem_ptr = NULL;
exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Allocate memory of a given size, for DMA access to a given.
 * If DMA is enabled for the device, a DMA buffer will be allocated, otherwise regular memory will be allocated.
 * The allocated buffer is stored in the given pointer */

uint32 dcmn_alloc_dma_mem(
    const int unit,
    const uint8     is_slam,    /* If not FALSE, DMA enabled will be tested for SLAM DMA and not for table DMA */
    void            **mem_ptr,  /* Will hold the pointer to the allocated memory, must be NULL */
    const unsigned  size,       /* memory size in bytes to be allocated */
    const char      *alloc_name /* name of the memory allocation, used for debugging */
)
{
    SOCDNX_INIT_FUNC_DEFS;
    if (mem_ptr == NULL || alloc_name == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("null parameter" )));
    } else if (*mem_ptr != NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("memory pointer value is not NULL, when attempted to allocate %s"), alloc_name));
    }
    if (is_slam == FALSE ? soc_mem_dmaable(unit, 0, 0) : soc_mem_slamable(unit, 0, 0)) { /* check if DMA is enabled */
        if ((*mem_ptr = soc_cm_salloc(unit, size, alloc_name)) == NULL){
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY,(_BSL_SOCDNX_MSG("Failed to allocate %u bytes of DMA memory for %s"), alloc_name));
        }
    } else {
        if ((*mem_ptr = sal_alloc(size, (char*)alloc_name)) == NULL){
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY,(_BSL_SOCDNX_MSG("Failed to allocate %u bytes of memory for %s"), alloc_name));
        }
    }
    sal_memset(*mem_ptr, 0, size); /* init the allocated memory to zero */
exit:
    SOCDNX_FUNC_RETURN;
}

/* deallocate memory of a given size, and store its location in the given pointer */
uint32 dcmn_free_dma_mem(
    const int   unit,
    const uint8 is_slam,  /* If not FALSE, DMA enabled will be tested for SLAM DMA and not for table DMA */
    void        **mem_ptr /* holds the pointer to the allocated memory, will be set to NULL */
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(mem_ptr);
    if (mem_ptr == NULL || *mem_ptr == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("null parameter" )));
    }
    if (is_slam == FALSE ? soc_mem_dmaable(unit, 0, 0) : soc_mem_slamable(unit, 0, 0)) { /* check if DMA is enabled */
        soc_cm_sfree(unit, *mem_ptr);
    } else {
        sal_free(*mem_ptr);
    }
    *mem_ptr = NULL;
exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Functions to fill memories using SLAM DMA if possible, using a pre-allocated DMA
 * buffer per device, to which the given entry is copied.
 */

#define MAX_U32S_IN_MEM_ENTRY 32
STATIC void *dma_buffers[SOC_SAND_MAX_DEVICE] = {0};
STATIC sal_mutex_t dma_buf_mutexes[SOC_SAND_MAX_DEVICE] = {0};

/* Init the dcmn fill table mechanism for a given unit */
uint32 dcmn_init_fill_table(
    const  int unit
)
{
    SOCDNX_INIT_FUNC_DEFS;
    if (soc_mem_slamable(unit, 0, 0)) { /* check if DMA is enabled */
        SOCDNX_IF_ERR_EXIT(dcmn_alloc_dma_mem(unit, TRUE, dma_buffers + unit, MAX_U32S_IN_MEM_ENTRY * sizeof(uint32), "fill_table"));
        dma_buf_mutexes[unit] = sal_mutex_create("dcmn_fill_table");
    }
exit:
    SOCDNX_FUNC_RETURN;
}

/* De-init the dcmn fill table mechanism for a given unit */
uint32 dcmn_deinit_fill_table(
    const  int unit
)
{
    SOCDNX_INIT_FUNC_DEFS;
    if (dma_buffers[unit] != NULL) {
        SOCDNX_IF_ERR_EXIT(sal_mutex_take(dma_buf_mutexes[unit], sal_mutex_FOREVER));
        SOCDNX_IF_ERR_EXIT(dcmn_free_dma_mem(unit, TRUE, dma_buffers + unit));
        sal_mutex_destroy(dma_buf_mutexes[unit]);
    }
exit:
    SOCDNX_FUNC_RETURN;
}




/* Fill the whole table with the given entry, uses fast DMA filling when run on real hardware */
uint32 dcmn_fill_table_with_entry(
    const int       unit,
    const soc_mem_t mem,        /* memory/table to fill */
    const int       copyno,     /* Memory/table block to fill */
    const void      *entry_data /* The contents of the entry to fill the table with. Does not have to be DMA memory */
  )
{
    int should_release = 0;
    void *buffer = dma_buffers[unit];
    SOCDNX_INIT_FUNC_DEFS;
    if (entry_data == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("null buffer" )));
    } else if (!soc_mem_is_valid(unit, mem)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY,(_BSL_SOCDNX_MSG("Invalid memory for unit" )));
    } else if (buffer != NULL) {
        if (soc_mem_entry_words(unit, mem) > MAX_U32S_IN_MEM_ENTRY) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY,(_BSL_SOCDNX_MSG("Memory entry is too big for the operation" )));
        }
        SOCDNX_IF_ERR_EXIT(sal_mutex_take(dma_buf_mutexes[unit], sal_mutex_FOREVER));
        should_release = 1;
        sal_memcpy(buffer, entry_data, soc_mem_entry_words(unit, mem) * sizeof(uint32));
    } else {
        buffer = (void*)entry_data;
    }
    SOCDNX_IF_ERR_EXIT(soc_mem_fill(unit, mem, copyno, buffer));
exit:
    if (should_release && sal_mutex_give(dma_buf_mutexes[unit])) {
        _bsl_error(_BSL_SOCDNX_MSG("Mutex give failed"));
        _rv = SOC_E_FAIL;
    }
    SOCDNX_FUNC_RETURN;
}

/* Fill the specified part of the table with the given entry, uses fast DMA filling when run on real hardware */
uint32 dcmn_fill_partial_table_with_entry(
    const int       unit,
    const soc_mem_t mem,               /* memory/table to fill */
    const unsigned  array_index_start, /* First array index to fill */
    const unsigned  array_index_end,   /* Last array index to fill */
    const int       copyno,            /* Memory/table block to fill */
    const int       index_start,       /* First table/memory index to fill */
    const int       index_end,         /* Last table/memory index to fill */
    const void      *entry_data        /* The contents of the entry to fill the table with. Does not have to be DMA memory */
  )
{
    int should_release = 0;
    void *buffer = dma_buffers[unit];
    SOCDNX_INIT_FUNC_DEFS;
    if (entry_data == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("null buffer" )));
    } else if (!soc_mem_is_valid(unit, mem)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY,(_BSL_SOCDNX_MSG("Invalid memory for unit" )));
    } else if (buffer != NULL) {
        if (soc_mem_entry_words(unit, mem) > MAX_U32S_IN_MEM_ENTRY) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY,(_BSL_SOCDNX_MSG("Memory entry is too big for the operation" )));
        }
        SOCDNX_IF_ERR_EXIT(sal_mutex_take(dma_buf_mutexes[unit], sal_mutex_FOREVER));
        should_release = 1;
        sal_memcpy(buffer, entry_data, soc_mem_entry_words(unit, mem) * sizeof(uint32));
    } else {
        buffer = (void*)entry_data;
    }
    SOCDNX_IF_ERR_EXIT(soc_mem_array_fill_range(unit, 0, mem, array_index_start, array_index_end, copyno, index_start, index_end, buffer));
exit:
    if (should_release && sal_mutex_give(dma_buf_mutexes[unit])) {
        _bsl_error(_BSL_SOCDNX_MSG("Mutex give failed"));
        _rv = SOC_E_FAIL;
    }
    SOCDNX_FUNC_RETURN;
}


/* 
 * This function reads from all cached memories in order to detect and fix SER errors
 */
uint32
soc_dcmn_cache_table_update_all(int unit)
{
    int 
        res = SOC_E_NONE,
        blk, mem, i ,index_cnt;
    soc_memstate_t *mem_state;
    soc_reg_above_64_val_t rd_data;
    
    SOCDNX_INIT_FUNC_DEFS;
      
    for (mem = 0; mem < NUM_SOC_MEM; mem++){
        if (!SOC_MEM_IS_VALID(unit, mem)){
            continue;
        }
        mem_state = &SOC_MEM_STATE(unit, mem);
        MEM_LOCK(unit, mem);
        SOC_MEM_BLOCK_ITER(unit, mem, blk){
            if (!SOC_MEM_BLOCK_VALID(unit, mem, blk)){
                continue;
            }
            if (!(SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE) && 
                 (mem_state->cache[blk] != NULL) && 
                  CACHE_VMAP_TST(mem_state->vmap[blk], 0) && 
                  !SOC_MEM_TEST_SKIP_CACHE(unit)) 
            {
                index_cnt = soc_mem_index_count(unit, mem);
                for (i = 0 ; i < index_cnt ; i++) {
                    /* we use SOC_MEM_DONT_USE_CACHE to read directly from HW */
                    res = soc_mem_array_read_flags(unit, mem, 0, blk, i, rd_data, SOC_MEM_DONT_USE_CACHE);
                    if (res != SOC_E_NONE) {
                        MEM_UNLOCK(unit, mem);
                        SOCDNX_IF_ERR_EXIT(res);
                    }
                }
            }
        }
        MEM_UNLOCK(unit, mem);
    } 

exit:
    SOCDNX_FUNC_RETURN; 
}

#undef _ERR_MSG_MODULE_NAME


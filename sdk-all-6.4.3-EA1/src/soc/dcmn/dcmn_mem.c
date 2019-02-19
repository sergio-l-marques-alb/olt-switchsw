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

/*  
 * Returns the block's indirect read and write registers' size.
 * Supports Jericho and FE3200
 * Used only for wide memory access, thus supports only the blocks which contain
 * the following memories:
 *     Jericho:
 *         IHB_FIFO_DSP_1m
 *         IHB_FIFO_DSP_2m
 *         IHP_FIFO_8_TO_41m
 *         IRE_LAST_RECEIVED_PACKETm
 *         MMU_DRAM_ADDRESS_SPACEm
 *         OCB_OCB_ADDRESS_SPACEm
 *     FE3200:
 *         DCL_CPU_Hm
 */
STATIC int
dcmn_mem_indirect_access_size(int unit, int blktype){
    switch (blktype) {
    /* JERICHO */
    case SOC_BLK_IRE:
        return 520;
    case SOC_BLK_IHP:
        return 590;
    case SOC_BLK_MMU:
    case SOC_BLK_IHB:
    case SOC_BLK_OCB:
    /* FE 3200 */
    case SOC_BLK_DCL:
        return 640;
    }
    return 0;
}

/* 
 * Read or write a register with a pre-determined block. 
 * Currently is being used only for wide memory indirect access. 
 */
STATIC int
dcmn_reg_access_with_block (int unit, int is_write, soc_reg_t reg, int blk, uint32* data){
    int block, reg_size;
    uint32 addr;
    uint8 at;
    
    addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0, is_write, &block, &at);
    block = SOC_BLOCK_INFO(unit,blk).cmic; /* override block */
    
    if ((reg == ECI_INDIRECT_COMMAND_RD_DATAr) || (reg == ECI_INDIRECT_COMMAND_WR_DATAr)) {
        reg_size = dcmn_mem_indirect_access_size(unit, SOC_BLOCK_INFO(unit,blk).type) / 32;
    } else if (SOC_REG_IS_ABOVE_64(unit, reg)) {
        reg_size = SOC_REG_ABOVE_64_INFO(unit, reg).size;
    } else if (SOC_REG_IS_64(unit, reg)) {
        reg_size = 2;
    } else {
        reg_size = 1;
    }
               
    if (is_write){
        return soc_direct_reg_set(unit, block, addr, reg_size, data);
    } else {
        return soc_direct_reg_get(unit, block, addr, reg_size, data);
    }
}

/* 
 * Read or write wide memory, supports Jericho and FE3200 
 * returns 0 on success, -1 on fail. 
 */
int
dcmn_mem_array_wide_access(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data,
                           unsigned operation) /* operation should be 1 for read and 0 for write */
{
    uint8
        acc_type;
    int
        rv = -1,
        words_left,
        blk;
    uint32  
        data32,
        address,
        indirect_size,
        dynamic_access_orig = 0,
        *entry_words = (uint32*)entry_data;
    soc_reg_above_64_val_t
        rd_data;
    soc_timeout_t
        to;

    assert(operation == 0 || operation == 1); /* write = 0, read = 1 */
    
    if (index < 0) {
        index = -index; /* get rid of cache marking, do not support cache */
    }
    
    words_left = soc_mem_entry_words(unit, mem);
    
    indirect_size = dcmn_mem_indirect_access_size(unit, SOC_BLOCK_INFO(unit, SOC_MEM_BLOCK_ANY(unit,mem)).type);
    if (indirect_size == 0) {
        cli_out("unit %d: invalid block for indirect access. blk=%d\n", unit, SOC_MEM_BLOCK_ANY(unit,mem));
        goto done;
    }

    MEM_LOCK(unit, mem);

    /* loop over the blocks */
    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
        if (copyno != COPYNO_ALL && copyno != SOC_CORE_ALL && copyno != blk) {
            continue;
        }

        if (soc_mem_is_signal(unit, mem)) {
            /* Save original dynamic memory access value */
            if (dcmn_reg_access_with_block(unit, 0, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, blk, &data32) != SOC_E_NONE){
                    cli_out("unit %d: Failed reading from reg=ENABLE_DYNAMIC_MEMORY_ACCESSr blk=%d\n", unit, blk);
                    goto done;
            }
            dynamic_access_orig = soc_reg_field_get(unit, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, data32, ENABLE_DYNAMIC_MEMORY_ACCESSf);

            if (dynamic_access_orig == 0) {
                /* Enable dynamic memory access */
                data32 = 0;
                soc_reg_field_set(unit, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, &data32, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1);
                if (dcmn_reg_access_with_block(unit, 1, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, blk, &data32) != SOC_E_NONE){
                    cli_out("unit %d: Failed writing to reg=ENABLE_DYNAMIC_MEMORY_ACCESSr blk=%d (data: %d)\n", unit, blk, data32);
                    goto done;
                }
            }
        }

        /* Trigger action automatically on write data */
        data32 = 0;
        soc_reg_field_set(unit, ECI_INDIRECT_COMMANDr, &data32, INDIRECT_COMMAND_TRIGGER_ON_DATAf, 1 - operation);
        soc_reg_field_set(unit, ECI_INDIRECT_COMMANDr, &data32, INDIRECT_COMMAND_TIMEOUTf, 0x7fff);
        if (dcmn_reg_access_with_block(unit, 1, ECI_INDIRECT_COMMANDr, blk, &data32) != SOC_E_NONE){
            cli_out("unit %d: Failed writing to reg=INDIRECT_COMMAND blk=%d (data: %d)\n", unit, blk, data32);
            goto done;
        }
              
        address = soc_mem_addr_get(unit, mem, array_index, blk, index, &acc_type);
        /* set start address (address is being automatically incremented by design) */
        data32 = 0;
        soc_reg_field_set(unit, ECI_INDIRECT_COMMAND_ADDRESSr, &data32, INDIRECT_COMMAND_ADDRf, address);
        soc_reg_field_set(unit, ECI_INDIRECT_COMMAND_ADDRESSr, &data32, INDIRECT_COMMAND_TYPEf, operation);
        if (dcmn_reg_access_with_block(unit, 1, ECI_INDIRECT_COMMAND_ADDRESSr, blk, &data32) != SOC_E_NONE){
            cli_out("unit %d: Failed writing to reg=INDIRECT_COMMAND_ADDRESS blk=%d (data: %d)\n", unit, blk, data32);
            goto done;
        }
        
        while (words_left > 0)
        {
            /* write data */
            if (operation == 0) {
                if (dcmn_reg_access_with_block(unit, 1, ECI_INDIRECT_COMMAND_WR_DATAr, blk, entry_words) != SOC_E_NONE){
                    cli_out("unit %d: Failed writing to reg=INDIRECT_COMMAND_WR_DATA blk=%d (data: %u)\n", unit, blk, *entry_words);
                    goto done;
                }
            }
            
            /* Reading requires triggering action manually */
            if(operation == 1){
                data32 = 0;
                soc_reg_field_set(unit, ECI_INDIRECT_COMMANDr, &data32, INDIRECT_COMMAND_TRIGGERf, 1);
                if (dcmn_reg_access_with_block(unit, 1, ECI_INDIRECT_COMMANDr, blk, &data32) != SOC_E_NONE){
                    cli_out("unit %d: Failed writing to reg=INDIRECT_COMMAND blk=%d (data: %d)\n", unit, blk, data32);
                    goto done;
                }
            }      
    
            /* wait for trigger to become 0 (transaction complete) */
            soc_timeout_init(&to, 5000, 10);
            while(1) {
                if (dcmn_reg_access_with_block(unit, 0, ECI_INDIRECT_COMMANDr, blk, &data32) != SOC_E_NONE){
                    cli_out("unit %d: Failed reading from reg=INDIRECT_COMMAND blk=%d\n", unit, blk);
                    goto done;
                }
                if (soc_reg_field_get(unit, ECI_INDIRECT_COMMANDr, data32, INDIRECT_COMMAND_TRIGGERf)) {
                    if (soc_timeout_check(&to)) {
                        cli_out("indirect wide memory operation timed out\n");
                        goto done;
                    }
                } else {
                    break;
                }
            }
            
            if (soc_reg_field_get(unit, ECI_INDIRECT_COMMANDr, data32, INDIRECT_COMMAND_STATUSf)){
                cli_out("indirect wide memory operation failed on time out\n");
                goto done;
            }
            
            /* Get read data */
            if (operation == 1) { 
                if (dcmn_reg_access_with_block(unit, 0, ECI_INDIRECT_COMMAND_RD_DATAr, blk, rd_data) != SOC_E_NONE){
                    cli_out("unit %d: Failed reading from reg=INDIRECT_COMMAND_RD_DATA blk=%d\n", unit, blk);
                    goto done;
                }
                memcpy(entry_words, rd_data, indirect_size/8);
            }
            
            entry_words += indirect_size/32; 
            words_left  -= indirect_size/32;
        }

        if (soc_mem_is_signal(unit, mem) && dynamic_access_orig == 0) {
            /* Disable dynamic memory access */
            data32 = 0;
            soc_reg_field_set(unit, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, &data32, ENABLE_DYNAMIC_MEMORY_ACCESSf, 0);
            if (dcmn_reg_access_with_block(unit, 1, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, blk, &data32) != SOC_E_NONE){
                cli_out("unit %d: Failed writing to reg=ENABLE_DYNAMIC_MEMORY_ACCESSr blk=%d (data: %d)\n", unit, blk, data32);
                goto done;
            }
        }

    } /* finished looping over blocks */

    rv = 0;

 done:
    MEM_UNLOCK(unit, mem);
    return rv;
}

#undef _ERR_MSG_MODULE_NAME


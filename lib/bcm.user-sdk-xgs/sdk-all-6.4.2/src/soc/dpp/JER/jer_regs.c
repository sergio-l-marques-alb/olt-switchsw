/*
 * $Id: jer_regs.c,v 1.0 13/07/2014 14:07:36 nhefetz Exp $
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
 */

/* must be at begining of file */
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

/*
 * Includes
 */
#include <shared/bsl.h>

/* SOC includes */
#include <soc/error.h>

/* SOC DPP includes */
#include <soc/dpp/drv.h>

/* SOC DPP JER includes */
#include <soc/dpp/JER/jer_regs.h>

/*****************/
/***  Defines  ***/
/*****************/ 

#define SOC_JER_REGS_ACCESS_CHECK_REF_TEST_VALUE 0xaaff5500
#define SOC_JER_REGS_ACCESS_CHECK_DMA_MEM_ENTRY_COUNT 20
#define SOC_JER_REGS_ALL_ONES 0xffffffff

/* a unique mask for higher bits (32 - 63) for IHB_ACTION_DISABLE reg, if changed, need to be updated according to regs DB file   */
/* the mask assures that only the actual bits in use from the most significant word of the reg are used                         */
#define SOC_JER_REGS_IHB_ACTION_DISABLE_HIGH_BITS_MASK 0x3fffffff

#define SOC_JER_REGS_ACTUAL_SIZE_OF_FULL_DRAM_REJECT_COUNTER_REG 128
#define SOC_JER_REG_IQM_HEADER_APPEND_PTR_TO_COUNTER_HDR_CMP_MAPPING_TABLE_MASK 0xffff
/*******************/
/***  Functions  ***/
/*******************/ 


/********************************/
/***  Check Access Functions  ***/
/********************************/ 

int soc_jer_regs_eci_access_check(int unit)
{
    uint32 reg_val = SOC_JER_REGS_ACCESS_CHECK_REF_TEST_VALUE;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(WRITE_ECI_TEST_REGISTERr(unit, reg_val));
    SOCDNX_IF_ERR_EXIT(READ_ECI_TEST_REGISTERr(unit, &reg_val));

#ifdef SAL_BOOT_PLISIM
    if (SAL_BOOT_PLISIM == 0) {
        reg_val = ~reg_val;
    }
#endif /* SAL_BOOT_PLISIM */

    if (reg_val != (SOC_JER_REGS_ACCESS_CHECK_REF_TEST_VALUE)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unable to properly access ECI\n")));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


/* Reads and writes to regs of sizes 32, 64 and >64 from IDR and IHB blocks */
int soc_jer_regs_blocks_access_check_regs(int unit)
{
    uint32                  reg32_original_value, reg32_test_value, reg32_test_value_ref = SOC_JER_REGS_ACCESS_CHECK_REF_TEST_VALUE;
    uint64                  reg64_original_value, reg64_test_value_ref, reg64_test_value;
    soc_reg_above_64_val_t  reg_above_64_original_value, reg_above_64_test_value_ref, reg_above_64_test_value, reg_above_64_auxiliary_mask;

    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_SET(reg64_test_value_ref, SOC_JER_REGS_ACCESS_CHECK_REF_TEST_VALUE & SOC_JER_REGS_IHB_ACTION_DISABLE_HIGH_BITS_MASK, SOC_JER_REGS_ACCESS_CHECK_REF_TEST_VALUE);
    
    /* just some random pattern, max size of pattern can be 0xff */
    SOC_REG_ABOVE_64_SET_PATTERN(reg_above_64_test_value_ref, 0xaa);

    SOC_REG_ABOVE_64_CREATE_MASK(reg_above_64_auxiliary_mask, SOC_JER_REGS_ACTUAL_SIZE_OF_FULL_DRAM_REJECT_COUNTER_REG, 0);
    SOC_REG_ABOVE_64_AND(reg_above_64_test_value_ref, reg_above_64_auxiliary_mask);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_test_value);


    /* Read and save starting value */
    SOCDNX_IF_ERR_EXIT(READ_IDR_RESERVED_SPARE_0r(unit, &reg32_original_value));
    SOCDNX_IF_ERR_EXIT(READ_IHB_ACTION_DISABLEr(unit, 0, &reg64_original_value));
    SOCDNX_IF_ERR_EXIT(READ_IDR_DRAM_FIFO_READ_DISABLEr(unit, reg_above_64_original_value));

    /* Write test value */
    SOCDNX_IF_ERR_EXIT(WRITE_IDR_RESERVED_SPARE_0r(unit, reg32_test_value_ref));
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_ACTION_DISABLEr(unit, 0, reg64_test_value_ref));
    SOCDNX_IF_ERR_EXIT(WRITE_IDR_DRAM_FIFO_READ_DISABLEr(unit, reg_above_64_test_value_ref));

    /* Read test value */
    SOCDNX_IF_ERR_EXIT(READ_IDR_RESERVED_SPARE_0r(unit, &reg32_test_value));
    SOCDNX_IF_ERR_EXIT(READ_IHB_ACTION_DISABLEr(unit, 0, &reg64_test_value));
    SOCDNX_IF_ERR_EXIT(READ_IDR_DRAM_FIFO_READ_DISABLEr(unit, reg_above_64_test_value));

    /* Check correctness of read values */
    if (reg32_test_value != reg32_test_value_ref)  {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Problem with Block access - Failed accessing a 32b Reg\n")));
    }

    if ( COMPILER_64_NE(reg64_test_value, reg64_test_value_ref)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Problem with Block access - Failed accessing a 64b Reg\n")));
    }
    
    if (!SOC_REG_ABOVE_64_IS_EQUAL(reg_above_64_test_value_ref, reg_above_64_test_value))  {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Problem with Block access - Failed accessing an above 64b Reg\n")));
    }

    /* Restore initial value */
    SOCDNX_IF_ERR_EXIT(WRITE_IDR_RESERVED_SPARE_0r(unit, reg32_original_value));
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_ACTION_DISABLEr(unit, 0, reg64_original_value));
    SOCDNX_IF_ERR_EXIT(WRITE_IDR_DRAM_FIFO_READ_DISABLEr(unit, reg_above_64_original_value));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_regs_blocks_access_check_mem(int unit, soc_mem_t test_mem, int block_num)
{
    uint32 mem_buff[SOC_MAX_MEM_WORDS], mask[SOC_MAX_MEM_WORDS];

    int word_index, entry_size;

    SOCDNX_INIT_FUNC_DEFS;    

    /* size of each entry in the memory */
    entry_size = soc_mem_entry_words(unit, test_mem);       

    /* bit mask for all r/w bits in memory entry */
    soc_mem_datamask_rw_get(unit, test_mem, mask);          

    /* filling buffer for writing */
    for (word_index = 0; word_index < entry_size; ++word_index) {
        mem_buff[word_index] = SOC_JER_REGS_ACCESS_CHECK_REF_TEST_VALUE & mask[word_index];
    }

    /* Writing and reading buffer */
    SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, test_mem, block_num, soc_mem_index_min(unit, test_mem), mem_buff));
    sal_memset(mem_buff, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, test_mem, block_num, soc_mem_index_min(unit, test_mem), mem_buff));

    /* checking result */
    for (word_index = 0; word_index < entry_size; ++word_index) {
        if ( (mem_buff[word_index] & mask[word_index]) != (SOC_JER_REGS_ACCESS_CHECK_REF_TEST_VALUE & mask[word_index]) ) {
            LOG_DEBUG(BSL_LS_SOC_INIT, (BSL_META_U(unit, "word %d: received:%x \t expected:%x\n"),
                                        word_index, mem_buff[word_index] & mask[word_index], SOC_JER_REGS_ACCESS_CHECK_REF_TEST_VALUE & mask[word_index]));
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Problem with Block access - Failed accessing Mem\n")));      
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_regs_blocks_access_check_dma(int unit)
{
    uint32 *mem_ptr, mask[SOC_MAX_MEM_WORDS];

    /* Should be filled with the checked memory */
    soc_mem_t test_mem = IRR_MCDBm;

    int i, j, dma_size, entry_size, word_count;

    SOCDNX_INIT_FUNC_DEFS;

    /* size of each entry in the memory */
    entry_size = soc_mem_entry_words(unit, test_mem);       

    /* bit mask for all r/w bits in memory entry */
    soc_mem_datamask_rw_get(unit, test_mem, mask);          

    word_count = entry_size * SOC_JER_REGS_ACCESS_CHECK_DMA_MEM_ENTRY_COUNT;
    dma_size = sizeof(uint32) * word_count;

    /* Allocation */
    if ((mem_ptr = soc_cm_salloc(unit, dma_size, "Jericho memory DMA access check")) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY,(_BSL_SOCDNX_MSG("Failed to allocate %u bytes of DMA memory access Check.\n"), dma_size));
    }

    /* filling buffer for writing */
    for (i = 0; i < SOC_JER_REGS_ACCESS_CHECK_DMA_MEM_ENTRY_COUNT; ++i) {
        for (j = 0; j < entry_size; ++j) {
            mem_ptr[i*entry_size + j] = SOC_JER_REGS_ACCESS_CHECK_REF_TEST_VALUE + i;                
        }
    }
     
    /* Writing then Reading using DMA */ 
    SOCDNX_IF_ERR_EXIT( soc_mem_write_range(unit, test_mem, MEM_BLOCK_ANY, 0, SOC_JER_REGS_ACCESS_CHECK_DMA_MEM_ENTRY_COUNT - 1, (void*)mem_ptr));
    sal_memset(mem_ptr, 0, dma_size);
    SOCDNX_IF_ERR_EXIT( soc_mem_read_range(unit, test_mem, MEM_BLOCK_ANY, 0, SOC_JER_REGS_ACCESS_CHECK_DMA_MEM_ENTRY_COUNT - 1, (void*)mem_ptr));

    /* Checking result, every entry should be checked with mask. */
    for (i = 0; i < SOC_JER_REGS_ACCESS_CHECK_DMA_MEM_ENTRY_COUNT; ++i) {
        for (j = 0; j < entry_size; ++j) {
            if ((mem_ptr[i*entry_size + j] & mask[j]) != ((SOC_JER_REGS_ACCESS_CHECK_REF_TEST_VALUE + i) & mask[j])) {
                LOG_DEBUG(BSL_LS_SOC_INIT, (BSL_META_U(unit, "entry %d: received:%x \t expected:%x\n"), i, mem_ptr[i*entry_size + j] & mask[j], (SOC_JER_REGS_ACCESS_CHECK_REF_TEST_VALUE + i) & mask[j]));
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Problem with Block access - Failed accessing DMA\n")));      
            }
        }
    }

exit:
    if (mem_ptr != NULL) {
        soc_cm_sfree(unit, mem_ptr); 
    }
    SOCDNX_FUNC_RETURN;
}

/* Check access to few random blocks IDR, IHB, IQM, EGQ and IRR blocks */
int soc_jer_regs_blocks_access_check(int unit)
{
    int use_dma;
    SOCDNX_INIT_FUNC_DEFS;

    /* check if we can use DMA */
    use_dma =
#ifdef PLISIM
    SAL_BOOT_PLISIM ? 0 :
#endif /* PLISIM */
      soc_mem_dmaable(unit, IRR_MCDBm, SOC_MEM_BLOCK_ANY(unit, IRR_MCDBm)); 

    /* Check access to blocks */
    SOCDNX_IF_ERR_EXIT(soc_jer_regs_blocks_access_check_regs(unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_regs_blocks_access_check_mem(unit, IQM_CNTCMD_HAPMm, IQM_BLOCK(unit, 0)));
    SOCDNX_IF_ERR_EXIT(soc_jer_regs_blocks_access_check_mem(unit, EGQ_PPCTm, EGQ_BLOCK(unit, 1)));

    if (use_dma) 
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_regs_blocks_access_check_dma(unit));
    }



exit:
    SOCDNX_FUNC_RETURN;
}

/* Must be at end of file */
#undef _ERR_MSG_MODULE_NAME





/*
 * $Id: sand_mem.c,v $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC SAND MEM
 */
 
#include <shared/bsl.h>
#define BSL_LOG_MODULE BSL_LS_SOC_MEM
#include <shared/shrextend/shrextend_debug.h>

#include <soc/sand/sand_mem.h>
#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#endif /* BCM_DPP_SUPPORT */
#include <soc/memory.h>
#include <soc/mem.h>

/* Allocate memory of a given size, and store its location in the given pointer */
uint32 sand_alloc_mem(
    const int unit,
    void      *mem_ptr,        /* output: Will hold the pointer to the allocated memory, must be NULL. The real type of the argument is void** is not used to avoid compilation warnings */
    const unsigned size,       /* memory size in bytes to be allocated */
    const char     *alloc_name /* name of the memory allocation, used for debugging */
)
{
    SHR_FUNC_INIT_VARS(unit);
    if (mem_ptr == NULL || alloc_name == NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "null parameter %s%s%s\n", EMPTY, EMPTY, EMPTY);
    } else if (*(void**)mem_ptr != NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "memory pointer value is not NULL, when attempted to allocate %s%s%s\n", alloc_name, EMPTY, EMPTY);
    }
    if ((*(void**)mem_ptr = sal_alloc(size, (char*)alloc_name)) == NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_MEMORY, "Failed to allocate %u bytes for %s%s\n", size, alloc_name, EMPTY);
    }
    sal_memset(*(void**)mem_ptr, 0, size); /* init the allocated memory to zero */
exit:
    SHR_FUNC_EXIT;
}

/* deallocate memory of a given size, and store its location in the given pointer */
uint32 sand_free_mem(
    const int unit,
    void **mem_ptr /* holds the pointer to the allocated memory, will be set to NULL */
)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mem_ptr, _SHR_E_PARAM, "mem_ptr");
    if (mem_ptr == NULL || *mem_ptr == NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "null parameter %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
    sal_free(*mem_ptr);
    *mem_ptr = NULL;
exit:
    SHR_FUNC_EXIT;
}

/* deallocate memory of a given size, and store its location in the given pointer */
void sand_free_mem_if_not_null(
    const int unit,
    void **mem_ptr /* holds the pointer to the allocated memory, will be set to NULL */
)
{
    if (mem_ptr != NULL && *mem_ptr != NULL) {
        sal_free(*mem_ptr);
        *mem_ptr = NULL;
    }
}


/*
 * Allocate memory of a given size, for DMA access to a given.
 * If DMA is enabled for the device, a DMA buffer will be allocated, otherwise regular memory will be allocated.
 * The allocated buffer is stored in the given pointer */

uint32 sand_alloc_dma_mem(
    const int unit,
    const uint8     is_slam,    /* If not FALSE, DMA enabled will be tested for SLAM DMA and not for table DMA */
    void            **mem_ptr,  /* Will hold the pointer to the allocated memory, must be NULL */
    const unsigned  size,       /* memory size in bytes to be allocated */
    const char      *alloc_name /* name of the memory allocation, used for debugging */
)
{
    SHR_FUNC_INIT_VARS(unit);
    if (mem_ptr == NULL || alloc_name == NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "null parameter %s%s%s\n", EMPTY, EMPTY, EMPTY);
    } else if (*mem_ptr != NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "memory pointer value is not NULL, when attempted to allocate %s%s%s\n", alloc_name, EMPTY, EMPTY);
    }
    if (is_slam == FALSE ? soc_mem_dmaable(unit, 0, 0) : soc_mem_slamable(unit, 0, 0)) { /* check if DMA is enabled */
        if ((*mem_ptr = soc_cm_salloc(unit, size, alloc_name)) == NULL){
            SHR_EXIT_WITH_LOG(SOC_E_MEMORY, "Failed to allocate %u bytes of DMA memory for %s%s\n", size, alloc_name, EMPTY);
        }
    } else {
        if ((*mem_ptr = sal_alloc(size, (char*)alloc_name)) == NULL){
            SHR_EXIT_WITH_LOG(SOC_E_MEMORY, "Failed to allocate %u bytes of memory for %s%s\n", size, alloc_name, EMPTY);
        }
    }
    sal_memset(*mem_ptr, 0, size); /* init the allocated memory to zero */
exit:
    SHR_FUNC_EXIT;
}

/* deallocate memory of a given size, and store its location in the given pointer */
uint32 sand_free_dma_mem(
    const int   unit,
    const uint8 is_slam,  /* If not FALSE, DMA enabled will be tested for SLAM DMA and not for table DMA */
    void        **mem_ptr /* holds the pointer to the allocated memory, will be set to NULL */
)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mem_ptr, _SHR_E_PARAM, "mem_ptr");
    if (mem_ptr == NULL || *mem_ptr == NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "null parameter %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
    if (is_slam == FALSE ? soc_mem_dmaable(unit, 0, 0) : soc_mem_slamable(unit, 0, 0)) { /* check if DMA is enabled */
        soc_cm_sfree(unit, *mem_ptr);
    } else {
        sal_free(*mem_ptr);
    }
    *mem_ptr = NULL;
exit:
    SHR_FUNC_EXIT;
}


/*
 * Functions to fill memories using SLAM DMA if possible, using a pre-allocated DMA
 * buffer per device, to which the given entry is copied.
 */

STATIC void *dma_buffers[SOC_MAX_NUM_DEVICES] = {0};
STATIC sal_mutex_t dma_buf_mutexes[SOC_MAX_NUM_DEVICES] = {0};

/* Init the dcmn fill table mechanism for a given unit */
uint32 sand_init_fill_table(
    const  int unit
)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dma_buf_mutexes[unit]==0 && soc_mem_slamable(unit, 0, 0)) { /* check if DMA is enabled */
        SHR_IF_ERR_EXIT(sand_alloc_dma_mem(unit, TRUE, dma_buffers + unit, SAND_MAX_U32S_IN_MEM_ENTRY * sizeof(uint32), "fill_table"));
        dma_buf_mutexes[unit] = sal_mutex_create("sand_fill_table");
    }
exit:
    SHR_FUNC_EXIT;
}

/* De-init the dcmn fill table mechanism for a given unit */
uint32 sand_deinit_fill_table(
    const  int unit
)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dma_buffers[unit] != NULL) {
        SHR_IF_ERR_EXIT(sal_mutex_take(dma_buf_mutexes[unit], sal_mutex_FOREVER));
        SHR_IF_ERR_EXIT(sand_free_dma_mem(unit, TRUE, dma_buffers + unit));
        sal_mutex_destroy(dma_buf_mutexes[unit]);
        dma_buf_mutexes[unit]=0;
    }
exit:
    SHR_FUNC_EXIT;
}

/* Fill the whole table with the given entry, uses fast DMA filling when run on real hardware */
uint32 sand_fill_table_with_entry(
    const int       unit,
    const soc_mem_t mem,        /* memory/table to fill */
    const int       copyno,     /* Memory/table block to fill */
    const void      *entry_data /* The contents of the entry to fill the table with. Does not have to be DMA memory */
  )
{
    int should_release = 0;
    void *buffer = dma_buffers[unit];
#if defined BCM_DPP_SUPPORT && defined(PALLADIUM_BACKDOOR)
    int mem_size;
#endif
#ifdef DCMN_RUNTIME_DEBUG
    sal_time_t start_time = sal_time();
    unsigned unsigned_i;
#endif /* DCMN_RUNTIME_DEBUG */

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DPP_SUPPORT
#ifndef PALLADIUM_BACKDOOR
#ifndef JERICHO_EMULATION_OLD_ACCELERATION_BEHAVIOR
    if (mem == IRR_MCDBm && SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->emulation_system) {
        goto fast_exit;
    }
#endif /* JERICHO_EMULATION_OLD_ACCELERATION_BEHAVIOR */
#else /* PALLADIUM_BACKDOOR */
    mem_size = SOC_MEM_INFO(unit, mem).index_max - SOC_MEM_INFO(unit, mem).index_min +1;

    /* We use backdoor for memories which have more than 256 entries. */
    if ((SOC_IS_JERICHO(unit)) && !SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->emulation_system && (mem_size > 256) && !SOC_IS_JERICHO_PLUS(unit)
#ifdef JERICHO_EMULATION_OLD_ACCELERATION_BEHAVIOR
      && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "no_backdoor", 0) != 1
#endif
      ) {
        int array_mem_idx;
        char array_mem_name[1024];
        soc_error_t rv;
#ifdef JERICHO_EMULATION_OLD_ACCELERATION_BEHAVIOR
        soc_mem_t exceptions[] = {
            FDT_IPT_MESH_MCm,
            EPNI_TX_TAG_TABLEm,
            PPDB_A_FEC_SUPER_ENTRY_BANKm
        };
        int exception_idx;
        const int nof_exceptions = sizeof(exceptions) / sizeof(exceptions[0]);

        /* Check that this is not an exception */
        for (exception_idx = 0; exception_idx < nof_exceptions; exception_idx++) {
            if (mem == exceptions[exception_idx]) {
                break;
            }
        }
#endif /* JERICHO_EMULATION_OLD_ACCELERATION_BEHAVIOR */

        /* No exception */
        if (
#ifndef JERICHO_EMULATION_OLD_ACCELERATION_BEHAVIOR
          mem == IRR_MCDBm || mem == EDB_EEDB_BANKm || mem == IHB_DESTINATION_STATUSm
#else
          exception_idx == nof_exceptions
#endif
          ) {
            /* dealing with array memory case*/
            if (SOC_MEM_IS_ARRAY(unit, mem))
            {
                /* we write to each part of the array*/
                for (array_mem_idx = 0; array_mem_idx < SOC_MEM_NUMELS(unit, mem); array_mem_idx++)
                {
                    sal_sprintf(array_mem_name, "%s%d", SOC_MEM_NAME(unit, mem), array_mem_idx);
                    rv = _arad_palladium_backdoor_dispatch_full_table_write(unit, array_mem_name, entry_data, soc_mem_entry_words(unit, mem));
                    SHR_IF_ERR_EXIT(rv);
                }
            } else {
                rv = _arad_palladium_backdoor_dispatch_full_table_write(unit, SOC_MEM_NAME(unit, mem), entry_data, soc_mem_entry_words(unit, mem));
                SHR_IF_ERR_EXIT(rv);
            }
            
            SOC_EXIT;
        }
    }
#endif /* PALLADIUM_BACKDOOR */
#endif /* BCM_DPP_SUPPORT */

   if (entry_data == NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "null buffer %s%s%s\n", EMPTY, EMPTY, EMPTY);
    } else if (!soc_mem_is_valid(unit, mem)) {
        SHR_EXIT_WITH_LOG(SOC_E_MEMORY, "Invalid memory for unit %s%s%s\n", EMPTY, EMPTY, EMPTY);
    } else if ((buffer != NULL) && (soc_mem_entry_words(unit, mem) < SAND_MAX_U32S_IN_MEM_ENTRY)) {
        SHR_IF_ERR_EXIT(sal_mutex_take(dma_buf_mutexes[unit], sal_mutex_FOREVER));
        should_release = 1;
        sal_memcpy(buffer, entry_data, soc_mem_entry_words(unit, mem) * sizeof(uint32));
    } else {
        buffer = (void*)entry_data;
    }
    SHR_IF_ERR_EXIT(soc_mem_fill(unit, mem, copyno, buffer));
    
exit:
    if (should_release && sal_mutex_give(dma_buf_mutexes[unit])) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Mutex give failed\n")));
        _func_rv = SOC_E_FAIL;
    }
#ifdef DCMN_RUNTIME_DEBUG
    if (((unsigned_i = sal_time() - start_time)) >= 5) {
        LOG_INFO(BSL_LS_SOC_INIT, ("==> sand_fill_table_with_entry(%s) ran for %u:%2.2u\n",
          SOC_MEM_NAME(unit, mem), unsigned_i / 60, unsigned_i % 60));
    }
    if (sand_runtime_debug_per_device[unit].run_stage <= sand_runtime_debug_state_initializing) {
        for (unsigned_i = 0; unsigned_i < soc_mem_entry_words(unit, mem); ++unsigned_i) {
            if (((const uint32*)entry_data)[unsigned_i]) { /* assume that entry_data is aligned */
                break;
            }
        }
        if (unsigned_i >= soc_mem_entry_words(unit, mem)) {
            LOG_INFO(BSL_LS_SOC_INIT, ("==> sand_fill_table_with_entry(%s) received a zero entry\n", SOC_MEM_NAME(unit, mem)));
        }
    }
#endif /* DCMN_RUNTIME_DEBUG */
#if defined(BCM_DPP_SUPPORT) && !defined(PALLADIUM_BACKDOOR) && !defined(JERICHO_EMULATION_OLD_ACCELERATION_BEHAVIOR)
    fast_exit:
#endif
    SHR_FUNC_EXIT;
}

/* Fill the specified part of the table with the given entry, uses fast DMA filling when run on real hardware */
uint32 sand_fill_partial_table_with_entry(
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
#ifdef DCMN_RUNTIME_DEBUG
    sal_time_t start_time = sal_time();
    unsigned unsigned_i;
#endif /* DCMN_RUNTIME_DEBUG */
    SHR_FUNC_INIT_VARS(unit);
    if (entry_data == NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "null buffer %s%s%s\n", EMPTY, EMPTY, EMPTY);
    } else if (!soc_mem_is_valid(unit, mem)) {
        SHR_EXIT_WITH_LOG(SOC_E_MEMORY, "Invalid memory for unit %s%s%s\n", EMPTY, EMPTY, EMPTY);
    } else if (buffer != NULL) {
        if (soc_mem_entry_words(unit, mem) > SAND_MAX_U32S_IN_MEM_ENTRY) {
            SHR_EXIT_WITH_LOG(SOC_E_MEMORY, "Memory entry is too big for the operation %s%s%s\n", EMPTY, EMPTY, EMPTY);
        }
        SHR_IF_ERR_EXIT(sal_mutex_take(dma_buf_mutexes[unit], sal_mutex_FOREVER));
        should_release = 1;
        sal_memcpy(buffer, entry_data, soc_mem_entry_words(unit, mem) * sizeof(uint32));
    } else {
        buffer = (void*)entry_data;
    }
    SHR_IF_ERR_EXIT(soc_mem_array_fill_range(unit, 0, mem, array_index_start, array_index_end, copyno, index_start, index_end, buffer));
exit:
    if (should_release && sal_mutex_give(dma_buf_mutexes[unit])) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Mutex give failed\n")));
        _func_rv = SOC_E_FAIL;
    }
#ifdef DCMN_RUNTIME_DEBUG
    if (((unsigned_i = sal_time() - start_time)) >= 5) {
        LOG_INFO(BSL_LS_SOC_INIT, ("==> sand_fill_partial_table_with_entry(%s) ran for %u:%2.2u\n",
          SOC_MEM_NAME(unit, mem), unsigned_i / 60, unsigned_i % 60));
    }
    if (sand_runtime_debug_per_device[unit].run_stage <= sand_runtime_debug_state_initializing) {
        for (unsigned_i = 0; unsigned_i < soc_mem_entry_words(unit, mem); ++unsigned_i) {
            if (((const uint32*)entry_data)[unsigned_i]) { /* assume that entry_data is aligned */
                break;
            }
        }
        if (unsigned_i >= soc_mem_entry_words(unit, mem)) {
            LOG_INFO(BSL_LS_SOC_INIT, ("==> sand_fill_partial_table_with_entry(%s) received a zero entry\n", SOC_MEM_NAME(unit, mem)));
        }
    }
#endif /* DCMN_RUNTIME_DEBUG */
    SHR_FUNC_EXIT;
}


/* 
 * Read or write wide memory, supports Jericho and RAMON.
 * Uses the indirect memory access registers in the memory's block,
 * and should also work for non-wide memories, except for writes to memories
 * marked in reggen_db; where the write register is too narrow.
 */
int sand_mem_array_wide_access(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data,
    unsigned do_read) /* do_read should be non-zero for read and 0 for write */
{
    uint8 acc_type;
    int rv, blk, cmic_block, need_to_unlock = 0;
    uint32 words_left, words_in_operation, w_i, access_size, reg32,
    addr_address, addr_command, addr_write_data, addr_read_data, cache_consistency_check = 0,
           *entry_words = entry_data, *buf;
    soc_reg_above_64_val_t reg_above_64;
    uint8 orig_read_mode = SOC_MEM_FORCE_READ_THROUGH(unit);
    soc_reg_access_info_t access_info;

    SHR_FUNC_INIT_VARS(unit);

    if (!soc_mem_is_valid(unit, mem) || index < soc_mem_index_min(unit, mem) || index > soc_mem_index_max(unit, mem)) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Invalid memory or index %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
    if (do_read) {
        do_read = 1;
    }

#ifdef JR2_CRASH_RECOVERY_SUPPORT
        if (BCM_UNIT_DO_HW_READ_WRITE(unit)){
            soc_dnxc_cr_suppress(unit, dnxc_cr_no_support_wide_mem);
        }
#endif /* JR2_CRASH_RECOVERY_SUPPORT */
    
    words_left = soc_mem_entry_words(unit, mem);


    /* Get indirect memory access registers addresses */
    rv = soc_reg_xaddr_get(unit, ECI_INDIRECT_COMMAND_ADDRESSr, REG_PORT_ANY, 0, SOC_REG_ADDR_OPTION_NONE, &access_info);
    addr_address = access_info.offset;
    rv |= soc_reg_xaddr_get(unit, ECI_INDIRECT_COMMANDr, REG_PORT_ANY, 0, SOC_REG_ADDR_OPTION_NONE, &access_info);
    addr_command = access_info.offset;
    rv |= soc_reg_xaddr_get(unit, ECI_INDIRECT_COMMAND_WR_DATAr, REG_PORT_ANY, 0, SOC_REG_ADDR_OPTION_NONE, &access_info);
    addr_write_data = access_info.offset;
    rv |= soc_reg_xaddr_get(unit, ECI_INDIRECT_COMMAND_RD_DATAr, REG_PORT_ANY, 0, SOC_REG_ADDR_OPTION_NONE, &access_info);
    addr_read_data = access_info.offset;
    if (rv != SOC_E_NONE) {
        SHR_EXIT_WITH_LOG(SOC_E_FAIL, "Invalid memory or index %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

    /*
     * Assuming the read and write registers are 640 bit wide.
     * This is correct for all read registers and for write registers
     * in blocks that have wide memories that are writable by the CPU.
     */
    access_size = SOC_REG_ABOVE_64_MAX_SIZE_U32;

    MEM_LOCK(unit, mem);
    need_to_unlock = 1;

    /* loop over the blocks */
    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
        if (copyno != COPYNO_ALL && copyno != SOC_CORE_ALL && copyno != blk) {
            continue;
        }

        if ((flags & SOC_MEM_DONT_USE_CACHE) != SOC_MEM_DONT_USE_CACHE) {
            if (!do_read) {
                _soc_mem_write_cache_update(unit, mem, blk, 0, index, array_index, entry_data, NULL, NULL, NULL);
            } else {
                SOC_MEM_FORCE_READ_THROUGH_SET(unit, 0);
                if (TRUE == _soc_mem_read_cache_attempt(unit, flags, mem, blk, index, array_index, entry_data, NULL, &cache_consistency_check)) {
                    SOC_MEM_FORCE_READ_THROUGH_SET(unit, orig_read_mode);
                    SHR_EXIT();
                }
            }
        }

        cmic_block = SOC_BLOCK_INFO(unit, blk).cmic;

        /* set start address (address is being automatically incremented by design) */
        reg32 = 0;
        soc_reg_field_set(unit, ECI_INDIRECT_COMMAND_ADDRESSr, &reg32, INDIRECT_COMMAND_ADDRf,
          soc_mem_addr_get(unit, mem, array_index, blk, index, &acc_type));
        soc_reg_field_set(unit, ECI_INDIRECT_COMMAND_ADDRESSr, &reg32, INDIRECT_COMMAND_TYPEf, do_read);
        SHR_IF_ERR_EXIT(soc_direct_reg_set(unit, cmic_block, addr_address, 1, &reg32));

        reg32 = 0;
        if (do_read) { /* Trigger the read operation */
            soc_reg_field_set(unit, ECI_INDIRECT_COMMANDr, &reg32, INDIRECT_COMMAND_TRIGGERf, 1);
        } else { /* Trigger the write operation when data is written to the register */
            soc_reg_field_set(unit, ECI_INDIRECT_COMMANDr, &reg32, INDIRECT_COMMAND_TRIGGER_ON_DATAf, 1);
        }
        soc_reg_field_set(unit, ECI_INDIRECT_COMMANDr, &reg32, INDIRECT_COMMAND_TIMEOUTf, 0x3fff); /* configure access timeout */
        SHR_IF_ERR_EXIT(soc_direct_reg_set(unit, cmic_block, addr_command, 1, &reg32));

        /* Access  the entry part by part */
        for (; words_left > 0; words_left -= words_in_operation)
        {
            words_in_operation = access_size < words_left ? access_size : words_left;
            if (do_read) {  /* read part of the memory entry */
                SHR_IF_ERR_EXIT(soc_direct_reg_get(unit, cmic_block, addr_read_data, words_in_operation, reg_above_64));
                for (buf = reg_above_64, w_i = words_in_operation; w_i; --w_i) {
                    *(entry_words++) = *(buf++);
                } /* does not write beyond the end of the input buffer */
            } else { /* write part of the memory entry */
                for (w_i = 0; w_i < words_in_operation; ++w_i) {
                    reg_above_64[w_i] = *(entry_words++);
                } /* does not read beyond the end of the input buffer */
                SHR_IF_ERR_EXIT(soc_direct_reg_set(unit, cmic_block, addr_write_data, words_in_operation, reg_above_64));
            }
        }

    } /* finished looping over blocks */

exit:
    if (need_to_unlock) {
        MEM_UNLOCK(unit, mem);
    }
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME

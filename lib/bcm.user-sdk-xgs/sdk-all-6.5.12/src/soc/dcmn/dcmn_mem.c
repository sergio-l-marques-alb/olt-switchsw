
/*
 * $Id: dcmn_mem.c,v 1.3 Broadcom SDK $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
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
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/drv.h>
#include <soc/memory.h>
#include <soc/mem.h>
#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#endif /* BCM_DPP_SUPPORT */
#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#ifdef BCM_88950_A0
#include <soc/dfe/fe3200/fe3200_fabric_cell.h>
#endif
#endif /* BCM_DFE_SUPPORT */

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#endif /* CRASH_RECOVERY_SUPPORT */

#if defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
#include <soc/dnxc/legacy/dnxc_mem.h>
#endif /* BCM_DNXF_SUPPORT */


int dcmn_tbl_is_dynamic(int unit,soc_mem_t mem)
{
#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_DFE(unit) && soc_dfe_tbl_is_dynamic(unit, mem)) {
        return TRUE;
    }
#endif /* BCM_DFE_SUPPORT */
#ifdef BCM_DPP_SUPPORT
    if(SOC_IS_DPP(unit) && dpp_tbl_is_dynamic(unit, mem)) {
        return TRUE;
    }
#endif /* BCM_DPP_SUPPORT */
#if defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
        if((SOC_IS_DNXF(unit) || SOC_IS_DNX(unit)) && dnxc_tbl_is_dynamic(unit, mem)) {
            return TRUE;
        }
#endif /* BCM_DNXF_SUPPORT */

    return FALSE;
}

/* check if a given mem contain one of the fields apear in given list*/

int dcmn_mem_contain_one_of_the_fields(int unit,const soc_mem_t mem,soc_field_t *fields)
{

  int i;
  for (i=0;fields[i]!=NUM_SOC_FIELD;i++) {
      if (SOC_MEM_FIELD_VALID(unit,mem,fields[i])) {
          return 1;
      }
  }
  return 0;
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
    uint32 rd_data[120];
    
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
    case SOC_BLK_IPSEC:
    case SOC_BLK_MMU:
    case SOC_BLK_IHB:
    case SOC_BLK_OCB:
    case SOC_BLK_EPNI:
    case SOC_BLK_SPB:
    /* FE 3200 */
    case SOC_BLK_DCL:
    /* QUX */
    case SOC_BLK_NIF:
    /* Jericho PLUS */
    case SOC_BLK_FDT:
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
    uint32 options = SOC_REG_ADDR_OPTION_NONE;

    if (is_write) {
        options |= SOC_REG_ADDR_OPTION_WRITE;
    }

    addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0, options, &block, &at);
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
dcmn_mem_array_wide_access(int unit, uint32 flags, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data,
                           unsigned operation) /* operation should be 1 for read and 0 for write */
{
    uint8
        acc_type;
    int
        rv = -1,
        words_left,
        blk;
    uint32  
        data32[1],
        address,
        indirect_size,
        dynamic_access_orig = 0,
        *entry_words = (uint32*)entry_data;
    soc_reg_above_64_val_t
        rd_data;
    uint8 orig_read_mode = SOC_MEM_FORCE_READ_THROUGH(unit);
    uint32 cache_consistency_check = 0;

    assert(operation == 0 || operation == 1); /* write = 0, read = 1 */

#ifdef CRASH_RECOVERY_SUPPORT
        if (BCM_UNIT_DO_HW_READ_WRITE(unit)){
            soc_dcmn_cr_suppress(unit, dcmn_cr_no_support_wide_mem);
        }
#endif /* CRASH_RECOVERY_SUPPORT */

    if (index < 0) {
        index = -index; /* get rid of cache marking, do not support cache */
    }
    
    words_left = soc_mem_entry_words(unit, mem);
    
    MEM_LOCK(unit, mem);
    indirect_size = dcmn_mem_indirect_access_size(unit, SOC_BLOCK_INFO(unit, SOC_MEM_BLOCK_ANY(unit,mem)).type);
    if (indirect_size == 0) {
        cli_out("unit %d: invalid block for indirect access. blk=%d\n", unit, SOC_MEM_BLOCK_ANY(unit,mem));
        goto done;
    }
#ifdef BCM_PETRA_SUPPORT
    /* be care broadcast block */
    if (copyno == soc_mem_broadcast_block_get(unit,mem)){
        copyno = COPYNO_ALL;
    }
#endif

    /* loop over the blocks */
    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
        if (copyno != COPYNO_ALL && copyno != SOC_CORE_ALL && copyno != blk) {
            continue;
        }

        if ((flags & SOC_MEM_DONT_USE_CACHE) != SOC_MEM_DONT_USE_CACHE) {
            if (operation == 0) {
                _soc_mem_write_cache_update(unit, mem, blk, 0, index, array_index, entry_data, NULL, NULL, NULL);
            } else {
                SOC_MEM_FORCE_READ_THROUGH_SET(unit, 0);
                if (TRUE == _soc_mem_read_cache_attempt(unit, flags, mem, blk, index, array_index, entry_data, NULL, &cache_consistency_check)) {
                    rv = 0;
                    SOC_MEM_FORCE_READ_THROUGH_SET(unit, orig_read_mode);
                    goto done;
                }
            }
        }

        if (soc_mem_is_signal(unit, mem)) {
            /* Save original dynamic memory access value */
            if (dcmn_reg_access_with_block(unit, 0, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, blk, data32) != SOC_E_NONE){
                    cli_out("unit %d: Failed reading from reg=ENABLE_DYNAMIC_MEMORY_ACCESSr blk=%d\n", unit, blk);
                    goto done;
            }
            dynamic_access_orig = soc_reg_field_get(unit, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, *data32, ENABLE_DYNAMIC_MEMORY_ACCESSf);

            if (dynamic_access_orig == 0) {
                /* Enable dynamic memory access */
                *data32 = 0;
                soc_reg_field_set(unit, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, data32, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1);
                if (dcmn_reg_access_with_block(unit, 1, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, blk, data32) != SOC_E_NONE){
                    cli_out("unit %d: Failed writing to reg=ENABLE_DYNAMIC_MEMORY_ACCESSr blk=%d (data: %d)\n", unit, blk, *data32);
                    goto done;
                }
            }
        }

        address = soc_mem_addr_get(unit, mem, array_index, blk, index, &acc_type);
        /* set start address (address is being automatically incremented by design) */
        *data32 = 0;
        soc_reg_field_set(unit, ECI_INDIRECT_COMMAND_ADDRESSr, data32, INDIRECT_COMMAND_ADDRf, address);
        soc_reg_field_set(unit, ECI_INDIRECT_COMMAND_ADDRESSr, data32, INDIRECT_COMMAND_TYPEf, operation);
        if (dcmn_reg_access_with_block(unit, 1, ECI_INDIRECT_COMMAND_ADDRESSr, blk, data32) != SOC_E_NONE){
            cli_out("unit %d: Failed writing to reg=INDIRECT_COMMAND_ADDRESS blk=%d (data: %d)\n", unit, blk, *data32);
            goto done;
        }
        
        /* Trigger action automatically on write data */
        *data32 = 0;
        if(operation == 0)
            soc_reg_field_set(unit, ECI_INDIRECT_COMMANDr, data32, INDIRECT_COMMAND_TRIGGER_ON_DATAf, 1);
        else
            soc_reg_field_set(unit, ECI_INDIRECT_COMMANDr, data32, INDIRECT_COMMAND_TRIGGERf, 1);

        soc_reg_field_set(unit, ECI_INDIRECT_COMMANDr, data32, INDIRECT_COMMAND_TIMEOUTf, 0x7fff);
        if (dcmn_reg_access_with_block(unit, 1, ECI_INDIRECT_COMMANDr, blk, data32) != SOC_E_NONE){
            cli_out("unit %d: Failed writing to reg=INDIRECT_COMMAND blk=%d (data: %d)\n", unit, blk, *data32);
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
            /* Get read data */
            if (operation == 1) { 
                if (dcmn_reg_access_with_block(unit, 0, ECI_INDIRECT_COMMAND_RD_DATAr, blk, rd_data) != SOC_E_NONE){
                    cli_out("unit %d: Failed reading from reg=INDIRECT_COMMAND_RD_DATA blk=%d\n", unit, blk);
                    goto done;
                }
                memcpy(entry_words, rd_data, (words_left > (indirect_size/32))? indirect_size/8 : words_left*4);
            }
            
            entry_words += indirect_size/32; 
            words_left  -= indirect_size/32;
        }

        if (soc_mem_is_signal(unit, mem) && dynamic_access_orig == 0) {
            /* Disable dynamic memory access */
            *data32 = 0;
            soc_reg_field_set(unit, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, data32, ENABLE_DYNAMIC_MEMORY_ACCESSf, 0);
            if (dcmn_reg_access_with_block(unit, 1, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, blk, data32) != SOC_E_NONE){
                cli_out("unit %d: Failed writing to reg=ENABLE_DYNAMIC_MEMORY_ACCESSr blk=%d (data: %d)\n", unit, blk, *data32);
                goto done;
            }
        }

    } /* finished looping over blocks */

    rv = 0;

 done:
    MEM_UNLOCK(unit, mem);
    return rv;
}
/*
 * Procedures related to PEM block access - 'wide' access.
 * {
 */
int
dpp_do_read_table(int unit, soc_mem_t mem, unsigned array_index,
                  int index, int count, uint32 *entry_ptr)
{
    int       kk, ii;
    int       rv ;
    int       copyno ;
/*    int       dump_disable_cache; - It may be need in feature plan */

    rv = 0 ;
/*    dump_disable_cache = 1 ;*/
    if (mem >= NUM_SOC_MEM) {
        LOG_INFO(BSL_LS_APPL_COMMON,
            (BSL_META_U(unit, "%s(): Illegal mem specifier: %d\n"),
            __FUNCTION__,(int)mem)) ;
        rv = -1 ;
        goto exit ;
    }
    copyno = SOC_MEM_BLOCK_MIN(unit, mem) ;
    if (copyno != SOC_MEM_BLOCK_MAX(unit, mem)) {
        LOG_INFO(BSL_LS_APPL_COMMON,
            (BSL_META_U(unit, "%s(): Memory has more than one block: table %s.%s. Num blocks %d\r\n"),
            __FUNCTION__,SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), (SOC_MEM_BLOCK_MAX(unit, mem) - copyno))) ;
        rv = -2 ;
        goto exit ;
    }

    for (kk = index; kk < index + count; kk++) {
        /* may be needed in feature plan
          else if(!(dump_disable_cache)) {
           ii = soc_mem_array_read(unit, mem, array_index, copyno, kk, entry_ptr) ;
        } else */ {
           ii = soc_mem_array_read_flags(unit, mem, array_index, copyno, kk, entry_ptr, SOC_MEM_DONT_USE_CACHE);
        }
        if (ii < 0) {
            LOG_INFO(BSL_LS_APPL_COMMON,
                (BSL_META_U(unit, "%s(): Read ERROR: table %s.%s[%d]: %s\n"),
                __FUNCTION__,SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), kk, soc_errmsg(ii))) ;
            rv = -3 ;
            goto exit ;
        }
    }
 exit:
    return rv;
}


/*
 * }
 */
#ifdef DCMN_RUNTIME_DEBUG
dcmn_runtime_debug_t dcmn_runtime_debug_per_device[SOC_MAX_NUM_DEVICES] = {{0}};

/* update time counter and print time since start and since last call */
void dcmn_runtime_debug_update_print_time(int unit, const char *string_to_print) {
    unsigned secs_s, mins_s; /* time since start */
    unsigned secs_l, mins_l; /* time since last check */
    dcmn_runtime_debug_t *debug = dcmn_runtime_debug_per_device + unit;
    sal_time_t current_secs = sal_time();
    if (debug->run_stage == dcmn_runtime_debug_state_loading) {
        debug->run_stage = dcmn_runtime_debug_state_initializing;
        debug->last_time = debug->start_time = sal_time();
    }

    secs_s = current_secs - debug->start_time;
    secs_l = current_secs - debug->last_time;
    mins_s = secs_s / 60;
    mins_l = secs_l / 60;
    debug->last_time = current_secs;
    LOG_INFO(BSL_LS_SOC_INIT, ("==> u:%d %s from start: %u:%2.2u:%2.2u  from last: %u:%2.2u:%2.2u\n",
      unit, string_to_print, mins_s / 60, mins_s % 60, secs_s % 60, mins_l / 60, mins_l % 60, secs_l % 60));
}
#endif /* DCMN_RUNTIME_DEBUG */

#undef _ERR_MSG_MODULE_NAME

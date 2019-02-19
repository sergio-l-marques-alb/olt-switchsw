
/*
 * $Id: dnxc_mem.c,v 1.3 Broadcom SDK $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC DNXC MEM
 */
 
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_MEM
#include <shared/bsl.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/dnxc/legacy/dnxc_mem.h>
#include <soc/sand/sand_mem.h>
#include <soc/drv.h>
#include <soc/memory.h>
#include <soc/mem.h>
#include <shared/shrextend/shrextend_debug.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/legacy/drv.h>
#include <soc/dnx/legacy/ARAD/arad_tbl_access.h>
#endif /* BCM_DNX_SUPPORT */
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/ramon/ramon_fabric_cell.h>
#endif /* BCM_DNXF_SUPPORT */

#ifdef JR2_CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#include <soc/dnxc/legacy/dnxc_crash_recovery.h>
#endif /* JR2_CRASH_RECOVERY_SUPPORT */

int dnxc_tbl_is_dynamic(int unit,soc_mem_t mem)
{
#ifdef BCM_DNXF_SUPPORT
    if(SOC_IS_DNXF(unit) && soc_dnxf_tbl_is_dynamic(unit, mem)) {
        return TRUE;
    }
#endif /* BCM_DNXF_SUPPORT */
#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit) && dnx_tbl_is_dynamic(unit, mem)) {
        return TRUE;
    }
#endif /* BCM_DNX_SUPPORT */
    return FALSE;
}

/* check if a given mem contain one of the fields apear in given list*/

int dnxc_mem_contain_one_of_the_fields(int unit,const soc_mem_t mem,soc_field_t *fields)
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
 * fill the memory each entry value will be entry_id 
 * In a case field != INVALIDf the entry will be set to 0 and just the spcified field will be set to entry_id. 
 * This function use DMA doe efficency. 
 */
int dnxc_fill_table_with_index_val(
    int       unit,
    soc_mem_t mem,        /* memory/table to fill */
    soc_field_t field    /*field to set*/
  )
{
    uint32 *entries = NULL, *entry = NULL;
    uint32 field_val[SOC_MAX_MEM_WORDS] = {0};
    uint32 entry_words;
    int blk_index, blk_index_min, blk_index_max;
    int index, index_min, index_max;
    int array_index, array_index_min = 0, array_index_max = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Allocate dma buffer */
    SHR_IF_ERR_EXIT(sand_alloc_dma_mem(unit, 0, (void **) &entries, SOC_MEM_TABLE_BYTES(unit, mem), "dnxc_fill_table_with_index_val"));

    /** get info about table */
    blk_index_min = SOC_MEM_BLOCK_MIN(unit, mem);
    blk_index_max = SOC_MEM_BLOCK_MAX(unit, mem);

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    entry_words = soc_mem_entry_words(unit, mem);

    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        array_index_max = SOC_MEM_NUMELS(unit, mem) - 1;
    }

    /** fill dma memory */
    for (index = index_min; index <= index_max; index++)
    {
        entry = entries + (entry_words * index);
        if (field == INVALIDf)
        {
            *entry = index;
        }
        else
        {
            *field_val = index;
            soc_mem_field_width_fit_set(unit, mem, entry, field, field_val);
        }
    }

    /** set each table instance*/
    for (blk_index = blk_index_min; blk_index <= blk_index_max; blk_index++)
    {
        for (array_index = array_index_min; array_index <= array_index_max; array_index++)
        {
            SHR_IF_ERR_EXIT(soc_mem_array_write_range(unit, 0, mem, array_index, blk_index, index_min, index_max, entries));
        }
    }
    
exit:
    if (entries != NULL)
    {
        SHR_IF_ERR_EXIT(sand_free_dma_mem(unit, 0, (void **) &entries));
    }
    SHR_FUNC_EXIT;
}

/* 
 * This function reads from all cached memories in order to detect and fix SER errors
 */
uint32
soc_dnxc_cache_table_update_all(int unit)
{
    int 
        res = _SHR_E_NONE,
        blk, mem, i ,index_cnt;
    soc_memstate_t *mem_state;
    uint32 rd_data[120];
    
    SHR_FUNC_INIT_VARS(unit);
      
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
                    if (res != _SHR_E_NONE) {
                        MEM_UNLOCK(unit, mem);
                        SHR_IF_ERR_EXIT(res);
                    }
                }
            }
        }
        MEM_UNLOCK(unit, mem);
    } 

exit:
    SHR_FUNC_EXIT; 
}


/*
 * Procedures related to PEM block access - 'wide' access.
 * {
 */
int
dnx_do_read_table(int unit, soc_mem_t mem, unsigned array_index,
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
#ifdef DNXC_RUNTIME_DEBUG
dnxc_runtime_debug_t dnxc_runtime_debug_per_device[SOC_MAX_NUM_DEVICES] = {{0}};

/* update time counter and print time since start and since last call */
void dnxc_runtime_debug_update_print_time(int unit, const char *string_to_print) {
    unsigned secs_s, mins_s; /* time since start */
    unsigned secs_l, mins_l; /* time since last check */
    dnxc_runtime_debug_t *debug = dnxc_runtime_debug_per_device + unit;
    sal_time_t current_secs = sal_time();
    if (debug->run_stage == dnxc_runtime_debug_state_loading) {
        debug->run_stage = dnxc_runtime_debug_state_initializing;
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
#endif /* DNXC_RUNTIME_DEBUG */

#undef BSL_LOG_MODULE

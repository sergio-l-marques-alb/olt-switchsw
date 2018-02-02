/*
 * $Id: soc_dnxc_mem_cache.c, v1 16/06/2014 09:55:39 azarrin $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Purpose:    Implement soc cache memories.
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/mem.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/legacy/drv.h>
#endif /* BCM_DNX_SUPPORT */

#include <soc/dnxc/legacy/error.h>
#include <shared/shrextend/shrextend_debug.h>

/*************
 * DEFINES   *
 *************/
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INTR

/*************
 * FUNCTIONS *
 *************/
int
dnxc_mem_cache_attach(int unit, soc_reg_t mem, int block_instance)
{
    int rc;
    
    SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;
   
    if (soc_mem_is_valid(unit, mem) &&
        ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0)) {

       rc = soc_mem_cache_set(unit, mem, block_instance, 1);
       SHR_IF_ERR_EXIT(rc);

    } else {
            SHR_IF_ERR_EXIT(_SHR_E_UNAVAIL);
    }
   
exit:
    SHR_FUNC_EXIT;
}

int
dnxc_mem_cache_detach(int unit, soc_reg_t mem, int block_instance)
{
    int rc;
  
    SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;
  
    if (soc_mem_is_valid(unit, mem) &&
            ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0)) {
        rc = soc_mem_cache_set(unit, mem, block_instance, 0);
        SHR_IF_ERR_EXIT(rc);

    }
    else {
        SHR_IF_ERR_EXIT(_SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT;
}


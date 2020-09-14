/*
 * $Id: soc_dnxc_mem_cache.c, v1 16/06/2014 09:55:39 azarrin $
 *
 * $Copyright: (c) 2020 Broadcom.
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
#include <soc/sand/sand_mem.h>
#include <shared/shrextend/shrextend_debug.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#endif
#if defined(BCM_DNX_SUPPORT)
#include <soc/dnxc/dnxc_mem.h>
#include <soc/dnx/drv.h>
#endif
#include <soc/dnxc/dnxc_ha.h>

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_dev_init.h>
#endif
/*************
 * DEFINES   *
 *************/
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MEM

/*************
 * FUNCTIONS *
 *************/
/* 
 * Mark all cacheable tables
 */
shr_error_e
soc_dnxc_mem_mark_cachable(
    int unit)
{
    soc_mem_t mem;
#ifdef BCM_DNX_SUPPORT
    const dnxc_data_table_info_t *table_info;
    const dnx_data_dev_init_shadow_uncacheable_mem_t *uncache_mem;
    const dnx_data_intr_ser_static_mem_t *static_mem;
    int mem_index;
    soc_block_type_t block_type;
#endif
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_IF_ERR_EXIT(SOC_E_UNIT);
    }

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        /** Get size of table default size */
        table_info = dnx_data_intr.ser.static_mem_info_get(unit);
        /*
         * Iterate over the table and set value for each mem
         */
        for (mem_index = 0; mem_index < table_info->key_size[0]; mem_index++)
        {
            static_mem = dnx_data_intr.ser.static_mem_get(unit, mem_index);
            if (SOC_MEM_IS_VALID(unit, static_mem->mem))
            {
                SOC_MEM_INFO(unit, static_mem->mem).flags &= ~SOC_MEM_FLAG_SIGNAL;
                LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "Force %s static\n"), SOC_MEM_NAME(unit, static_mem->mem)));
            }
        }
    }
#endif

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (!SOC_MEM_IS_VALID(unit, mem))
        {
            continue;
        }
        /*
         * Skip the Read-only/Write-Only/Signal tables 
         */
        if (sand_tbl_is_dynamic(unit, mem))
        {
            continue;
        }

#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_Q2A(unit))
        {
            block_type = SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem));
            if (block_type == SOC_BLK_FSCL)
            {
                continue;
            }
        }
#endif

        SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "caching %s \n"), SOC_MEM_NAME(unit, mem)));
    }

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        /** Get size of table default size */
        table_info = dnx_data_dev_init.shadow.uncacheable_mem_info_get(unit);
        /*
         * Iterate over the table and set value for each mem
         */
        for (mem_index = 0; mem_index < table_info->key_size[0]; mem_index++)
        {
            uncache_mem = dnx_data_dev_init.shadow.uncacheable_mem_get(unit, mem_index);
            if (SOC_MEM_IS_VALID(unit, uncache_mem->mem))
            {
                SOC_MEM_INFO(unit, uncache_mem->mem).flags &= ~SOC_MEM_FLAG_CACHABLE;
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Skip caching %s \n"), SOC_MEM_NAME(unit, uncache_mem->mem)));
            }
        }
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_mem_cache_init(
    int unit)
{
    soc_mem_t mem, mem_iter;
    uint32 cache_enable = 0;
    uint32 enable_all, enable_parity, enable_ecc, enable_specific, disable_specific;

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        /*
         * don't go to exit label because unit is being used at exit 
         */
        SHR_IF_ERR_RETURN(SOC_E_UNIT);
    }

    /*
     * GET SOC PROPERTY
     */
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        enable_all = dnx_data_dev_init.shadow.cache_enable_all_get(unit);
        enable_parity = dnx_data_dev_init.shadow.cache_enable_parity_get(unit);
        enable_ecc = dnx_data_dev_init.shadow.cache_enable_ecc_get(unit);
        enable_specific = dnx_data_dev_init.shadow.cache_enable_specific_get(unit);
        disable_specific = dnx_data_dev_init.shadow.cache_disable_specific_get(unit);
    }
    else
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        enable_all = dnxf_data_dev_init.shadow.cache_enable_all_get(unit);
        enable_parity = dnxf_data_dev_init.shadow.cache_enable_parity_get(unit);
        enable_ecc = dnxf_data_dev_init.shadow.cache_enable_ecc_get(unit);
        enable_specific = dnxf_data_dev_init.shadow.cache_enable_specific_get(unit);
        disable_specific = dnxf_data_dev_init.shadow.cache_disable_specific_get(unit);

    }
    else
#endif
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_UNIT, "Should be dnx or dnxf device!\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U
               (unit,
                "soc_dpp_cache_enable_init: enable_all %d enable_parity %d enable_specific %d disable_specific %d\n"),
               enable_all, enable_parity, enable_specific, disable_specific));

    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    for (mem_iter = 0; mem_iter < NUM_SOC_MEM; mem_iter++)
    {
        mem = mem_iter;
        SOC_MEM_ALIAS_TO_ORIG(unit, mem);

        if (!SOC_MEM_IS_VALID(unit, mem) || !soc_mem_is_cachable(unit, mem))
        {
            continue;
        }
        /*
         * Skip the "mem_nocache_" memories in soc_property 
         */
        if (enable_all)
        {
            cache_enable = 1;
        }
        else
        {
            if (enable_ecc && SOC_MEM_FIELD_VALID(unit, mem, ECCf))
            {
                cache_enable = 1;
            }
            else if (enable_parity && SOC_MEM_FIELD_VALID(unit, mem, PARITYf))
            {
                cache_enable = 1;
            }
            else
            {
                cache_enable = 0;
            }
        }

        if (cache_enable)
        {
            if (disable_specific && soc_sand_mem_is_in_soc_property(unit, mem, 0))
            {
                cache_enable = 0;
            }
        }
        else if (enable_specific && soc_sand_mem_is_in_soc_property(unit, mem, 1))
        {
            cache_enable = 1;
        }

        /*
         * Turn on cache memory for all tables 
         */
        if (soc_mem_is_valid(unit, mem) && ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0))
        {
            SHR_IF_ERR_EXIT(soc_mem_cache_set(unit, mem, COPYNO_ALL, cache_enable));
        }

    }

exit:
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_mem_cache_deinit(
    int unit)
{
    soc_mem_t mem;
    SHR_FUNC_INIT_VARS(unit);

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (!SOC_MEM_IS_VALID(unit, mem) || !soc_mem_is_cachable(unit, mem))
        {
            continue;
        }

        /*
         * Deallocate table cache memory, if caching enabled 
         */
        if (_SHR_E_FAILURE(soc_mem_cache_set(unit, mem, COPYNO_ALL, 0)))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error to deallocate cache for mem %d\n"), mem));
        }
    }

    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE

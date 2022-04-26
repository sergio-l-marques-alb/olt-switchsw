#ifndef _SOC_DNXC_MEM_CACHE_H_
#define _SOC_DNXC_MEM_CACHE_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (Jr2) and DNXF family only!"
#endif

#include <soc/mcm/allenum.h>

/*
 *  mark static memory cacheable
 */
shr_error_e soc_dnxc_mem_mark_cachable(
    int unit);

/*
 *  List all memories, and set memory cache
 */
shr_error_e soc_dnxc_mem_cache_init(
    int unit);

/*
 *  Deinit cache for each memory
 */
shr_error_e soc_dnxc_mem_cache_deinit(
    int unit);

/*
 * }
 */

#endif /*_SOC_DNXC_MEM_CACHE_H_*/

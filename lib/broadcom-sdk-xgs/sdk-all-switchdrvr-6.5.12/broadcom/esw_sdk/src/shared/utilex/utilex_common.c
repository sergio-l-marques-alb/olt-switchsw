/*
 * $Id: utilex_common.c,v 1.9 Broadcom SDK $
 $Copyright: (c) 2017 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$ 
 */
/** \file utilex_common.c 
 *  
 * All utilities common to all the various utilex services. 
 *  
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

/*************
* INCLUDES  *
*************/
/** { */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <soc/defs.h>
#include <shared/utilex/utilex_common.h>
#include <shared/utilex/utilex_occupation_bitmap.h>
#include <shared/utilex/utilex_hashtable.h>
#include <shared/utilex/utilex_multi_set.h>
#include <shared/utilex/utilex_res_tag_bitmap.h>

/*
 * SW is included
 */
#include <shared/swstate/access/sw_state_access.h>

/** } */

/*************
 * DEFINES   *
 *************/
/** { */

/** } */

#ifdef BCM_DNX_SUPPORT
/*
 * { 
 */
/*
 * These are entry procedure for initialization process as implemented on DNX only.
 */
/*
 * See dnx_utilex_init()
 */
shr_error_e
utilex_init(
    uint32 unit,
    uint32 nof_cores,
    UTILEX_COMMON_SIZES * utilex_common_sizes)
{
    SHR_FUNC_INIT_VARS(unit);
    UTILEX_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(utilex_common_sizes, _SHR_E_INTERNAL, "utilex_common_sizes");
    SHR_IF_ERR_EXIT(utilex_occ_bm_init(unit, utilex_common_sizes->max_nof_dss_per_core * nof_cores));
    SHR_IF_ERR_EXIT(utilex_hash_table_init(unit, utilex_common_sizes->max_nof_hashs_per_core * nof_cores));
    SHR_IF_ERR_EXIT(utilex_multi_set_init(unit, utilex_common_sizes->max_nof_multis_per_core * nof_cores));
    SHR_IF_ERR_EXIT(utilex_res_tag_bitmap_init
                    (unit, utilex_common_sizes->max_nof_res_tag_bitmaps_per_core * nof_cores));
exit:
    SHR_FUNC_EXIT;
}
/*
 */
shr_error_e
utilex_deinit(
    uint32 unit)
{
    SHR_FUNC_INIT_VARS(unit);
    UTILEX_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_IF_ERR_EXIT(utilex_res_tag_bitmap_deinit(unit));
    /*
     * Note that 'multi_set' uses 'hash_table' and 'hash_table'
     * uses 'occ_bm' so the order of deinit must be:
     *   multi_set
     *   hash_table
     *   occupation_bitmap
     */
    SHR_IF_ERR_EXIT(utilex_multi_set_destroy_all(unit));
    SHR_IF_ERR_EXIT(utilex_hash_table_destroy_all(unit, TRUE));
    SHR_IF_ERR_EXIT(utilex_occ_bm_destroy_all(unit, TRUE));
exit:
    SHR_FUNC_EXIT;
}
/*
 * } 
 */
#endif

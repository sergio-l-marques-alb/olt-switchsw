/** \file dnxc_sw_state_common.c
 *  This file holds implementations that are shared between
 *  "plain" and "wb" sw state implemantations.
 */
/*
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */

/*
 * Include files
 * {
 */
#include <assert.h>
#include <shared/bsl.h>
#include <shared/mem_measure_tool.h>
#include <soc/drv.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_plain.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnxc_sw_state_common.h>
/*
 * }
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/** 
 * This array holds all the roots of the sw_state forest 
 * (every xml is generating a root)
 */ 
void**  sw_state_roots_array[SOC_MAX_NUM_DEVICES];

/*
 * see .h file for description
 */
int dnxc_sw_state_memcpy_common(
    int unit,
    uint32 module_id,
    uint32 size,
    uint8 *dest,
    uint8 *src,
    uint32 flags,
    char *dbg_string) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (DNXC_VERIFY_ALLOWED_GET(unit))
    {
        /*
         * Verify input.
         */
        DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_sw_state_memcpy_verify_common(unit, module_id, size, dest, src, flags, dbg_string));
    }

#ifdef BCM_DNX_SUPPORT
    if(dnxc_sw_state_journal_is_done_init(unit) && !sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_memcpy(unit, module_id, size, dest));
    }
#endif /* BCM_DNX_SUPPORT */

    sal_memcpy(dest, src, size);

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_memset_common(
    int unit,
    uint32 module_id,
    uint8 *dest,
    uint32 value,
    uint32 size,
    uint32 flags,
    char *dbg_string) {
        
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (DNXC_VERIFY_ALLOWED_GET(unit))
    {
        /*
         * Verify input.
         */
        DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_sw_state_memset_verify_common(unit, module_id, dest, value, size, flags, dbg_string));
    }

#ifdef BCM_DNX_SUPPORT
    if(dnxc_sw_state_journal_is_done_init(unit) && !sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_memcpy(unit, module_id, size, dest));
    }
#endif /* BCM_DNX_SUPPORT */

    sal_memset(dest, value, size);

    DNXC_SW_STATE_FUNC_RETURN;

}

/*
 * see .h file for description
 */
int dnxc_sw_state_counter_inc_common(
    int unit,
    uint32 module_id,
    uint8 *ptr_location,
    uint32 inc_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string) {
        
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    switch(type_size) {
        case 1: /*uint8*/
#ifdef BCM_DNX_SUPPORT
            if(dnxc_sw_state_journal_is_done_init(unit) && !sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
                SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_memcpy(unit, module_id, 1, ptr_location));
            }
#endif /* BCM_DNX_SUPPORT */
            (*(uint8 *)(ptr_location))+=inc_value;
            break;
        case 4: /*uint32*/
#ifdef BCM_DNX_SUPPORT
            if(dnxc_sw_state_journal_is_done_init(unit) && !sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
                SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_memcpy(unit, module_id, 4, ptr_location));
            }
#endif /* BCM_DNX_SUPPORT */
            (*(uint32 *)(ptr_location))+=inc_value;
            break;
        default:
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: Unsupported counter type \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_counter_dec_common(
    int unit,
    uint32 module_id,
    uint8 *ptr_location,
    uint32 dec_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string){
        
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    switch(type_size) {
        case 1: /*uint8*/
#ifdef BCM_DNX_SUPPORT
            if(dnxc_sw_state_journal_is_done_init(unit) && !sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
                SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_memcpy(unit, module_id, 1, ptr_location));
            }
#endif /* BCM_DNX_SUPPORT */
            (*(uint8 *)(ptr_location))-=dec_value;
            break;
        case 4: /*uint32*/
#ifdef BCM_DNX_SUPPORT
            if(dnxc_sw_state_journal_is_done_init(unit) && !sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
                SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_memcpy(unit, module_id, 4, ptr_location));
            }
#endif /* BCM_DNX_SUPPORT */
            (*(uint32 *)(ptr_location))-=dec_value;
            break;
        default:
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: Unsupported counter type \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_dynamic_allocated_size_get(
    int unit,
    uint32 module_id,
    uint8 *ptr,
    uint32 *allocated_size)
{
    uint32 size = 0;
    uint32 nof_elements_retrieved = 0;
    uint32 element_size_retrieved = 0;

    if(DNXC_SW_STATE_ALLOC_SIZE(unit, module_id, (uint8 *)ptr, &nof_elements_retrieved, &element_size_retrieved) != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META_U(unit, "sw state OUT OF BOUND ERROR: Failed to retrieved size\n")));
        return SOC_E_INTERNAL;
    }
    size = nof_elements_retrieved * element_size_retrieved;
    *allocated_size = size;

    return SOC_E_NONE;
}
